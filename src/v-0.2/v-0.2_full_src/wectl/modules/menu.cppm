module;
#include <arpa/inet.h>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <unistd.h>
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

inline void clear_screen() {
    write(STDOUT_FILENO, "\033[2J\033[H", 7);
}

inline std::string read_line() {
    std::string s;
    std::getline(std::cin, s);
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ')) s.pop_back();
    return s;
}

inline void pause_prompt() {
    std::cout << "\n\033[90mPress Enter to continue...\033[0m" << std::flush;
    std::string dummy;
    std::getline(std::cin, dummy);
}

inline bool is_daemon_alive() {
    const int fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (fd < 0) return false;
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(43197);
    const bool ok = (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == 0);
    close(fd);
    return ok;
}

inline void print_status_block() {
    const auto& hw = load_hw();
    std::cout << "Root:    " << root_dir().string() << "\n"
              << "Display: " << hw.monitor << " | " << hw.resolution << " | Max " << hw.max_hz << "Hz | fill\n"
              << "Service: " << (is_daemon_alive() ? "\033[1;32mACTIVE\033[0m (http://127.0.0.1:43197)\n" : "\033[1;31mSTOPPED\033[0m\n")
              << "Media:   " << read_file(state_dir() / "current", "none") << "\n"
              << "FPS:     " << clamp_fps(std::atoi(read_file(state_dir() / "fps", "60").c_str())) << " FPS\n";

    const pid_t pid = find_renderer_pid();
    if (pid > 0) {
        std::array<char, 64> stat_path{};
        std::snprintf(stat_path.data(), stat_path.size(), "/proc/%d/stat", pid);
        std::ifstream sf(stat_path.data());
        std::string line;
        char state_ch = '?';
        long rss_pages = 0;
        if (std::getline(sf, line)) {
            const size_t rparen = line.rfind(')');
            if (rparen != std::string::npos && rparen + 2 < line.size()) {
                state_ch = line[rparen + 2];
                std::sscanf(line.c_str() + rparen + 4,
                            "%*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*d %*d %*d %*d %*d %*d %*u %*u %ld",
                            &rss_pages);
            }
        }
        const long rss_kb = rss_pages * (sysconf(_SC_PAGESIZE) / 1024);
        std::cout << "Render:  \033[1;32mPID " << pid << "\033[0m | State: " << state_ch
                  << (state_ch == 'T' ? " (FROZEN 0% GPU)" : " (ACTIVE)")
                  << " | RSS: " << rss_kb << " KB\n";
    } else {
        std::cout << "Render:  STOPPED (0% GPU)\n";
    }
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

    std::ifstream deps_in(state_dir() / "installed_deps.list");
    std::string pkg, safe_pkgs;
    while (std::getline(deps_in, pkg)) {
        if (pkg == "mpvpaper" || pkg == "swaybg") {
            safe_pkgs += " " + sh_quote(pkg);
        }
    }
    if (!safe_pkgs.empty()) {
        std::system(("sudo pacman -Rns --noconfirm" + safe_pkgs + " >/dev/null 2>&1 || true").c_str());
    }

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
                  << "\033[1;34m│\033[0m  1) Up (Start & Open Gallery)   4) Down (Stop All)     \033[1;34m│\033[0m\n"
                  << "\033[1;34m│\033[0m  2) Status & GPU Telemetry      5) Restart Service     \033[1;34m│\033[0m\n"
                  << "\033[1;34m│\033[0m  3) Freeze / Resume (0% GPU)    6) Uninstall wectl     \033[1;34m│\033[0m\n"
                  << "\033[1;34m│\033[0m  0) Back                                               \033[1;34m│\033[0m\n"
                  << "\033[1;34m└────────────────────────────────────────────────────────┘\033[0m\n"
                  << "\nSelect action > " << std::flush;

        const std::string ch = read_line();
        if (ch == "0" || ch.empty()) return;
        std::cout << "\n";
        if (ch == "1") {
            std::system("systemctl --user reset-failed wectl.service >/dev/null 2>&1 || true");
            std::system("systemctl --user start wectl.service");
            std::system("xdg-open http://127.0.0.1:43197 >/dev/null 2>&1 &");
            std::cout << "Gallery opened: http://127.0.0.1:43197\n";
        } else if (ch == "2") {
            print_status_block();
        } else if (ch == "3") {
            const bool frozen = toggle_freeze_frame();
            std::cout << (frozen ? "Frozen on current frame (0% GPU).\n"
                                 : "Resumed wallpaper animation.\n");
        } else if (ch == "4") {
            stop_all_services();
            std::cout << "Daemon and renderer stopped.\n";
        } else if (ch == "5") {
            kill_renderer();
            std::system("systemctl --user reset-failed wectl.service >/dev/null 2>&1 || true");
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
        print_logo("Module: Config & Media");
        const auto& hw = load_hw();
        const std::string cur_scn = read_file(state_dir() / "current", "none");
        const int cur_fps = clamp_fps(std::atoi(read_file(state_dir() / "fps", "60").c_str()));

        std::cout << "\033[1;35m┌────────────────────────────────────────────────────────┐\033[0m\n"
                  << "\033[1;35m│\033[0m  Active: " << cur_scn << " (" << cur_fps << " FPS / max " << hw.max_hz << "Hz)\n"
                  << "\033[1;35m├────────────────────────────────────────────────────────┤\033[0m\n"
                  << "\033[1;35m│\033[0m  1) Select Media (Scene/Vid/Pic)  3) Restore Last      \033[1;35m│\033[0m\n"
                  << "\033[1;35m│\033[0m  2) Set Target FPS                0) Back              \033[1;35m│\033[0m\n"
                  << "\033[1;35m└────────────────────────────────────────────────────────┘\033[0m\n"
                  << "\nSelect action > " << std::flush;

        const std::string ch = read_line();
        if (ch == "0" || ch.empty()) return;
        std::cout << "\n";
        if (ch == "1") {
            std::vector<std::string> ids;
            std::error_code ec;
            if (fs::exists(scenes_dir(), ec)) {
                for (const auto& e : fs::directory_iterator(scenes_dir(), ec))
                    if (e.is_directory(ec)) ids.push_back("scene:" + e.path().filename().string());
            }
            if (fs::exists(vids_dir(), ec)) {
                for (const auto& e : fs::directory_iterator(vids_dir(), ec))
                    if (e.is_regular_file(ec)) ids.push_back("vid:" + e.path().filename().string());
            }
            if (fs::exists(photos_dir(), ec)) {
                for (const auto& e : fs::directory_iterator(photos_dir(), ec))
                    if (e.is_regular_file(ec)) ids.push_back("photo:" + e.path().filename().string());
            }
            for (size_t i = 0; i < ids.size(); ++i) {
                std::cout << "  " << (i + 1) << ") " << ids.at(i) << "\n";
            }
            std::cout << "\nMedia number > " << std::flush;
            const size_t idx = static_cast<size_t>(std::atoi(read_line().c_str()));
            if (idx >= 1 && idx <= ids.size()) {
                launch_wallpaper(ids.at(idx - 1));
                std::cout << "Started: " << ids.at(idx - 1) << "\n";
            }
        } else if (ch == "2") {
            std::cout << "Enter FPS (15.." << hw.max_hz << ") > " << std::flush;
            const int f = clamp_fps(std::atoi(read_line().c_str()));
            write_file(state_dir() / "fps", std::to_string(f));
            if (!cur_scn.empty() && cur_scn != "none") launch_wallpaper(cur_scn, f);
            std::cout << "Target FPS set to: " << f << "\n";
        } else if (ch == "3") {
            const std::string last = read_file(state_dir() / "current", "scene:4K SCI-FI Black Hole");
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
        << "\033[1;36m│\033[0m  1) App Control     (Up, Status, Freeze, Down, Remove) \033[1;36m│\033[0m\n"
        << "\033[1;36m│\033[0m  2) Config & Media  (Scenes, Videos, Photos, FPS)      \033[1;36m│\033[0m\n"
        << "\033[1;36m│\033[0m  0) Exit                                               \033[1;36m│\033[0m\n"
        << "\033[1;36m└────────────────────────────────────────────────────────┘\033[0m\n"
        << "\nSelect module > " << std::flush;
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
