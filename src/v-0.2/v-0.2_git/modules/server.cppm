module;
#include <arpa/inet.h>
#include <array>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <fcntl.h>
#include <filesystem>
#include <mutex>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string>
#include <string_view>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

export module wectl.server;
import wectl.core;
import wectl.ipc;
import wectl.render;
import wectl.ui;

export namespace wectl::server {
namespace fs = std::filesystem;
using namespace wectl::core;
using namespace wectl::ipc;
using namespace wectl::render;
using namespace wectl::ui;

inline std::mutex g_thumb_mtx;

inline std::string url_decode(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '%' && i + 2 < s.size()) {
            std::array<char, 3> hex{s[i + 1], s[i + 2], 0};
            out.push_back(static_cast<char>(std::strtol(hex.data(), nullptr, 16)));
            i += 2;
        } else if (s[i] == '+') {
            out.push_back(' ');
        } else {
            out.push_back(s[i]);
        }
    }
    return out;
}

inline std::string query_param(std::string_view query, std::string_view key) {
    size_t start = 0;
    while (start < query.size()) {
        size_t amp = query.find('&', start);
        if (amp == std::string_view::npos) amp = query.size();
        std::string_view kv = query.substr(start, amp - start);
        size_t eq = kv.find('=');
        if (eq != std::string_view::npos && kv.substr(0, eq) == key) {
            return url_decode(kv.substr(eq + 1));
        }
        start = amp + 1;
    }
    return {};
}

inline void send_ok(int cli) {
    constexpr std::string_view resp = "HTTP/1.1 200 OK\r\nContent-Length: 2\r\nConnection: close\r\n\r\nok";
    write(cli, resp.data(), resp.size());
}

inline const char* mime_for_ext(std::string_view ext) {
    if (ext == ".gif")  return "image/gif";
    if (ext == ".png")  return "image/png";
    if (ext == ".webp") return "image/webp";
    if (ext == ".mp4")  return "video/mp4";
    if (ext == ".webm") return "video/webm";
    if (ext == ".mkv")  return "video/x-matroska";
    return "image/jpeg";
}

inline void stream_file_zero_copy(int cli, const fs::path& fp, const char* mime) {
    const int fd = open(fp.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        constexpr std::string_view resp = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        write(cli, resp.data(), resp.size());
        return;
    }
    struct stat st{};
    if (fstat(fd, &st) < 0 || !S_ISREG(st.st_mode)) {
        close(fd);
        constexpr std::string_view resp = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
        write(cli, resp.data(), resp.size());
        return;
    }
    const std::string hdr = "HTTP/1.1 200 OK\r\nContent-Type: " + std::string(mime) +
                            "\r\nContent-Length: " + std::to_string(st.st_size) +
                            "\r\nCache-Control: max-age=86400\r\nConnection: close\r\n\r\n";
    write(cli, hdr.data(), hdr.size());
    off_t offset = 0;
    size_t left = static_cast<size_t>(st.st_size);
    while (left > 0) {
        const ssize_t sent = sendfile(cli, fd, &offset, left);
        if (sent <= 0) break;
        left -= static_cast<size_t>(sent);
    }
    close(fd);
}

inline void ensure_video_thumb(const fs::path& vid_path, const fs::path& jpg_path) {
    std::error_code ec;
    if (fs::exists(jpg_path, ec) && fs::file_size(jpg_path, ec) > 0) return;

    std::lock_guard<std::mutex> lock(g_thumb_mtx);
    if (fs::exists(jpg_path, ec) && fs::file_size(jpg_path, ec) > 0) return;
    fs::create_directories(jpg_path.parent_path(), ec);

    const std::string in_s = vid_path.string();
    const std::string out_s = jpg_path.string();
    const pid_t pid = fork();
    if (pid == 0) {
        const int devnull = open("/dev/null", O_RDWR);
        if (devnull >= 0) {
            dup2(devnull, STDIN_FILENO);
            dup2(devnull, STDOUT_FILENO);
            dup2(devnull, STDERR_FILENO);
            if (devnull > 2) close(devnull);
        }
        std::array<const char*, 17> argv = {
            "ffmpeg", "-y", "-hide_banner", "-loglevel", "error",
            "-ss", "00:00:00",
            "-i", in_s.c_str(),
            "-frames:v", "1",
            "-vf", "scale=480:-1",
            "-q:v", "5",
            out_s.c_str(),
            nullptr
        };
        execvp("ffmpeg", const_cast<char* const*>(argv.data()));
        _exit(127);
    } else if (pid > 0) {
        waitpid(pid, nullptr, 0);
    }
}

inline void warm_video_thumbs_async() {
    std::thread([]() {
        std::error_code ec;
        if (!fs::exists(vids_dir(), ec)) return;
        for (const auto& e : fs::directory_iterator(vids_dir(), ec)) {
            if (!e.is_regular_file(ec)) continue;
            const fs::path thumb_fp = state_dir() / "thumbs" / (e.path().filename().string() + ".jpg");
            ensure_video_thumb(e.path(), thumb_fp);
        }
    }).detach();
}

inline void handle_client(int cli) {
    int one = 1;
    setsockopt(cli, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));

    std::array<char, 4096> buf{};
    const ssize_t n = read(cli, buf.data(), buf.size() - 1);
    if (n <= 0) {
        close(cli);
        return;
    }

    std::string_view req(buf.data(), static_cast<size_t>(n));
    const size_t sp1 = req.find(' ');
    if (sp1 == std::string_view::npos) {
        close(cli);
        return;
    }
    const size_t sp2 = req.find(' ', sp1 + 1);
    if (sp2 == std::string_view::npos) {
        close(cli);
        return;
    }

    std::string_view target = req.substr(sp1 + 1, sp2 - sp1 - 1);
    std::string_view path = target;
    std::string_view query;
    if (const size_t q = target.find('?'); q != std::string_view::npos) {
        path = target.substr(0, q);
        query = target.substr(q + 1);
    }

    if (path == "/set") {
        const std::string s = query_param(query, "s");
        const std::string f = query_param(query, "fps");
        launch_wallpaper(s, f.empty() ? -1 : std::atoi(f.c_str()));
        send_ok(cli);
    } else if (path == "/fps") {
        const std::string f = query_param(query, "v");
        if (!f.empty()) write_file(state_dir() / "fps", std::to_string(clamp_fps(std::atoi(f.c_str()))));
        send_ok(cli);
    } else if (path == "/hide") {
        const std::string s = query_param(query, "s");
        const ResolvedTarget t = resolve_media(s);
        std::error_code ec;
        if (t.kind != MediaKind::None && fs::exists(t.path, ec)) {
            fs::path dst_dir = scenes_hidden_dir();
            if (t.kind == MediaKind::Video) {
                dst_dir = vids_hidden_dir();
                const std::string fn = t.path.filename().string();
                fs::remove(state_dir() / "thumbs" / (fn + ".jpg"), ec);
                fs::remove(state_dir() / "meta" / (fn + ".res"), ec);
            } else if (t.kind == MediaKind::Photo) {
                dst_dir = photos_hidden_dir();
            }

            fs::create_directories(dst_dir, ec);
            fs::rename(t.path, dst_dir / t.path.filename(), ec);
        }
        send_ok(cli);
    } else if (path.rfind("/vthumb/", 0) == 0) {
        const std::string fn = url_decode(path.substr(8));
        const fs::path vid_fp = vids_dir() / fn;
        const fs::path thumb_fp = state_dir() / "thumbs" / (fn + ".jpg");
        std::error_code ec;
        if (fn.find("..") == std::string::npos && fs::exists(vid_fp, ec)) {
            ensure_video_thumb(vid_fp, thumb_fp);
            stream_file_zero_copy(cli, thumb_fp, "image/jpeg");
        } else {
            constexpr std::string_view resp = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
            write(cli, resp.data(), resp.size());
        }
    } else if (path.rfind("/media/", 0) == 0) {
        const std::string rel = url_decode(path.substr(7));
        const fs::path fp = root_dir() / "wallpapers" / rel;
        if (rel.find("..") == std::string::npos) {
            stream_file_zero_copy(cli, fp, mime_for_ext(fp.extension().string()));
        } else {
            constexpr std::string_view resp = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
            write(cli, resp.data(), resp.size());
        }
    } else {
        const std::string body = render_gallery();
        const std::string hdr = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nContent-Length: " +
                                std::to_string(body.size()) + "\r\nConnection: close\r\n\r\n";
        write(cli, hdr.data(), hdr.size());
        write(cli, body.data(), body.size());
    }
    close(cli);
}

inline void on_sig(int) {
    purge_all_renderers_unlocked();
    _exit(0);
}

inline void run_daemon() {
    std::signal(SIGTERM, on_sig);
    std::signal(SIGINT, on_sig);
    std::signal(SIGCHLD, SIG_IGN);

    const std::string mon = load_hw().monitor;
    for (int i = 0; i < 10 && query_socket("j/monitors").find(mon) == std::string::npos; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    int srv = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(43197);

    if (bind(srv, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) return;
    listen(srv, 32);

    {
        std::string saved = read_file(state_dir() / "current", "");
        while (!saved.empty() && (saved.back() == '\n' || saved.back() == '\r' || saved.back() == ' ')) saved.pop_back();
        if (saved.empty()) {
            saved = "scene:4K SCI-FI Black Hole";
            std::error_code ec;
            if (!fs::exists(scenes_dir() / "4K SCI-FI Black Hole", ec) && fs::exists(scenes_dir(), ec)) {
                for (const auto& e : fs::directory_iterator(scenes_dir(), ec)) {
                    if (e.is_directory(ec)) {
                        saved = "scene:" + e.path().filename().string();
                        break;
                    }
                }
            }
        }
        if (!saved.empty()) launch_wallpaper(saved);
    }

    start_autopause_worker();
    warm_video_thumbs_async();

    while (true) {
        int cli = accept4(srv, nullptr, nullptr, SOCK_CLOEXEC);
        if (cli < 0) continue;
        std::thread(handle_client, cli).detach();
    }
}

}

