#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

static void print_banner() {
    std::cout <<
        "\033[1;36m"
        " ██╗    ██╗███████╗ ██████╗████████╗██╗     \n"
        " ██║    ██║██╔════╝██╔════╝╚══██╔══╝██║     \n"
        " ██║ █╗ ██║█████╗  ██║        ██║   ██║     \n"
        " ██║███╗██║██╔══╝  ██║        ██║   ██║     \n"
        " ╚███╔███╔╝███████╗╚██████╗   ██║   ███████╗\n"
        "  ╚══╝╚══╝ ╚══════╝ ╚═════╝   ╚═╝   ╚══════╝\n"
        "\033[0m"
        " \033[1;32m[v-0.1 Installer]\033[0m Portable Hyprland C++20 Build\n\n";
}

static std::string sh_quote(std::string_view s) {
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

static int run_cmd(const std::string& cmd) {
    return std::system(cmd.c_str());
}

static std::string hypr_ipc(std::string_view cmd) {
    const char* his = std::getenv("HYPRLAND_INSTANCE_SIGNATURE");
    const char* xdg = std::getenv("XDG_RUNTIME_DIR");
    if (!his || !*his || !xdg || !*xdg) return {};

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) return {};

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::snprintf(addr.sun_path, sizeof(addr.sun_path), "%s/hypr/%s/.socket.sock", xdg, his);
    if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        close(fd);
        return {};
    }
    write(fd, cmd.data(), cmd.size());
    std::string out;
    out.reserve(512);
    char buf[512];
    ssize_t n;
    while ((n = read(fd, buf, sizeof(buf))) > 0) out.append(buf, static_cast<size_t>(n));
    close(fd);
    return out;
}

static void cache_display_profile(const fs::path& state_dir) {
    std::string mon = "eDP-1";
    std::string res = "1920x1080";
    int max_hz = 60;

    const std::string j = hypr_ipc("j/monitors");
    auto get_str = [&](std::string_view k) -> std::string {
        auto p = j.find("\"" + std::string(k) + "\":");
        if (p == std::string::npos) return {};
        p = j.find('"', p + k.size() + 3);
        if (p == std::string::npos) return {};
        auto e = j.find('"', p + 1);
        return (e == std::string::npos) ? std::string{} : j.substr(p + 1, e - p - 1);
    };
    auto get_num = [&](std::string_view k) -> double {
        auto p = j.find("\"" + std::string(k) + "\":");
        if (p == std::string::npos) return 0.0;
        p += k.size() + 3;
        while (p < j.size() && (j[p] == ' ' || j[p] == ':')) ++p;
        return std::atof(j.c_str() + p);
    };

    const std::string m = get_str("name");
    const int w = static_cast<int>(get_num("width"));
    const int h = static_cast<int>(get_num("height"));
    const int hz = static_cast<int>(get_num("refreshRate") + 0.5);

    if (!m.empty()) mon = m;
    if (w > 0 && h > 0) res = std::to_string(w) + "x" + std::to_string(h);
    if (hz >= 15 && hz <= 1000) max_hz = hz;

    fs::create_directories(state_dir);
    std::ofstream out(state_dir / "hardware.env", std::ios::trunc);
    out << "MONITOR=" << mon << "\nRESOLUTION=" << res << "\nMAX_HZ=" << max_hz << "\n";
    std::cout << "[HW] Display cached: " << mon << " (" << res << " @ " << max_hz << "Hz)\n";
}

static bool is_cachy_v3() {
    std::ifstream cpu("/proc/cpuinfo"), osr("/etc/os-release");
    std::string c((std::istreambuf_iterator<char>(cpu)), std::istreambuf_iterator<char>());
    std::string o((std::istreambuf_iterator<char>(osr)), std::istreambuf_iterator<char>());
    return c.find("avx2") != std::string::npos && c.find("bmi2") != std::string::npos &&
           (o.find("cachyos") != std::string::npos || o.find("CachyOS") != std::string::npos);
}

static std::string pkg_base(std::string_view asset) {
    if (asset.rfind("03_dep-", 0) == 0) asset.remove_prefix(7);
    for (size_t i = 0; i + 1 < asset.size(); ++i) {
        if (asset[i] == '-' && std::isdigit(static_cast<unsigned char>(asset[i + 1]))) {
            return std::string(asset.substr(0, i));
        }
    }
    return std::string(asset);
}

int main(int argc, char** argv) {
    print_banner();
    const char* home_env = std::getenv("HOME");
    const std::string home = home_env ? home_env : "/tmp";
    const fs::path target_dir = (argc > 1) ? fs::path(*(argv + 1)) : (fs::path(home) / "wectl");

    fs::path self_path;
    {
        char exe_buf[4096] = {0};
        const ssize_t len = readlink("/proc/self/exe", exe_buf, sizeof(exe_buf) - 1);
        if (len > 0) self_path = fs::path(std::string_view(exe_buf, static_cast<size_t>(len)));
    }

    const fs::path tmp_dir = fs::temp_directory_path() / ("wectl_inst_" + std::to_string(getpid()));
    fs::create_directories(tmp_dir);

    const std::string gh_dl = std::string("https") + "://github." + "com/ltwtd/wectl/releases/download";
    const std::string manifest_url = gh_dl + "/assets-v.0.1/00_MANIFEST.txt";
    const std::string src_rel_url  = gh_dl + "/v-0.1/wectl-src.tar.zst";

    std::cout << "[1/5] Fetching asset manifest...\n";
    const fs::path manifest_file = tmp_dir / "manifest.txt";
    if (run_cmd("curl -fsSL " + sh_quote(manifest_url) + " -o " + sh_quote(manifest_file.string())) != 0) {
        std::cerr << "Error: Failed to fetch manifest.\n";
        fs::remove_all(tmp_dir);
        return 1;
    }

    std::string engine_url, scene_url;
    std::vector<std::pair<std::string, std::string>> dep_urls;
    {
        std::ifstream mf(manifest_file);
        std::string line;
        while (std::getline(mf, line)) {
            const auto bar = line.find('|');
            if (bar == std::string::npos) continue;
            std::string name = line.substr(0, bar);
            std::string url  = line.substr(bar + 1);
            while (!url.empty() && (url.back() == '\r' || url.back() == '\n')) url.pop_back();
            if (name == "01_lwe-engine-nocef-r643.tar.zst") engine_url = std::move(url);
            else if (name == "02_default-scene-blackhole-4k.tar.zst") scene_url = std::move(url);
            else if (name.rfind("03_dep-", 0) == 0) dep_urls.emplace_back(std::move(name), std::move(url));
        }
    }

    std::cout << "[2/5] Checking & installing missing dependencies...\n";
    if (is_cachy_v3()) {
        bool need_local = false;
        for (const auto& [name, url] : dep_urls) {
            const std::string pkg = pkg_base(name);
            if (run_cmd("pacman -Qi " + sh_quote(pkg) + " >/dev/null 2>&1") != 0) {
                std::cout << "  -> Downloading " << pkg << "...\n";
                const fs::path dst = tmp_dir / name;
                if (run_cmd("curl -fL --progress-bar --retry 3 " + sh_quote(url) + " -o " + sh_quote(dst.string())) == 0) {
                    need_local = true;
                }
            }
        }
        if (need_local) run_cmd("sudo pacman -U --needed --noconfirm " + sh_quote(tmp_dir.string()) + "/03_dep-*.pkg.tar.zst");
    } else {
        run_cmd("sudo pacman -S --needed --noconfirm ffmpeg mpv sdl2-compat lz4 glew glm wayland procps-ng xdg-utils gcc libjxl libplacebo sndio curl zstd psmisc");
    }
    dep_urls.clear();
    dep_urls.shrink_to_fit();

    std::cout << "[3/5] Building portable skeleton in " << target_dir.string() << "...\n";
    fs::create_directories(target_dir / "bin");
    fs::create_directories(target_dir / "state");
    fs::create_directories(target_dir / "wallpapers" / "scenes");
    fs::create_directories(target_dir / "wallpapers" / "scenes_hidden");
    fs::create_directories(target_dir / "wallpapers" / "vids");
    fs::create_directories(target_dir / "wallpapers" / "photos");

    if (!engine_url.empty()) {
        const fs::path arc = tmp_dir / "engine.tar.zst";
        run_cmd("curl -fL --progress-bar --retry 3 " + sh_quote(engine_url) + " -o " + sh_quote(arc.string()));
        run_cmd("tar -C " + sh_quote(target_dir.string()) + " --zstd -xf " + sh_quote(arc.string()));
        fs::remove(arc);
    }

    if (!scene_url.empty()) {
        const fs::path arc = tmp_dir / "scene.tar.zst";
        run_cmd("curl -fL --progress-bar --retry 3 " + sh_quote(scene_url) + " -o " + sh_quote(arc.string()));
        run_cmd("tar -C " + sh_quote((target_dir / "wallpapers" / "scenes").string()) + " --zstd -xf " + sh_quote(arc.string()));
        fs::remove(arc);
        std::ofstream(target_dir / "state" / "current") << "4K SCI-FI Black Hole\n";
        std::ofstream(target_dir / "state" / "fps") << "60\n";
        std::ofstream(target_dir / "state" / "scale") << "fill\n";
    }

    std::cout << "[4/5] Compiling 6 C++20 modules (-O3 -march=native -flto)...\n";
    {
        const fs::path src_arc = tmp_dir / "src.tar.zst";
        const fs::path bdir = tmp_dir / "build";
        fs::create_directories(bdir);
        run_cmd("curl -fL --progress-bar --retry 3 " + sh_quote(src_rel_url) + " -o " + sh_quote(src_arc.string()));
        run_cmd("tar -C " + sh_quote(bdir.string()) + " --zstd -xf " + sh_quote(src_arc.string()));

        const std::string flags = "-std=c++20 -fmodules-ts -O3 -march=native -flto -fno-rtti -pthread";
        const std::string out_bin = (target_dir / "wectl").string();
        const std::string cmd =
            "cd " + sh_quote(bdir.string()) + " && "
            "g++ " + flags + " -c src/modules/banner.cppm -o banner.o && "
            "g++ " + flags + " -c src/modules/core.cppm -o core.o && "g++ " + flags + " "
            "g++ " + flags + " -c src/modules/ipc.cppm -o ipc.o && "
            "g++ " + flags + " -c src/modules/render.cppm -o render.o && "
            "g++ " + flags + " -c src/modules/ui.cppm -o ui.o && "
            "g++ " + flags + " -c src/modules/server.cppm -o server.o && "g++ " + flags + " -c src/modules/menu.cppm -o menu.o && "
            "g++ " + flags + " -c src/main.cpp -o main.o && "
            "g++ " + flags + " banner.o core.o ipc.o render.o ui.o server.o menu.o main.o -o " + sh_quote(out_bin) + " && "
            "strip -s " + sh_quote(out_bin);

        if (run_cmd(cmd) != 0) {
            std::cerr << "Error: Compilation failed.\n";
            fs::remove_all(tmp_dir);
            return 1;
        }
    }

    run_cmd("chmod +x " + sh_quote((target_dir / "wectl").string()) + " " + sh_quote((target_dir / "bin" / "linux-wallpaperengine").string()) + " 2>/dev/null || true");
    cache_display_profile(target_dir / "state");

    const fs::path local_bin = fs::path(home) / ".local" / "bin";
    fs::create_directories(local_bin);
    fs::remove(local_bin / "wectl");
    fs::create_symlink(target_dir / "wectl", local_bin / "wectl");

    std::cout << "[5/5] Enabling wectl.service & wiping temporary files...\n";
    const fs::path unit_dir = fs::path(home) / ".config" / "systemd" / "user";
    fs::create_directories(unit_dir);
    {
        std::ofstream unit(unit_dir / "wectl.service", std::ios::trunc);
        unit << "[Unit]\nDescription=Pure C++20 Wallpaper Engine Gallery Daemon (Hyprland)\nAfter=graphical-session.target\n\n"
             << "[Service]\nPassEnvironment=WAYLAND_DISPLAY XDG_RUNTIME_DIR HYPRLAND_INSTANCE_SIGNATURE\n"
             << "Environment=\"PATH=" << (target_dir / "bin").string() << ":" << local_bin.string() << ":/usr/local/bin:/usr/bin:/bin\"\n"
             << "ExecStart=" << (target_dir / "wectl").string() << " daemon\n"
             << "Restart=on-failure\nRestartSec=1\n\n[Install]\nWantedBy=default.target\n";
    }

    run_cmd("systemctl --user import-environment WAYLAND_DISPLAY XDG_RUNTIME_DIR HYPRLAND_INSTANCE_SIGNATURE PATH 2>/dev/null || true");
    run_cmd("systemctl --user daemon-reload");
    run_cmd("systemctl --user enable --now wectl.service");
    run_cmd("systemctl --user restart wectl.service");

    fs::remove_all(tmp_dir);
    if (!self_path.empty() && fs::exists(self_path)) {
        std::error_code ec;
        fs::remove(self_path, ec);
    }

    std::cout << "\nDone! Portable installation ready in: " << target_dir.string() << "\n"
              << "Commands: wectl up | wectl status | wectl stop | wectl uninstall\n";
    return 0;
}
