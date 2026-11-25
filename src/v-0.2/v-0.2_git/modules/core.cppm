module;
#include <array>
#include <cstdlib>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
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
        std::array<char, 4096> buf{};
        const ssize_t len = readlink("/proc/self/exe", buf.data(), buf.size() - 1);
        if (len > 0) return fs::path(std::string_view(buf.data(), static_cast<size_t>(len))).parent_path();
        return fs::current_path();
    }();
    return cached;
}

inline fs::path state_dir()         { return root_dir() / "state"; }
inline fs::path scenes_dir()        { return root_dir() / "wallpapers" / "scenes"; }
inline fs::path scenes_hidden_dir() { return root_dir() / "wallpapers" / "scenes_hidden"; }
inline fs::path vids_dir()          { return root_dir() / "wallpapers" / "vids"; }
inline fs::path vids_hidden_dir()   { return root_dir() / "wallpapers" / "vids_hidden"; }
inline fs::path photos_dir()        { return root_dir() / "wallpapers" / "photos"; }
inline fs::path photos_hidden_dir() { return root_dir() / "wallpapers" / "photos_hidden"; }

inline void relocate_self(bool verbose = true) {
    const fs::path root = root_dir();
    const fs::path bin = root / "wectl";
    const char* home_env = std::getenv("HOME");
    const std::string home = home_env ? home_env : "/tmp";
    const fs::path local_bin = fs::path(home) / ".local" / "bin";
    const fs::path unit_dir = fs::path(home) / ".config" / "systemd" / "user";

    std::error_code ec;
    fs::create_directories(local_bin, ec);
    fs::remove(local_bin / "wectl", ec);
    fs::create_symlink(bin, local_bin / "wectl", ec);

    fs::create_directories(unit_dir, ec);
    {
        std::ofstream unit(unit_dir / "wectl.service", std::ios::trunc);
        unit << "[Unit]\nDescription=Pure C++20 Wallpaper Engine Gallery Daemon (Hyprland)\n"
             << "After=default.target\nStartLimitIntervalSec=0\n\n"
             << "[Service]\nType=simple\nPassEnvironment=WAYLAND_DISPLAY XDG_RUNTIME_DIR HYPRLAND_INSTANCE_SIGNATURE XDG_CURRENT_DESKTOP\n"
             << "WorkingDirectory=" << root.string() << "\n"
             << "Environment=\"PATH=" << (root / "bin").string() << ":" << local_bin.string() << ":/usr/local/bin:/usr/bin:/bin\"\n"
             << "Environment=\"LD_LIBRARY_PATH=" << (root / "lib").string() << "\"\n"
             << "Environment=\"XDG_SESSION_TYPE=wayland\"\n"
             << "ExecStartPre=/bin/sh -c 'until systemctl --user show-environment | grep -q \"HYPRLAND_INSTANCE_SIGNATURE=\"; do sleep 0.2; done; sleep 0.5'\n"
             << "ExecStart=" << bin.string() << " daemon\n"
             << "Restart=on-failure\nRestartSec=1\n\n[Install]\nWantedBy=default.target\n";
    }

    std::system("systemctl --user import-environment WAYLAND_DISPLAY XDG_RUNTIME_DIR HYPRLAND_INSTANCE_SIGNATURE PATH >/dev/null 2>&1 || true");
    std::system("systemctl --user daemon-reload >/dev/null 2>&1");
    std::system("systemctl --user reset-failed wectl.service >/dev/null 2>&1 || true");
    std::system("systemctl --user enable --now wectl.service >/dev/null 2>&1");
    std::system("systemctl --user restart wectl.service >/dev/null 2>&1");

    if (verbose) {
        const std::string msg = "Relinked & restarted service at: " + root.string() + "\n";
        write(STDOUT_FILENO, msg.data(), msg.size());
    }
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
    const int fd = open(p.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0) return std::string(def);
    std::array<char, 512> buf{};
    const ssize_t n = read(fd, buf.data(), buf.size());
    close(fd);
    if (n <= 0) return std::string(def);
    std::string_view sv(buf.data(), static_cast<size_t>(n));
    while (!sv.empty() && (sv.back() == '\n' || sv.back() == '\r' || sv.back() == ' ')) sv.remove_suffix(1);
    return sv.empty() ? std::string(def) : std::string(sv);
}

inline void write_file(const fs::path& p, std::string_view val) {
    std::error_code ec;
    fs::create_directories(p.parent_path(), ec);
    const int fd = open(p.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644);
    if (fd >= 0) {
        write(fd, val.data(), val.size());
        write(fd, "\n", 1);
        close(fd);
    }
}

inline int clamp_fps(int fps) {
    const int max_hz = load_hw().max_hz;
    if (fps < 15) return 15;
    if (fps > max_hz) return max_hz;
    return fps;
}

}
