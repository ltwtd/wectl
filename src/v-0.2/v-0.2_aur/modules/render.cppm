module;
#include <array>
#include <atomic>
#include <cctype>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <filesystem>
#include <mutex>
#include <string>
#include <string_view>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>

export module wectl.render;
import wectl.core;
import wectl.ipc;

export namespace wectl::render {
namespace fs = std::filesystem;
using namespace wectl::core;
using namespace wectl::ipc;

enum class RenderState : uint8_t { Stopped = 0, Running = 1, Frozen = 2 };
enum class MediaKind   : uint8_t { None = 0, Scene = 1, Video = 2, Photo = 3 };

struct ResolvedTarget {
    MediaKind kind = MediaKind::None;
    fs::path path;
    std::string id;
};

inline std::atomic<pid_t> g_pid{-1};
inline std::atomic<RenderState> g_state{RenderState::Stopped};
inline std::atomic<MediaKind> g_kind{MediaKind::None};
inline std::mutex g_render_mtx;

inline std::string lower_ext(const fs::path& p) {
    std::string e = p.extension().string();
    for (char& c : e) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return e;
}

inline bool is_video_ext(std::string_view ext) {
    return ext == ".mp4" || ext == ".webm" || ext == ".mkv" || ext == ".mov" || ext == ".avi" || ext == ".gif";
}

inline bool is_photo_ext(std::string_view ext) {
    return ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".webp" || ext == ".jxl" || ext == ".bmp";
}

inline std::string find_tool_bin(const char* name) {
    std::error_code ec;
    const fs::path local = root_dir() / "bin" / name;
    if (fs::exists(local, ec)) return local.string();
    const std::string usr_bin = std::string("/usr/bin/") + name;
    if (access(usr_bin.c_str(), X_OK) == 0) return usr_bin;
    const std::string local_usr = std::string("/usr/local/bin/") + name;
    if (access(local_usr.c_str(), X_OK) == 0) return local_usr;
    return name;
}

inline bool is_our_wallpaper_process(pid_t pid) {
    if (pid <= 1) return false;
    std::array<char, 64> path{};
    std::snprintf(path.data(), path.size(), "/proc/%d/cmdline", pid);
    const int fd = open(path.data(), O_RDONLY | O_CLOEXEC);
    if (fd < 0) return false;

    std::array<char, 2048> cmd{};
    const ssize_t n = read(fd, cmd.data(), cmd.size() - 1);
    close(fd);
    if (n <= 0) return false;

    for (ssize_t i = 0; i < n; ++i) {
        if (cmd.at(static_cast<size_t>(i)) == '\0') cmd.at(static_cast<size_t>(i)) = ' ';
    }
    cmd.at(static_cast<size_t>(n)) = '\0';

    const std::string root_str = root_dir().string();
    if (std::strstr(cmd.data(), root_str.c_str()) == nullptr) return false;

    return std::strstr(cmd.data(), "linux-wallpaperengine") != nullptr ||
           std::strstr(cmd.data(), "mpvpaper") != nullptr ||
           std::strstr(cmd.data(), "swaybg") != nullptr;
}

inline pid_t find_renderer_pid() {
    const pid_t cached = g_pid.load(std::memory_order_acquire);
    if (cached > 0 && kill(cached, 0) == 0 && is_our_wallpaper_process(cached)) return cached;

    DIR* dir = opendir("/proc");
    if (!dir) return -1;

    pid_t found = -1;
    dirent* ent;
    while ((ent = readdir(dir)) != nullptr) {
        if (*ent->d_name < '0' || *ent->d_name > '9') continue;
        const pid_t pid = static_cast<pid_t>(std::atoi(ent->d_name));
        if (is_our_wallpaper_process(pid)) {
            found = pid;
            break;
        }
    }
    closedir(dir);
    g_pid.store(found, std::memory_order_release);
    return found;
}

inline void purge_all_renderers_unlocked() {
    const pid_t cached = g_pid.load(std::memory_order_acquire);
    if (cached > 0) {
        kill(cached, SIGKILL);
        waitpid(cached, nullptr, WNOHANG);
    }

    DIR* dir = opendir("/proc");
    if (dir) {
        dirent* ent;
        while ((ent = readdir(dir)) != nullptr) {
            if (*ent->d_name < '0' || *ent->d_name > '9') continue;
            const pid_t pid = static_cast<pid_t>(std::atoi(ent->d_name));
            if (is_our_wallpaper_process(pid)) {
                kill(pid, SIGKILL);
                waitpid(pid, nullptr, WNOHANG);
            }
        }
        closedir(dir);
    }
    g_pid.store(-1, std::memory_order_release);
    g_state.store(RenderState::Stopped, std::memory_order_release);
    g_kind.store(MediaKind::None, std::memory_order_release);
}

inline void freeze_on_current_frame() {
    std::lock_guard<std::mutex> lock(g_render_mtx);
    const pid_t pid = find_renderer_pid();
    if (pid > 0 && kill(pid, SIGSTOP) == 0) {
        g_state.store(RenderState::Frozen, std::memory_order_release);
    }
}

inline void kill_renderer() {
    std::lock_guard<std::mutex> lock(g_render_mtx);
    purge_all_renderers_unlocked();
}

inline void stop_all_services() {
    kill_renderer();
    std::system("systemctl --user stop wectl.service >/dev/null 2>&1 || true");
}

inline ResolvedTarget resolve_media(std::string_view input) {
    if (input.empty()) return {};
    std::error_code ec;

    if (input.rfind("scene:", 0) == 0) {
        fs::path p = scenes_dir() / input.substr(6);
        if (fs::is_directory(p, ec)) return {MediaKind::Scene, fs::canonical(p, ec), "scene:" + p.filename().string()};
    } else if (input.rfind("vid:", 0) == 0) {
        fs::path p = vids_dir() / input.substr(4);
        if (fs::is_regular_file(p, ec)) return {MediaKind::Video, fs::canonical(p, ec), "vid:" + p.filename().string()};
    } else if (input.rfind("photo:", 0) == 0) {
        fs::path p = photos_dir() / input.substr(6);
        if (fs::is_regular_file(p, ec)) return {MediaKind::Photo, fs::canonical(p, ec), "photo:" + p.filename().string()};
    }

    fs::path direct(input);
    if (fs::exists(direct, ec)) {
        fs::path c = fs::canonical(direct, ec);
        if (fs::is_directory(c, ec)) return {MediaKind::Scene, c, "scene:" + c.filename().string()};
        const std::string ext = lower_ext(c);
        if (is_video_ext(ext)) return {MediaKind::Video, c, "vid:" + c.filename().string()};
        if (is_photo_ext(ext)) return {MediaKind::Photo, c, "photo:" + c.filename().string()};
    }

    if (fs::is_directory(scenes_dir() / direct, ec)) {
        fs::path c = fs::canonical(scenes_dir() / direct, ec);
        return {MediaKind::Scene, c, "scene:" + c.filename().string()};
    }
    if (fs::is_regular_file(vids_dir() / direct, ec)) {
        fs::path c = fs::canonical(vids_dir() / direct, ec);
        return {MediaKind::Video, c, "vid:" + c.filename().string()};
    }
    if (fs::is_regular_file(photos_dir() / direct, ec)) {
        fs::path c = fs::canonical(photos_dir() / direct, ec);
        return {MediaKind::Photo, c, "photo:" + c.filename().string()};
    }
    return {};
}

inline pid_t spawn_media_process(const ResolvedTarget& t, int fps) {
    std::string mon_str = load_hw().monitor;
    const std::string jmon = query_socket("j/monitors");
    if (!jmon.empty() && jmon.find("\"" + mon_str + "\"") == std::string::npos) {
        const size_t kpos = jmon.find("\"name\":");
        if (kpos != std::string::npos) {
            const size_t q1 = jmon.find('"', kpos + 7);
            const size_t q2 = (q1 != std::string::npos) ? jmon.find('"', q1 + 1) : std::string::npos;
            if (q1 != std::string::npos && q2 != std::string::npos && q2 > q1 + 1) {
                mon_str = jmon.substr(q1 + 1, q2 - q1 - 1);
            }
        }
    }
    const std::string path_str = t.path.string();
    const std::string fps_str = std::to_string(fps);

    const pid_t pid = fork();
    if (pid == 0) {
        prctl(PR_SET_PDEATHSIG, SIGKILL);
        const int devnull = open("/dev/null", O_RDWR);
        if (devnull >= 0) {
            dup2(devnull, STDIN_FILENO);
            dup2(devnull, STDOUT_FILENO);
            dup2(devnull, STDERR_FILENO);
            if (devnull > 2) close(devnull);
        }

        if (t.kind == MediaKind::Scene) {
            const std::string bin_str = find_tool_bin("linux-wallpaperengine");
            std::array<const char*, 12> argv = {
                bin_str.c_str(),
                "--screen-root", mon_str.c_str(),
                "--fps", fps_str.c_str(),
                "--scaling", "fill",
                "--silent",
                "--no-fullscreen-pause",
                path_str.c_str(),
                nullptr
            };
            execvp(bin_str.c_str(), const_cast<char* const*>(argv.data()));
        } else if (t.kind == MediaKind::Video) {
            const std::string bin_str = find_tool_bin("mpvpaper");
            const std::string mpv_opts = "hwdec=auto-safe no-audio loop-file=inf panscan=1.0 video-sync=display-resample";
            std::array<const char*, 6> argv = {
                bin_str.c_str(),
                "-o", mpv_opts.c_str(),
                mon_str.c_str(),
                path_str.c_str(),
                nullptr
            };
            execvp(bin_str.c_str(), const_cast<char* const*>(argv.data()));
        } else if (t.kind == MediaKind::Photo) {
            const std::string bin_str = find_tool_bin("swaybg");
            std::array<const char*, 6> argv = {
                bin_str.c_str(),
                "-i", path_str.c_str(),
                "-m", "fill",
                nullptr
            };
            execvp(bin_str.c_str(), const_cast<char* const*>(argv.data()));
        }
        _exit(127);
    }
    return pid;
}

inline void launch_wallpaper(std::string_view input, int fps_in = -1);

inline void launch_wallpaper_unlocked(std::string_view input, int fps_in = -1, bool seamless_swap = true) {
    const ResolvedTarget t = resolve_media(input);
    if (t.kind == MediaKind::None) return;

    const std::string prev_id = read_file(state_dir() / "current", "");
    const int raw_fps = (fps_in > 0) ? fps_in : std::atoi(read_file(state_dir() / "fps", "60").c_str());
    const int fps = clamp_fps(raw_fps);

    write_file(state_dir() / "current", t.id);
    write_file(state_dir() / "fps", std::to_string(fps));
    write_file(state_dir() / "scale", "fill");

    const pid_t old_pid = find_renderer_pid();
    const MediaKind old_kind = g_kind.load(std::memory_order_acquire);
    const bool is_same_media = (prev_id == t.id);
    const bool allow_overlap = seamless_swap && !is_same_media &&
                               (old_kind == MediaKind::Scene) && (t.kind == MediaKind::Scene);

    if (!allow_overlap) {
        purge_all_renderers_unlocked();
        if (old_pid > 0) {
            for (int i = 0; i < 20 && kill(old_pid, 0) == 0; ++i) {
                std::this_thread::sleep_for(std::chrono::milliseconds(15));
            }
        }
    }

    const pid_t new_pid = spawn_media_process(t, fps);
    if (new_pid > 0) {
        g_pid.store(new_pid, std::memory_order_release);
        g_kind.store(t.kind, std::memory_order_release);
        g_state.store(RenderState::Running, std::memory_order_release);

        if (allow_overlap && old_pid > 0 && old_pid != new_pid) {
            std::thread([old_pid, new_pid]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(700));
                kill(old_pid, SIGKILL);
                waitpid(old_pid, nullptr, WNOHANG);
                std::this_thread::sleep_for(std::chrono::milliseconds(150));
                if (kill(new_pid, 0) != 0) {
                    const std::string cur = read_file(state_dir() / "current", "");
                    if (!cur.empty()) launch_wallpaper(cur);
                }
            }).detach();
        }
    }
}

inline void launch_wallpaper(std::string_view input, int fps_in) {
    std::lock_guard<std::mutex> lock(g_render_mtx);
    launch_wallpaper_unlocked(input, fps_in, true);
}

inline void sync_fullscreen_state(bool fs_active) {
    std::lock_guard<std::mutex> lock(g_render_mtx);
    const RenderState st = g_state.load(std::memory_order_acquire);
    const MediaKind kd = g_kind.load(std::memory_order_acquire);
    const std::string saved = read_file(state_dir() / "current", "");

    if (fs_active) {
        if (st == RenderState::Running) {
            if (kd == MediaKind::Photo) {
                purge_all_renderers_unlocked();
                g_state.store(RenderState::Frozen, std::memory_order_release);
                g_kind.store(MediaKind::Photo, std::memory_order_release);
            } else {
                const pid_t pid = find_renderer_pid();
                if (pid > 0 && kill(pid, SIGSTOP) == 0) {
                    g_state.store(RenderState::Frozen, std::memory_order_release);
                }
            }
        }
    } else if (st == RenderState::Frozen) {
        const pid_t pid = find_renderer_pid();
        if (pid > 0 && kd != MediaKind::Photo && kill(pid, SIGCONT) == 0) {
            g_state.store(RenderState::Running, std::memory_order_release);
        } else if (!saved.empty()) {
            launch_wallpaper_unlocked(saved, -1, false);
        } else {
            purge_all_renderers_unlocked();
        }
    }
}

inline void start_autopause_worker() {
    std::thread([]() {
        int fd = -1;
        while ((fd = open_event_stream()) < 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }

        sync_fullscreen_state(is_fullscreen_active());

        std::array<char, 512> buf{};
        std::string acc;
        acc.reserve(512);
        while (true) {
            const ssize_t n = read(fd, buf.data(), buf.size());
            if (n <= 0) {
                close(fd);
                while ((fd = open_event_stream()) < 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                }
                acc.clear();
                sync_fullscreen_state(is_fullscreen_active());
                continue;
            }
            acc.append(buf.data(), static_cast<size_t>(n));
            size_t pos;
            while ((pos = acc.find('\n')) != std::string::npos) {
                std::string_view ev(acc.data(), pos);
                if (ev == "fullscreen>>1") {
                    sync_fullscreen_state(true);
                } else if (ev == "fullscreen>>0") {
                    sync_fullscreen_state(false);
                } else if (ev.rfind("workspace>>", 0) == 0 ||
                           ev.rfind("activewindow>>", 0) == 0 ||
                           ev.rfind("closewindow>>", 0) == 0) {
                    sync_fullscreen_state(is_fullscreen_active());
                }
                acc.erase(0, pos + 1);
            }
            if (acc.capacity() > 4096) acc.shrink_to_fit();
        }
    }).detach();
}
inline bool is_renderer_frozen(pid_t pid) {
    if (pid <= 0) return false;
    std::array<char, 64> stat_path{};
    std::snprintf(stat_path.data(), stat_path.size(), "/proc/%d/stat", pid);
    const int fd = open(stat_path.data(), O_RDONLY | O_CLOEXEC);
    if (fd < 0) return false;
    std::array<char, 256> buf{};
    const ssize_t n = read(fd, buf.data(), buf.size() - 1);
    close(fd);
    if (n <= 0) return false;
    std::string_view sv(buf.data(), static_cast<size_t>(n));
    const size_t rp = sv.rfind(')');
    return (rp != std::string_view::npos && rp + 2 < sv.size() && sv.at(rp + 2) == 'T');
}

inline bool toggle_freeze_frame() {
    std::lock_guard<std::mutex> lock(g_render_mtx);
    const pid_t pid = find_renderer_pid();
    if (pid <= 0) return false;

    if (is_renderer_frozen(pid)) {
        if (kill(pid, SIGCONT) == 0) {
            g_state.store(RenderState::Running, std::memory_order_release);
            return false; // Теперь активно (не заморожено)
        }
    } else {
        if (kill(pid, SIGSTOP) == 0) {
            g_state.store(RenderState::Frozen, std::memory_order_release);
            return true; // Теперь заморожено
        }
    }
    return false;
}

}

