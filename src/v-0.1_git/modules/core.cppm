module;
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <unistd.h>

export module wectl.core;

export namespace wectl::core {
namespace fs = std::filesystem;

struct HardwareProfile {
    std::string monitor = "eDP-1";
    std::string resolution = "1920x1080";
    int max_hz = 60;
};

inline fs::path root_dir() {
    static const fs::path cached = []() {
        char buf[4096] = {0};
        const ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
        if (len > 0) return fs::path(std::string_view(buf, static_cast<size_t>(len))).parent_path();
        return fs::current_path();
    }();
    return cached;
}

inline fs::path state_dir()  { return root_dir() / "state"; }
inline fs::path scenes_dir() { return root_dir() / "wallpapers" / "scenes"; }
inline fs::path hidden_dir() { return root_dir() / "wallpapers" / "scenes_hidden"; }

inline void relocate_self(bool verbose = true) {
    const fs::path root = root_dir();
    const fs::path bin = root / "wectl";
    const char* home_env = std::getenv("HOME");
    const std::string home = home_env ? home_env : "/tmp";
    const fs::path local_bin = fs::path(home) / ".local" / "bin";
    const fs::path unit_dir = fs::path(home) / ".config" / "systemd" / "user";

    fs::create_directories(local_bin);
    std::error_code ec;
    fs::remove(local_bin / "wectl", ec);
    fs::create_symlink(bin, local_bin / "wectl", ec);

    fs::create_directories(unit_dir);
    {
        std::ofstream unit(unit_dir / "wectl.service", std::ios::trunc);
        unit << "[Unit]\nDescription=Pure C++20 Wallpaper Engine Gallery Daemon (Hyprland)\nAfter=graphical-session.target\n\n"
             << "[Service]\nPassEnvironment=WAYLAND_DISPLAY XDG_RUNTIME_DIR HYPRLAND_INSTANCE_SIGNATURE\n"
             << "Environment=\"PATH=" << (root / "bin").string() << ":" << local_bin.string() << ":/usr/local/bin:/usr/bin:/bin\"\n"
             << "ExecStart=" << bin.string() << " daemon\n"
             << "Restart=on-failure\nRestartSec=1\n\n[Install]\nWantedBy=default.target\n";
    }

    std::system("systemctl --user import-environment WAYLAND_DISPLAY XDG_RUNTIME_DIR HYPRLAND_INSTANCE_SIGNATURE PATH 2>/dev/null || true");
    std::system("systemctl --user daemon-reload");
    std::system("systemctl --user enable --now wectl.service");
    std::system("systemctl --user restart wectl.service");

    if (verbose) std::cout << "Relinked & restarted service at: " << root.string() << "\n";
}

inline void ensure_linked() {
    const char* home_env = std::getenv("HOME");
    if (!home_env || !*home_env) return;
    const fs::path link_path = fs::path(home_env) / ".local" / "bin" / "wectl";
    std::error_code ec;
    if (!fs::exists(link_path, ec) || fs::weakly_canonical(link_path, ec) != fs::weakly_canonical(root_dir() / "wectl", ec)) {
        relocate_self(false);
    }
}

inline const HardwareProfile& load_hw() {
    static const HardwareProfile hw = []() {
        HardwareProfile p;
        std::ifstream in(state_dir() / "hardware.env");
        if (!in) return p;
        std::string line;
        while (std::getline(in, line)) {
            const auto eq = line.find('=');
            if (eq == std::string::npos) continue;
            std::string_view k(line.data(), eq);
            std::string v = line.substr(eq + 1);
            while (!v.empty() && (v.back() == '\r' || v.back() == '\n' || v.back() == ' ')) v.pop_back();
            if (k == "MONITOR" && !v.empty()) p.monitor = std::move(v);
            else if (k == "RESOLUTION" && !v.empty()) p.resolution = std::move(v);
            else if (k == "MAX_HZ" && !v.empty()) {
                const int hz = std::atoi(v.c_str());
                if (hz >= 15 && hz <= 1000) p.max_hz = hz;
            }
        }
        return p;
    }();
    return hw;
}

inline std::string read_file(const fs::path& p, std::string_view def = "") {
    std::ifstream f(p);
    if (!f) return std::string(def);
    std::string s((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    while (!s.empty() && (s.back() == '\n' || s.back() == '\r' || s.back() == ' ')) s.pop_back();
    if (s.empty()) return std::string(def);
    s.shrink_to_fit();
    return s;
}

inline void write_file(const fs::path& p, std::string_view val) {
    fs::create_directories(p.parent_path());
    std::ofstream f(p, std::ios::trunc);
    f << val << '\n';
}

inline int clamp_fps(int fps) {
    const int max_hz = load_hw().max_hz;
    if (fps < 15) return 15;
    if (fps > max_hz) return max_hz;
    return fps;
}

}
