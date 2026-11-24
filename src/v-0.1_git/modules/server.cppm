module;
#include <arpa/inet.h>
#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <netinet/in.h>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <unistd.h>

export module wectl.server;
import wectl.core;
import wectl.render;
import wectl.ui;

export namespace wectl::server {
namespace fs = std::filesystem;
using namespace wectl::core;
using namespace wectl::render;
using namespace wectl::ui;

inline std::string url_decode(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '%' && i + 2 < s.size()) {
            char hex[3] = {s[i + 1], s[i + 2], 0};
            out.push_back(static_cast<char>(std::strtol(hex, nullptr, 16)));
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

inline void handle_client(int cli) {
    char buf[4096] = {0};
    const ssize_t n = read(cli, buf, sizeof(buf) - 1);
    if (n <= 0) return;

    std::string_view req(buf, static_cast<size_t>(n));
    const size_t sp1 = req.find(' ');
    if (sp1 == std::string_view::npos) return;
    const size_t sp2 = req.find(' ', sp1 + 1);
    if (sp2 == std::string_view::npos) return;

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
    } else if (path == "/stop") {
        stop_wallpaper_session();
        send_ok(cli);
    } else if (path == "/hide") {
        const std::string s = query_param(query, "s");
        const fs::path src = scenes_dir() / s;
        if (!s.empty() && fs::exists(src)) {
            fs::create_directories(hidden_dir());
            fs::rename(src, hidden_dir() / s);
        }
        send_ok(cli);
    } else if (path.rfind("/thumb/", 0) == 0) {
        const std::string rel = url_decode(path.substr(7));
        const fs::path fp = scenes_dir() / rel;
        if (fs::exists(fp) && fs::is_regular_file(fp)) {
            std::ifstream f(fp, std::ios::binary);
            std::string body((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
            const std::string ext = fp.extension().string();
            const char* mime = (ext == ".gif") ? "image/gif" : (ext == ".png") ? "image/png" : "image/jpeg";
            const std::string hdr = "HTTP/1.1 200 OK\r\nContent-Type: " + std::string(mime) +
                                    "\r\nContent-Length: " + std::to_string(body.size()) +
                                    "\r\nCache-Control: max-age=86400\r\nConnection: close\r\n\r\n";
            write(cli, hdr.data(), hdr.size());
            write(cli, body.data(), body.size());
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
}

inline void on_sig(int) {
    kill_renderer();
    _exit(0);
}

inline void run_daemon() {
    std::signal(SIGTERM, on_sig);
    std::signal(SIGINT, on_sig);
    start_autopause_worker();

    int srv = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(43197);

    if (bind(srv, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) return;
    listen(srv, 16);

    {
        const std::string saved = read_file(state_dir() / "current", "4K SCI-FI Black Hole");
        if (!saved.empty()) launch_wallpaper(saved);
    }

    while (true) {
        int cli = accept(srv, nullptr, nullptr);
        if (cli < 0) continue;
        handle_client(cli);
        close(cli);
    }
}

}
