#include <cstdlib>
#include <iostream>
#include <string>

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
    const std::string cmd = (argc > 1) ? *(argv + 1) : "-h";

    if (cmd == "daemon") {
        run_daemon();
        return 0;
    }

    if (cmd == "-h" || cmd == "--help" || cmd == "help") {
        run_interactive_hub();
        return 0;
    }

    clear_screen();
    print_logo();

    if (cmd == "status") {
        print_status_block();
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
        run_uninstall();
    } else {
        launch_wallpaper(cmd);
    }
    return 0;
}
