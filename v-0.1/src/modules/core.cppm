module;
#include <cstdlib>
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
