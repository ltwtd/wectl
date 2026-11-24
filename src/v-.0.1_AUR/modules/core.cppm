module;
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <algorithm>
#include <unistd.h>
#include <sys/stat.h>

export module wectl.core;

export namespace wectl::core {

namespace fs = std::filesystem;

inline std::string trim(const std::string& s) {
    size_t b = 0;
    while (b < s.size() && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    size_t e = s.size();
    while (e > b && std::isspace(static_cast<unsigned char>(s[e - 1]))) --e;
    return s.substr(b, e - b);
}

inline std::string get_env_str(const char* name, const std::string& def = "") {
    const char* v = std::getenv(name);
    return (v && *v) ? std::string(v) : def;
}

inline fs::path root_dir() {
    std::string xdg = get_env_str("XDG_DATA_HOME");
    if (!xdg.empty()) return fs::path(xdg) / "wectl";
    std::string home = get_env_str("HOME", "/tmp");
    return fs::path(home) / ".local/share/wectl";
}

inline fs::path state_dir() { return root_dir() / "state"; }
inline fs::path wallpapers_dir() { return root_dir() / "wallpapers"; }
inline fs::path scenes_dir() { return wallpapers_dir() / "scenes"; }
inline fs::path scenes_hidden_dir() { return wallpapers_dir() / "scenes_hidden"; }
inline fs::path hidden_dir() { return scenes_hidden_dir(); }

inline std::string sh_quote(const std::string& s) {
    std::string out = "'";
    for (char c : s) {
        if (c == '\'') out += "'\\''";
        else out += c;
    }
    out += "'";
    return out;
}

inline std::string exec_capture(const std::string& cmd) {
    std::string res;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return res;
    char buf[256];
    while (fgets(buf, sizeof(buf), pipe) != nullptr) {
        res += buf;
    }
    pclose(pipe);
    return trim(res);
}

inline std::string read_file(const fs::path& p, const std::string& def = "") {
    std::ifstream in(p);
    if (!in.is_open()) return def;
    std::ostringstream ss;
    ss << in.rdbuf();
    std::string val = trim(ss.str());
    return val.empty() ? def : val;
}

inline void write_file(const fs::path& p, const std::string& val) {
    std::error_code ec;
    fs::create_directories(p.parent_path(), ec);
    std::ofstream out(p, std::ios::trunc);
    if (out.is_open()) out << val << "\n";
}

inline int clamp_fps(int v) {
    if (v < 1) return 1;
    if (v > 360) return 360;
    return v;
}

struct hw_info {
    std::string monitor = "eDP-1";
    std::string resolution = "1920x1080";
    int max_hz = 60;
    std::string scaling = "fill";
};

inline void ensure_initialized() {
    std::error_code ec;
    fs::create_directories(state_dir(), ec);
    fs::create_directories(scenes_dir(), ec);
    fs::create_directories(scenes_hidden_dir(), ec);
    fs::create_directories(wallpapers_dir() / "photos", ec);
    fs::create_directories(wallpapers_dir() / "vids", ec);

    if (!fs::exists(state_dir() / "monitor")) {
        std::string mon = exec_capture("hyprctl monitors -j | grep -o '\"name\": *\"[^\"]*\"' | head -n1 | cut -d'\"' -f4");
        if (mon.empty()) mon = "eDP-1";
        write_file(state_dir() / "monitor", mon);

        std::string w = exec_capture("hyprctl monitors -j | grep -o '\"width\": *[0-9]*' | head -n1 | grep -o '[0-9]*'");
        std::string h = exec_capture("hyprctl monitors -j | grep -o '\"height\": *[0-9]*' | head -n1 | grep -o '[0-9]*'");
        if (!w.empty() && !h.empty()) {
            write_file(state_dir() / "resolution", w + "x" + h);
        } else {
            write_file(state_dir() / "resolution", "1920x1080");
        }

        std::string hz = exec_capture("hyprctl monitors -j | grep -o '\"refreshRate\": *[0-9]*' | head -n1 | grep -o '[0-9]*'");
        write_file(state_dir() / "max_hz", hz.empty() ? "60" : hz);
        write_file(state_dir() / "scaling", "fill");
        write_file(state_dir() / "fps", "60");
    }
}

inline hw_info load_hw() {
    ensure_initialized();
    hw_info hw;
    hw.monitor = read_file(state_dir() / "monitor", "eDP-1");
    hw.resolution = read_file(state_dir() / "resolution", "1920x1080");
    try { hw.max_hz = std::stoi(read_file(state_dir() / "max_hz", "60")); } catch (...) { hw.max_hz = 60; }
    hw.scaling = read_file(state_dir() / "scaling", "fill");
    return hw;
}

inline std::string get_scene_title(const fs::path& scene_path) {
    fs::path pj = scene_path / "project.json";
    if (!fs::exists(pj)) return scene_path.filename().string();
    std::ifstream in(pj);
    if (!in.is_open()) return scene_path.filename().string();
    std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    size_t pos = content.find("\"title\"");
    if (pos == std::string::npos) return scene_path.filename().string();
    pos = content.find(':', pos);
    if (pos == std::string::npos) return scene_path.filename().string();
    size_t q1 = content.find('"', pos + 1);
    if (q1 == std::string::npos) return scene_path.filename().string();
    size_t q2 = q1 + 1;
    while (q2 < content.size()) {
        if (content[q2] == '"' && content[q2 - 1] != '\\') break;
        ++q2;
    }
    if (q2 >= content.size()) return scene_path.filename().string();
    std::string title = content.substr(q1 + 1, q2 - q1 - 1);
    return title.empty() ? scene_path.filename().string() : title;
}

struct scene_entry {
    std::string id;
    std::string title;
    fs::path path;
};

inline std::vector<scene_entry> list_scenes() {
    ensure_initialized();
    std::vector<scene_entry> list;
    std::error_code ec;
    if (!fs::exists(scenes_dir(), ec)) return list;
    for (const auto& entry : fs::directory_iterator(scenes_dir(), ec)) {
        if (!entry.is_directory(ec)) continue;
        std::string id = entry.path().filename().string();
        if (id.empty() || id[0] == '.') continue;
        list.push_back({id, get_scene_title(entry.path()), entry.path()});
    }
    std::sort(list.begin(), list.end(), [](const scene_entry& a, const scene_entry& b) {
        return a.title < b.title;
    });
    return list;
}

inline std::string resolve_scene(const std::string& query) {
    auto list = list_scenes();
    for (const auto& item : list) {
        if (item.id == query) return item.id;
    }
    for (const auto& item : list) {
        if (item.title == query) return item.id;
    }
    return query;
}

inline std::string service_status() {
    std::string out = exec_capture("systemctl --user is-active wectl.service 2>/dev/null");
    return out.empty() ? "inactive" : out;
}

inline bool is_service_active() {
    return service_status() == "active";
}

inline void service_up() {
    std::system("systemctl --user enable --now wectl.service >/dev/null 2>&1");
}

inline void service_stop() {
    std::system("systemctl --user stop wectl.service >/dev/null 2>&1");
}

inline void service_down() {
    std::system("systemctl --user disable --now wectl.service >/dev/null 2>&1");
}

inline void ensure_linked() {
    ensure_initialized();
}

inline void relocate_self(bool = false) { ensure_initialized(); }
inline void _unused_uninstall() {
    service_down();
}

} // namespace wectl::core

