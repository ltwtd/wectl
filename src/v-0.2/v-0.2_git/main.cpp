#include <cstdlib>
#include <iostream>
#include <string>
#include <unistd.h>

import wectl.banner;
import wectl.core;
import wectl.menu;
import wectl.render;
import wectl.server;

using namespace wectl::banner;
using namespace wectl::core;
using namespace wectl::menu;
using namespace wectl::render;
using namespace wectl::server;

int main(int argc, char** argv) {
    if (!std::getenv("XDG_RUNTIME_DIR")) {
        const std::string rt = "/run/user/" + std::to_string(getuid());
        setenv("XDG_RUNTIME_DIR", rt.c_str(), 1);
    }
    if (!std::getenv("DBUS_SESSION_BUS_ADDRESS")) {
        const std::string bus = "unix:path=/run/user/" + std::to_string(getuid()) + "/bus";
        setenv("DBUS_SESSION_BUS_ADDRESS", bus.c_str(), 1);
    }

    const std::string cmd = (argc > 1) ? *(argv + 1) : "-h";

    if (cmd == "daemon") {
        run_daemon();
        return 0;
    }

    ensure_linked();

    if (cmd == "-h" || cmd == "--help" || cmd == "help") {
        run_interactive_hub();
        return 0;
    }

    clear_screen();
    print_logo();

    if (cmd == "status") {
        print_status_block();
    } else if (cmd == "up") {
        std::system("systemctl --user reset-failed wectl.service >/dev/null 2>&1 || true");
        std::system("systemctl --user start wectl.service");
        std::system("xdg-open http://127.0.0.1:43197 >/dev/null 2>&1 &");
        std::cout << "Gallery: http://127.0.0.1:43197\n";
    } else if (cmd == "stop" || cmd == "freeze" || cmd == "resume") {
        const bool frozen = toggle_freeze_frame();
        std::cout << (frozen ? "Wallpapers frozen on current frame (0% GPU)\n"
                             : "Wallpapers resumed\n");
    } else if (cmd == "down") {
        stop_all_services();
        std::cout << "wectl daemon and wallpaper engine stopped\n";
    } else if (cmd == "restart") {
        kill_renderer();
        std::system("systemctl --user reset-failed wectl.service >/dev/null 2>&1 || true");
        std::system("systemctl --user restart wectl.service");
        std::cout << "wectl.service restarted\n";
    } else if (cmd == "restore") {
        const std::string cur = read_file(state_dir() / "current", "scene:4K SCI-FI Black Hole");
        if (!cur.empty()) launch_wallpaper(cur);
    } else if (cmd == "parallax") {
        if (argc < 3) {
            const bool on = (read_file(state_dir() / "parallax", "1") != "0");
            std::cout << "Parallax: " << (on ? "on" : "off") << "\n";
            return 0;
        }
        const std::string arg = *(argv + 2);
        bool next_val = true;
        if (arg == "off" || arg == "0" || arg == "false" || arg == "disable") {
            next_val = false;
        } else if (arg == "on" || arg == "1" || arg == "true" || arg == "enable") {
            next_val = true;
        } else if (arg == "toggle") {
            next_val = (read_file(state_dir() / "parallax", "1") == "0");
        } else {
            std::cerr << "Usage: wectl parallax [on|off|toggle]\n";
            return 1;
        }
        write_file(state_dir() / "parallax", next_val ? "1" : "0");
        reload_current_wallpaper(true);
        std::cout << "Parallax: " << (next_val ? "on" : "off") << "\n";
    } else if (cmd == "scale") {
        if (argc < 3) {
            std::cout << "Scale: " << normalize_scale(read_file(state_dir() / "scale", "fill")) << "\n";
            return 0;
        }
        const std::string arg = *(argv + 2);
        if (arg != "fill" && arg != "fit" && arg != "stretch" && arg != "center") {
            std::cerr << "Usage: wectl scale [fill|fit|stretch|center]\n";
            return 1;
        }
        write_file(state_dir() / "scale", arg);
        reload_current_wallpaper(false);
        std::cout << "Scale: " << arg << "\n";
    } else if (cmd == "relink") {
        relocate_self(true);
    } else if (cmd == "uninstall") {
        run_uninstall();
    } else {
        launch_wallpaper(cmd);
    }

    return 0;
}

