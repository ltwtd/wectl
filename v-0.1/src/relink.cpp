#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

namespace fs = std::filesystem;

int main() {
    char buf[4096] = {0};
    const ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    const fs::path root = (len > 0) ? fs::path(std::string(buf, static_cast<size_t>(len))).parent_path() : fs::current_path();
    const fs::path bin = root / "wectl";

    if (!fs::exists(bin)) {
        std::cerr << "Error: wectl binary not found in " << root.string() << "\n";
        return 1;
    }

    const char* home_env = std::getenv("HOME");
    const std::string home = home_env ? home_env : "/tmp";
    const fs::path local_bin = fs::path(home) / ".local" / "bin";
    const fs::path unit_dir = fs::path(home) / ".config" / "systemd" / "user";

    fs::create_directories(local_bin);
    fs::remove(local_bin / "wectl");
    fs::create_symlink(bin, local_bin / "wectl");

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

    std::cout << "Relinked to: " << root.string() << "\n";
    return 0;
}
