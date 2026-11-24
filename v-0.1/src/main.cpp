#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

import wectl.banner;
import wectl.core;
import wectl.render;
import wectl.server;

namespace fs = std::filesystem;
using namespace wectl::banner;
using namespace wectl::core;
using namespace wectl::render;
using namespace wectl::server;

int main(int argc, char** argv) {
    const std::string cmd = (argc > 1) ? *(argv + 1) : "-h";
    const auto& hw = load_hw();

    if (cmd == "daemon") {
        run_daemon();
        return 0;
    }

    print_logo();

    if (cmd == "-h" || cmd == "--help") {
        std::cout << "Root:    " << root_dir().string() << "\n"
                  << "Display: " << hw.monitor << " (" << hw.resolution << " @ max " << hw.max_hz << "Hz)\n\n"
                  << "Usage: wectl <command | scene_name>\n\n"
                  << "Commands:\n"
                  << "  up             Start daemon and open web gallery (http://127.0.0.1:43197)\n"
                  << "  status         Show daemon status, active scene, FPS, and GPU renderer state\n"
                  << "  stop           Stop wallpaper rendering (0% GPU) while keeping daemon idle\n"
                  << "  down           Stop both wallpaper rendering and wectl daemon\n"
                  << "  restart        Restart wectl.service\n"
                  << "  restore        Restore last selected wallpaper\n"
                  << "  uninstall      Completely remove wectl, its service, symlinks, and directory\n"
                  << "  <scene_name>   Launch a specific scene\n"
                  << "  -h, --help     Show this help message\n";
    } else if (cmd == "status") {
        std::cout << "Root:    " << root_dir().string() << "\n"
                  << "Display: " << hw.monitor << " | " << hw.resolution << " | Max " << hw.max_hz << "Hz | fill\n";
        std::system("systemctl --user is-active --quiet wectl.service && echo 'Service: ACTIVE (http://127.0.0.1:43197)' || echo 'Service: STOPPED'");
        std::cout << "Scene:   " << read_file(state_dir() / "current", "none") << "\n"
                  << "FPS:     " << clamp_fps(std::atoi(read_file(state_dir() / "fps", "60").c_str())) << " FPS\n";
        std::system("pgrep -f wallpaperengine >/dev/null && ps -o pid,stat,rss,comm -C linux-wallpaperengine || echo 'Render:  STOPPED (0% GPU)'");
    } else if (cmd == "up") {
        std::system("systemctl --user start wectl.service");
        std::system("xdg-open http://127.0.0.1:43197 >/dev/null 2>&1 &");
        std::cout << "Gallery: http://127.0.0.1:43197\n";
    } else if (cmd == "stop") {
        stop_wallpaper_session();
        std::cout << "Wallpapers stopped (0% GPU; wectl daemon idle at http://127.0.0.1:43197)\n";
    } else if (cmd == "down") {
        stop_all_services();
        std::cout << "wectl daemon and wallpaper engine stopped\n";
    } else if (cmd == "restart") {
        kill_renderer();
        std::system("systemctl --user restart wectl.service");
        std::cout << "wectl.service restarted\n";
    } else if (cmd == "restore") {
        const std::string cur = read_file(state_dir() / "current", "4K SCI-FI Black Hole");
        if (!cur.empty()) launch_wallpaper(cur);
    } else if (cmd == "uninstall") {
        stop_all_services();
        std::system("systemctl --user disable wectl.service >/dev/null 2>&1 || true");
        const char* home_env = std::getenv("HOME");
        const std::string home = home_env ? home_env : "/tmp";
        std::error_code ec;
        fs::remove(fs::path(home) / ".config" / "systemd" / "user" / "wectl.service", ec);
        fs::remove(fs::path(home) / ".local" / "bin" / "wectl", ec);
        std::system("systemctl --user daemon-reload >/dev/null 2>&1 || true");
        const fs::path rd = root_dir();
        std::cout << "wectl completely uninstalled from " << rd.string() << "\n";
        const std::string rm_cmd = "nohup sh -c 'sleep 0.2 && rm -rf " + sh_quote(rd.string()) + "' >/dev/null 2>&1 &";
        std::system(rm_cmd.c_str());
    } else {
        launch_wallpaper(cmd);
    }
    return 0;
}
