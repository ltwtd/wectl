module;
#include <cstdlib>
#include <filesystem>
#include <string>
#include <string_view>
#include <thread>
#include <unistd.h>

export module wectl.render;
import wectl.core;
import wectl.ipc;

export namespace wectl::render {
namespace fs = std::filesystem;
using namespace wectl::core;
using namespace wectl::ipc;

inline std::string sh_quote(std::string_view s) {
    std::string o;
    o.reserve(s.size() + 8);
    o.push_back('\'');
    for (char c : s) {
        if (c == '\'') o += "'\\''";
        else o.push_back(c);
    }
    o.push_back('\'');
    return o;
}

inline void kill_renderer() {
    std::system("pkill -CONT -f wallpaperengine >/dev/null 2>&1 || true");
    std::system("pkill -9 -f wallpaperengine >/dev/null 2>&1 || true");
    std::system("killall -9 linux-wallpaperengine >/dev/null 2>&1 || true");
}

inline void stop_wallpaper_session() {
    kill_renderer();
    write_file(state_dir() / "current", "");
}

inline void stop_all_services() {
    kill_renderer();
    std::system("systemctl --user stop wectl.service >/dev/null 2>&1 || true");
    std::system("pkill -9 -f 'wectl daemon' >/dev/null 2>&1 || true");
}

inline fs::path resolve_scene(std::string_view input) {
    if (input.empty()) return {};
    fs::path p(input);
    if (fs::exists(p) && fs::is_directory(p)) return fs::canonical(p);
    fs::path by_name = scenes_dir() / p;
    if (fs::exists(by_name) && fs::is_directory(by_name)) return fs::canonical(by_name);
    return {};
}

inline void launch_wallpaper(std::string_view scene_input, int fps_in = -1) {
    const fs::path sp = resolve_scene(scene_input);
    if (sp.empty()) return;

    const int raw_fps = (fps_in > 0) ? fps_in : std::atoi(read_file(state_dir() / "fps", "60").c_str());
    const int fps = clamp_fps(raw_fps);

    write_file(state_dir() / "current", sp.filename().string());
    write_file(state_dir() / "fps", std::to_string(fps));
    write_file(state_dir() / "scale", "fill");

    kill_renderer();

    fs::path local_bin = root_dir() / "bin" / "linux-wallpaperengine";
    const std::string bin_str = fs::exists(local_bin) ? local_bin.string() : "linux-wallpaperengine";
    const std::string cmd = "nohup " + sh_quote(bin_str) +
                            " --screen-root " + sh_quote(load_hw().monitor) +
                            " --fps " + std::to_string(fps) +
                            " --scaling fill --silent " + sh_quote(sp.string()) +
                            " >/dev/null 2>&1 &";
    std::system(cmd.c_str());
}

inline void apply_pause_state(bool should_pause, bool& current_state) {
    if (should_pause == current_state) return;
    std::system(should_pause ? "pkill -STOP -f wallpaperengine >/dev/null 2>&1 || true"
                             : "pkill -CONT -f wallpaperengine >/dev/null 2>&1 || true");
    current_state = should_pause;
}

inline void start_autopause_worker() {
    std::thread([]() {
        const int fd = open_event_stream();
        if (fd < 0) return;

        bool paused = false;
        apply_pause_state(is_fullscreen_active(), paused);

        char buf[512];
        std::string acc;
        acc.reserve(512);
        while (true) {
            const ssize_t n = read(fd, buf, sizeof(buf));
            if (n <= 0) break;
            acc.append(buf, static_cast<size_t>(n));
            size_t pos;
            while ((pos = acc.find('\n')) != std::string::npos) {
                std::string_view ev(acc.data(), pos);
                if (ev.rfind("fullscreen>>", 0) == 0 || ev.rfind("workspace>>", 0) == 0 ||
                    ev.rfind("activewindow>>", 0) == 0 || ev.rfind("closewindow>>", 0) == 0) {
                    apply_pause_state(is_fullscreen_active(), paused);
                }
                acc.erase(0, pos + 1);
            }
            if (acc.capacity() > 4096) acc.shrink_to_fit();
        }
        close(fd);
    }).detach();
}

}
