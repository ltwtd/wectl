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
        " \033[1;32m[v-0.2 Installer]\033[0m Ultra-Optimized NOCEF Engine & C++20 Gallery\n\n";
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

static std::string run_capture(const std::string& cmd) {
    std::string out;
    FILE* fp = popen(cmd.c_str(), "r");
    if (!fp) return out;
    char buf[512];
    while (fgets(buf, sizeof(buf), fp)) out += buf;
    pclose(fp);
    while (!out.empty() && (out.back() == '\n' || out.back() == '\r' || out.back() == ' ')) out.pop_back();
    return out;
}

static std::string hypr_ipc(std::string_view cmd) {
    const char* his = std::getenv("HYPRLAND_INSTANCE_SIGNATURE");
    const char* xdg = std::getenv("XDG_RUNTIME_DIR");
    if (!his || !*his || !xdg || !*xdg) return {};

    int fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
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

static bool is_arch_x86_64() {
    std::ifstream osr("/etc/os-release");
    std::string o((std::istreambuf_iterator<char>(osr)), std::istreambuf_iterator<char>());
    bool arch_like = (o.find("arch") != std::string::npos || o.find("cachyos") != std::string::npos || o.find("CachyOS") != std::string::npos);
    return arch_like && fs::exists("/usr/bin/pacman");
}

struct DepSpec {
    std::string filename;
    std::string url;
    std::string pkg_name;
    std::string pkg_ver;
};

static DepSpec parse_dep_asset(std::string_view name, std::string_view url) {
    DepSpec d{std::string(name), std::string(url), {}, {}};
    std::string_view s = name;
    if (s.rfind("03_dep-", 0) == 0) s.remove_prefix(7);
    if (s.size() > 12 && s.substr(s.size() - 12) == ".pkg.tar.zst") s.remove_suffix(12);
    for (const std::string_view arch : {"-x86_64_v3", "-x86_64", "-any"}) {
        if (s.size() > arch.size() && s.substr(s.size() - arch.size()) == arch) {
            s.remove_suffix(arch.size());
            break;
        }
    }
    for (size_t i = 0; i + 1 < s.size(); ++i) {
        if (s[i] == '-' && std::isdigit(static_cast<unsigned char>(s[i + 1]))) {
            d.pkg_name = std::string(s.substr(0, i));
            std::string v(s.substr(i + 1));
            for (char& c : v) {
                if (c == '_') c = ':';
            }
            d.pkg_ver = std::move(v);
            return d;
        }
    }
    d.pkg_name = std::string(s);
    return d;
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
    const std::string manifest_url = gh_dl + "/assets/00_MANIFEST.txt";
    const std::string src_rel_url  = gh_dl + "/v-0.2/wectl-src.tar.zst";
    const std::string bin_rel_url  = gh_dl + "/v-0.2/wectl-bin-x86_64.tar.zst";

    std::cout << "[1/5] Fetching shared asset manifest...\n";
    const fs::path manifest_file = tmp_dir / "manifest.txt";
    if (run_cmd("curl -fsSL " + sh_quote(manifest_url) + " -o " + sh_quote(manifest_file.string())) != 0) {
        std::cerr << "Error: Failed to fetch manifest from assets release.\n";
        fs::remove_all(tmp_dir);
        return 1;
    }

    std::string engine_url, scene_url;
    std::vector<DepSpec> deps;
    {
        std::ifstream mf(manifest_file);
        std::string line;
        while (std::getline(mf, line)) {
            while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) line.pop_back();
            if (line.empty()) continue;
            std::string name, url;
            const auto bar = line.find('|');
            if (bar != std::string::npos) {
                name = line.substr(0, bar);
                url  = line.substr(bar + 1);
            } else {
                const auto sp = line.find_last_of(" \t");
                if (sp == std::string::npos) continue;
                name = line.substr(sp + 1);
                url  = gh_dl + "/assets/" + name;
            }
            if (name == "01_lwe-engine-native-full.tar.zst") engine_url = std::move(url);
            else if (name == "02_default-scene-blackhole-4k.tar.zst") scene_url = std::move(url);
            else if (name.rfind("03_dep-", 0) == 0) deps.push_back(parse_dep_asset(name, url));
        }
    }

    std::cout << "[2/5] Installing exact verified dependencies from shared assets...\n";
    if (is_arch_x86_64() && !deps.empty()) {
        bool need_local = false;
        for (const auto& d : deps) {
            const std::string inst = run_capture("pacman -Q " + sh_quote(d.pkg_name) + " 2>/dev/null");
            const std::string expected = d.pkg_ver.empty() ? d.pkg_name : (d.pkg_name + " " + d.pkg_ver);
            if (inst != expected) {
                std::cout << "  -> Fetching verified " << expected << (inst.empty() ? " (missing)" : (" (installed: " + inst + ")")) << "...\n";
                const fs::path dst = tmp_dir / d.filename;
                if (run_cmd("curl -fL --progress-bar --retry 3 " + sh_quote(d.url) + " -o " + sh_quote(dst.string())) == 0) {
                    need_local = true;
                }
            }
        }
        if (need_local) {
            run_cmd("sudo pacman -U --noconfirm " + sh_quote(tmp_dir.string()) + "/03_dep-*.pkg.tar.zst");
        }
    } else {
        run_cmd("sudo pacman -S --needed --noconfirm ffmpeg mpv sdl2-compat lz4 glew glm wayland procps-ng xdg-utils gcc libjxl libplacebo sndio curl zstd psmisc");
    }
    deps.clear();
    deps.shrink_to_fit();

    std::cout << "[3/5] Deploying ultra-optimized NOCEF engine & default 4K scene into " << target_dir.string() << "...\n";
fs::create_directories(target_dir / "bin");
fs::create_directories(target_dir / "state");
fs::create_directories(target_dir / "wallpapers" / "scenes");
fs::create_directories(target_dir / "wallpapers" / "scenes_hidden");
fs::create_directories(target_dir / "wallpapers" / "vids");
fs::create_directories(target_dir / "wallpapers" / "vids_hidden");
fs::create_directories(target_dir / "wallpapers" / "photos");
fs::create_directories(target_dir / "wallpapers" / "photos_hidden");
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
#ifdef INSTALL_PREBUILT
std::cout << "[4/5] Installing prebuilt wectl binary...\n";
{
const fs::path bin_arc = tmp_dir / "wectl-bin.tar.zst";
run_cmd("curl -fL --progress-bar --retry 3 " + sh_quote(bin_rel_url) + " -o " + sh_quote(bin_arc.string()));
run_cmd("tar -C " + sh_quote(target_dir.string()) + " --zstd -xf " + sh_quote(bin_arc.string()));
}
#else
std::cout << "[4/5] Compiling 7 C++20 modules (-O3 -march=native -flto)...\n";
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
"g++ " + flags + " -c modules/banner.cppm -o banner.o && "
"g++ " + flags + " -c modules/core.cppm -o core.o && "
"g++ " + flags + " -c modules/ipc.cppm -o ipc.o && "
"g++ " + flags + " -c modules/render.cppm -o render.o && "
"g++ " + flags + " -c modules/ui.cppm -o ui.o && "
"g++ " + flags + " -c modules/server.cppm -o server.o && "
"g++ " + flags + " -c modules/menu.cppm -o menu.o && "
"g++ " + flags + " -c main.cpp -o main.o && "
"g++ " + flags + " banner.o core.o ipc.o render.o ui.o server.o menu.o main.o -o " + sh_quote(out_bin) + " && "
"strip -s " + sh_quote(out_bin);
if (run_cmd(cmd) != 0) {
std::cerr << "Error: Compilation failed.\n";
fs::remove_all(tmp_dir);
return 1;
}
}
#endif
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
unit << "[Unit]\nDescription=Pure C++20 Wallpaper Engine Gallery Daemon (Hyprland)\n"
<< "After=graphical-session.target\nStartLimitIntervalSec=0\n\n"
<< "[Service]\nPassEnvironment=WAYLAND_DISPLAY XDG_RUNTIME_DIR HYPRLAND_INSTANCE_SIGNATURE\n"
<< "Environment="PATH=" << (target_dir / "bin").string() << ":" << local_bin.string() << ":/usr/local/bin:/usr/bin:/bin"\n"
<< "ExecStart=" << (target_dir / "wectl").string() << " daemon\n"
<< "Restart=always\nRestartSec=1\n\n[Install]\nWantedBy=default.target\n";
}
run_cmd("systemctl --user import-environment WAYLAND_DISPLAY XDG_RUNTIME_DIR HYPRLAND_INSTANCE_SIGNATURE PATH >/dev/null 2>&1 || true");
run_cmd("systemctl --user daemon-reload >/dev/null 2>&1");
run_cmd("systemctl --user reset-failed wectl.service >/dev/null 2>&1 || true");
run_cmd("systemctl --user enable --now wectl.service >/dev/null 2>&1");
run_cmd("systemctl --user restart wectl.service >/dev/null 2>&1");
fs::remove_all(tmp_dir);
if (!self_path.empty() && fs::exists(self_path)) {
std::error_code ec;
fs::remove(self_path, ec);
}
std::cout << "\nDone! Portable installation ready in: " << target_dir.string() << "\n"
<< "Commands: wectl up | wectl status | wectl stop | wectl uninstall\n";
return 0;
}
