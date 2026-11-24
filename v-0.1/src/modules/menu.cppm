module;
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

export module wectl.menu;
import wectl.banner;
import wectl.core;
import wectl.render;

export namespace wectl::menu {
namespace fs = std::filesystem;
using namespace wectl::banner;
using namespace wectl::core;
using namespace wectl::render;

inline void clear_screen() {
    std::cout << "\033[2J\033[H" << std::flush;
}

inline std::string read_line() {
    std::string s;
    std::getline(std::cin, s);
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ')) s.pop_back();
    return s;
}

inline void pause_prompt() {
    std::cout << "\n\033[90mPress Enter to continue...\033[0m";
    std::string dummy;
    std::getline(std::cin, dummy);
}

inline void print_status_block() {
    const auto& hw = load_hw();
    std::cout << "Root:    " << root_dir().string() << "\n"
              << "Display: " << hw.monitor << " | " << hw.resolution << " | Max " << hw.max_hz << "Hz | fill\n";
    std::system("systemctl --user is-active --quiet wectl.service && echo 'Service: \033[1;32mACTIVE\033[0m (http://127.0.0.1:43197)' || echo 'Service: \033[1;31mSTOPPED\033[0m'");
    std::cout << "Scene:   " << read_file(state_dir() / "current", "none") << "\n"
              << "FPS:     " << clamp_fps(std::atoi(read_file(state_dir() / "fps", "60").c_str())) << " FPS\n";
    std::system("pgrep -f wallpaperengine >/dev/null && ps -o pid,stat,rss,comm -C linux-wallpaperengine || echo 'Render:  STOPPED (0% GPU)'");
}

inline void run_uninstall() {
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
}

inline void menu_app_control() {
    while (true) {
        clear_screen();
        print_logo("Module: App Control");
        std::cout << "\033[1;34m┌────────────────────────────────────────────────────────┐\033[0m\n"
                  << "\033[1;34m│\033[0m  [1] Up (Start & Open Gallery)   [4] Down (Stop All)   \033[1;34m│\033[0m\n"
                  << "\033[1;34m│\033[0m  [2] Status & GPU Telemetry      [5] Restart Service   \033[1;34m│\033[0m\n"
                  << "\033[1;34m│\033[0m  [3] Stop Wallpaper (0% GPU)     [6] Uninstall wectl   \033[1;34m│\033[0m\n"
                  << "\033[1;34m│\033[0m  [0] Back                                              \033[1;34m│\033[0m\n"
                  << "\033[1;34m└────────────────────────────────────────────────────────┘\033[0m\n"
                  << "\nSelect action > ";

        const std::string ch = read_line();
        if (ch == "0" || ch.empty()) return;
        std::cout << "\n";
        if (ch == "1") {
            std::system("systemctl --user start wectl.service");
            std::system("xdg-open http://127.0.0.1:43197 >/dev/null 2>&1 &");
            std::cout << "Gallery opened: http://127.0.0.1:43197\n";
        } else if (ch == "2") {
            print_status_block();
        } else if (ch == "3") {
            stop_wallpaper_session();
            std::cout << "Wallpapers stopped (0% GPU).\n";
        } else if (ch == "4") {
            stop_all_services();
            std::cout << "Daemon and renderer stopped.\n";
        } else if (ch == "5") {
            kill_renderer();
            std::system("systemctl --user restart wectl.service");
            std::cout << "wectl.service restarted.\n";
        } else if (ch == "6") {
            run_uninstall();
            std::exit(0);
        }
        pause_prompt();
    }
}

inline void menu_config_control() {
    while (true) {
        clear_screen();
        print_logo("Module: Config & Scenes");
        const auto& hw = load_hw();
        const std::string cur_scn = read_file(state_dir() / "current", "none");
        const int cur_fps = clamp_fps(std::atoi(read_file(state_dir() / "fps", "60").c_str()));

        std::cout << "\033[1;35m┌────────────────────────────────────────────────────────┐\033[0m\n"
                  << "\033[1;35m│\033[0m  Active: " << cur_scn << " (" << cur_fps << " FPS / max " << hw.max_hz << "Hz)\n"
                  << "\033[1;35m├────────────────────────────────────────────────────────┤\033[0m\n"
                  << "\033[1;35m│\033[0m  [1] Select Scene                [3] Restore Last      \033[1;35m│\033[0m\n"
                  << "\033[1;35m│\033[0m  [2] Set Target FPS              [0] Back              \033[1;35m│\033[0m\n"
                  << "\033[1;35m└────────────────────────────────────────────────────────┘\033[0m\n"
                  << "\nSelect action > ";

        const std::string ch = read_line();
        if (ch == "0" || ch.empty()) return;
        std::cout << "\n";
        if (ch == "1") {
            std::vector<std::string> names;
            if (fs::exists(scenes_dir())) {
                for (const auto& e : fs::directory_iterator(scenes_dir())) {
                    if (e.is_directory()) names.push_back(e.path().filename().string());
                }
            }
            for (size_t i = 0; i < names.size(); ++i) {
                std::cout << "  [" << (i + 1) << "] " << names[i] << "\n";
            }
            std::cout << "\nScene number > ";
            const size_t idx = static_cast<size_t>(std::atoi(read_line().c_str()));
            if (idx >= 1 && idx <= names.size()) {
                launch_wallpaper(names[idx - 1]);
                std::cout << "Started: " << names[idx - 1] << "\n";
            }
        } else if (ch == "2") {
            std::cout << "Enter FPS (15.." << hw.max_hz << ") > ";
            const int f = clamp_fps(std::atoi(read_line().c_str()));
            write_file(state_dir() / "fps", std::to_string(f));
            if (!cur_scn.empty() && cur_scn != "none") launch_wallpaper(cur_scn, f);
            std::cout << "Target FPS set to: " << f << "\n";
        } else if (ch == "3") {
            const std::string last = read_file(state_dir() / "current", "4K SCI-FI Black Hole");
            if (!last.empty()) {
                launch_wallpaper(last);
                std::cout << "Restored: " << last << "\n";
            }
        }
        pause_prompt();
    }
}

inline void run_interactive_hub() {
    while (true) {
        clear_screen();
        print_logo();
        std::cout << "\033[1;36m┌────────────────────────────────────────────────────────┐\033[0m\n"
                  << "\033[1;36m│\033[0m  [1] App Control     (Up, Status, Stop, Down, Remove)  \033[1;36m│\033[0m\n"
                  << "\033[1;36m│\033[0m  [2] Config & Scenes (Select Scene, FPS, Restore)      \033[1;36m│\033[0m\n"
                  << "\033[1;36m│\033[0m  [0] Exit                                              \033[1;36m│\033[0m\n"
                  << "\033[1;36m└────────────────────────────────────────────────────────┘\033[0m\n"
                  << "\nSelect module > ";

        const std::string ch = read_line();
        if (ch == "1") menu_app_control();
        else if (ch == "2") menu_config_control();
        else if (ch == "0" || ch.empty()) {
            clear_screen();
            return;
        }
    }
}

}
