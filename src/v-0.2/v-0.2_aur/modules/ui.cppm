module;
#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <filesystem>
#include <string>
#include <string_view>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

export module wectl.ui;
import wectl.core;

export namespace wectl::ui {
namespace fs = std::filesystem;
using namespace wectl::core;

struct GalleryItem {
    std::string id;
    std::string title;
    std::string kind;
    std::string res;
    std::string thumb;
    bool anim = false;
};

inline void append_url_enc(std::string& out, std::string_view s) {
    for (unsigned char c : s) {
        if (c == ' ') out += "%20";
        else if (c == '#') out += "%23";
        else if (c == '?') out += "%3F";
        else out.push_back(static_cast<char>(c));
    }
}

inline void append_html_esc(std::string& out, std::string_view s) {
    for (char c : s) {
        if (c == '&') out += "&amp;";
        else if (c == '<') out += "&lt;";
        else if (c == '>') out += "&gt;";
        else if (c == '"') out += "&quot;";
        else if (c == '\'') out += "&#39;";
        else out.push_back(c);
    }
}

inline void append_json_esc(std::string& out, std::string_view s) {
    for (char c : s) {
        if (c == '\\') out += "\\\\";
        else if (c == '"') out += "\\\"";
        else if (c == '\n') out += "\\n";
        else out.push_back(c);
    }
}

inline std::string probe_image_res(const fs::path& fp) {
    const int fd = open(fp.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0) return {};
    std::array<uint8_t, 4096> b{};
    const ssize_t n = read(fd, b.data(), b.size());
    close(fd);
    if (n < 24) return {};

    // PNG
    if (b.at(0) == 0x89 && b.at(1) == 0x50 && b.at(2) == 0x4E && b.at(3) == 0x47) {
        const uint32_t w = (uint32_t(b.at(16)) << 24) | (uint32_t(b.at(17)) << 16) | (uint32_t(b.at(18)) << 8) | b.at(19);
        const uint32_t h = (uint32_t(b.at(20)) << 24) | (uint32_t(b.at(21)) << 16) | (uint32_t(b.at(22)) << 8) | b.at(23);
        if (w > 0 && h > 0) return std::to_string(w) + "x" + std::to_string(h);
    }
    // JPEG
    if (b.at(0) == 0xFF && b.at(1) == 0xD8) {
        size_t i = 2;
        const size_t limit = static_cast<size_t>(n);
        while (i + 9 < limit) {
            if (b.at(i) != 0xFF) { ++i; continue; }
            const uint8_t m = b.at(i + 1);
            if (m == 0xC0 || m == 0xC1 || m == 0xC2) {
                const uint32_t h = (uint32_t(b.at(i + 5)) << 8) | b.at(i + 6);
                const uint32_t w = (uint32_t(b.at(i + 7)) << 8) | b.at(i + 8);
                if (w > 0 && h > 0) return std::to_string(w) + "x" + std::to_string(h);
                break;
            }
            const uint32_t len = (uint32_t(b.at(i + 2)) << 8) | b.at(i + 3);
            if (len < 2) break;
            i += 2 + len;
        }
    }
    // WebP VP8X
    if (n >= 30 && b.at(0) == 'R' && b.at(1) == 'I' && b.at(2) == 'F' && b.at(3) == 'F' &&
        b.at(12) == 'V' && b.at(13) == 'P' && b.at(14) == '8' && b.at(15) == 'X') {
        const uint32_t w = 1 + (uint32_t(b.at(24)) | (uint32_t(b.at(25)) << 8) | (uint32_t(b.at(26)) << 16));
        const uint32_t h = 1 + (uint32_t(b.at(27)) | (uint32_t(b.at(28)) << 8) | (uint32_t(b.at(29)) << 16));
        if (w > 0 && h > 0) return std::to_string(w) + "x" + std::to_string(h);
    }
    return {};
}

inline std::string probe_video_res_cached(const fs::path& fp) {
    const fs::path meta = state_dir() / "meta" / (fp.filename().string() + ".res");
    const std::string cached = read_file(meta, "");
    if (!cached.empty()) return cached;

    std::array<int, 2> pipefd{};
    if (pipe2(pipefd.data(), O_CLOEXEC) != 0) return {};

    const std::string in_s = fp.string();
    const pid_t pid = fork();
    if (pid == 0) {
        close(pipefd.at(0));
        dup2(pipefd.at(1), STDOUT_FILENO);
        close(pipefd.at(1));
        const int devnull = open("/dev/null", O_RDWR);
        if (devnull >= 0) {
            dup2(devnull, STDIN_FILENO);
            dup2(devnull, STDERR_FILENO);
            if (devnull > 2) close(devnull);
        }
        std::array<const char*, 12> argv = {
            "ffprobe", "-v", "error",
            "-select_streams", "v:0",
            "-show_entries", "stream=width,height",
            "-of", "csv=s=x:p=0",
            in_s.c_str(),
            nullptr
        };
        execvp("ffprobe", const_cast<char* const*>(argv.data()));
        _exit(127);
    }

    close(pipefd.at(1));
    std::array<char, 64> out{};
    const ssize_t n = read(pipefd.at(0), out.data(), out.size() - 1);
    close(pipefd.at(0));
    if (pid > 0) waitpid(pid, nullptr, 0);

    if (n > 0) {
        std::string res(out.data(), static_cast<size_t>(n));
        while (!res.empty() && (res.back() == '\n' || res.back() == '\r' || res.back() == ' ')) res.pop_back();
        if (res.find('x') != std::string::npos) {
            write_file(meta, res);
            return res;
        }
    }
    return {};
}

inline std::string probe_scene_res(const fs::path& dir) {
    std::string up = dir.filename().string();
    for (char& c : up) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    if (up.find("3840X2160") != std::string::npos || up.find("4K") != std::string::npos) return "3840x2160";
    if (up.find("2560X1440") != std::string::npos || up.find("2K") != std::string::npos) return "2560x1440";
    if (up.find("1920X1080") != std::string::npos || up.find("1080P") != std::string::npos) return "1920x1080";
    return {};
}

inline std::pair<std::string, bool> find_scene_thumb(const fs::path& dir) {
    std::error_code ec;
    for (const char* name : {"preview.gif", "preview.jpg", "preview.png", "preview.webp"}) {
        if (fs::exists(dir / name, ec)) {
            std::string res = "/media/scenes/";
            append_url_enc(res, dir.filename().string());
            res.push_back('/');
            res += name;
            return {res, std::string_view(name) == "preview.gif"};
        }
    }
    return {{}, false};
}


inline std::string render_gallery() {
    const int max_hz = load_hw().max_hz;
    const int cur_fps = clamp_fps(std::atoi(read_file(state_dir() / "fps", "60").c_str()));
    std::string cur_id = read_file(state_dir() / "current", "");
    if (!cur_id.empty() && cur_id.find(':') == std::string::npos) cur_id = "scene:" + cur_id;

    std::string init_tab = "scene";
    if (cur_id.rfind("vid:", 0) == 0) init_tab = "video";
    else if (cur_id.rfind("photo:", 0) == 0) init_tab = "photo";

    std::vector<GalleryItem> items;
    std::error_code ec;

    if (fs::exists(scenes_dir(), ec)) {
        for (const auto& e : fs::directory_iterator(scenes_dir(), ec)) {
            if (!e.is_directory(ec)) continue;
            const std::string fn = e.path().filename().string();
            auto [th, is_anim] = find_scene_thumb(e.path());
            items.push_back({"scene:" + fn, fn, "scene", probe_scene_res(e.path()), std::move(th), is_anim});
        }
    }
    if (fs::exists(vids_dir(), ec)) {
        for (const auto& e : fs::directory_iterator(vids_dir(), ec)) {
            if (!e.is_regular_file(ec)) continue;
            const std::string fn = e.path().filename().string();
            std::string url = "/vthumb/";
            append_url_enc(url, fn);
            items.push_back({"vid:" + fn, e.path().stem().string(), "video", probe_video_res_cached(e.path()), std::move(url), false});
        }
    }
    if (fs::exists(photos_dir(), ec)) {
        for (const auto& e : fs::directory_iterator(photos_dir(), ec)) {
            if (!e.is_regular_file(ec)) continue;
            const std::string fn = e.path().filename().string();
            std::string url = "/media/photos/";
            append_url_enc(url, fn);
            items.push_back({"photo:" + fn, e.path().stem().string(), "photo", probe_image_res(e.path()), std::move(url), false});
        }
    }

    std::sort(items.begin(), items.end(), [](const GalleryItem& a, const GalleryItem& b) {
        return a.title < b.title;
    });

    const std::string max_s = std::to_string(max_hz);
    const std::string cur_s = std::to_string(cur_fps);

    std::string h;
    h.reserve(5400 + items.size() * 380);
    h += "<!doctype html><html><head><meta charset='utf-8'><title>wectl</title>"
         "<style>*{box-sizing:border-box}body{margin:0;background:#0d1117;color:#e6edf3;font-family:system-ui,-apple-system,sans-serif}"
         ".bar{position:sticky;top:0;z-index:10;display:flex;flex-wrap:wrap;justify-content:space-between;align-items:center;gap:12px;padding:12px 20px;background:rgba(22,27,34,.92);backdrop-filter:blur(10px);border-bottom:1px solid #30363d}"
         ".left{display:flex;align-items:center;gap:10px;flex-wrap:wrap}"
         ".search{width:210px;background:#0d1117;color:#e6edf3;border:1px solid #30363d;border-radius:10px;padding:7px 12px;font-size:13px;outline:none;transition:.15s}"
         ".search:focus{border-color:#58a6ff;box-shadow:0 0 0 3px rgba(88,166,255,.15)}"
         ".tabs{display:flex;background:#0d1117;border:1px solid #30363d;border-radius:10px;padding:3px;gap:3px}"
         ".tab{background:transparent;color:#8b949e;border:none;border-radius:7px;padding:5px 10px;font-size:12px;font-weight:600;cursor:pointer;transition:.15s}"
         ".tab.on{background:#21262d;color:#e6edf3}"
         ".fps-pill{display:flex;align-items:center;gap:10px;background:#0d1117;border:1px solid #30363d;border-radius:10px;padding:5px 12px}"
         ".fps-lbl{font-size:11px;font-weight:600;color:#8b949e}"
         "input[type=range]{-webkit-appearance:none;appearance:none;width:105px;height:4px;border-radius:99px;background:#30363d;outline:none;cursor:pointer}"
         "input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;appearance:none;width:14px;height:14px;border-radius:50%;background:#58a6ff;border:2px solid #0d1117;cursor:pointer}"
         ".fps-num{width:46px;background:transparent;border:none;border-left:1px solid #21262d;padding:2px 0 2px 8px;color:#e6edf3;font-size:13px;font-weight:600;text-align:right;outline:none;-moz-appearance:textfield}"
         ".fps-num::-webkit-inner-spin-button,.fps-num::-webkit-outer-spin-button{-webkit-appearance:none;margin:0}"
         ".grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(240px,1fr));gap:16px;padding:20px}"
         ".c{position:relative;background:#161b22;border:2px solid #30363d;border-radius:12px;overflow:hidden;cursor:pointer;transition:transform .18s ease,border-color .18s ease,box-shadow .18s ease}"
         ".c:hover{transform:translateY(-3px);border-color:#58a6ff;box-shadow:0 8px 24px rgba(0,0,0,.35)}.c.act{border-color:#3fb950}"
         ".th{height:140px;background:#010409;display:flex;align-items:center;justify-content:center;color:#484f58;font-size:12px;overflow:hidden}"
         ".th img{width:100%;height:100%;object-fit:cover;display:block;transition:transform .35s ease}"
         ".c:hover .th img.anim{transform:scale(1.06)}"
         ".res{position:absolute;top:8px;left:8px;z-index:2;background:rgba(13,17,23,.84);color:#c9d1d9;border:1px solid rgba(255,255,255,.12);border-radius:6px;padding:2px 7px;font-size:10px;font-weight:700;letter-spacing:.02em}"
         ".ft{display:flex;align-items:center;justify-content:space-between;gap:8px;padding:9px 11px}"
         ".nm{font-size:13px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;flex:1}"
         ".kind{flex-shrink:0;background:#0d1117;color:#8b949e;border:1px solid #30363d;border-radius:5px;padding:2px 6px;font-size:10px;font-weight:700;text-transform:uppercase}"
         ".del{position:absolute;top:8px;right:8px;z-index:2;width:24px;height:24px;display:flex;align-items:center;justify-content:center;background:rgba(13,17,23,.84);color:#8b949e;border:1px solid rgba(255,255,255,.12);border-radius:6px;font-size:11px;cursor:pointer}"
         ".del:hover{color:#fff;border-color:#f85149}"
         "#msg{position:fixed;bottom:18px;right:18px;background:#238636;color:#fff;padding:9px 15px;border-radius:9px;display:none;font-size:13px}"
         "</style></head><body><div class='bar'><div class='left'>"
         "<input id='q' class='search' placeholder='Search...' oninput='flt()'>"
         "<div class='tabs'>"
         "<button class='tab' data-k='scene' onclick='setTab(\"scene\")'>Scenes</button>"
         "<button class='tab' data-k='video' onclick='setTab(\"video\")'>Videos</button>"
         "<button class='tab' data-k='photo' onclick='setTab(\"photo\")'>Photos</button>"
         "</div></div><div class='fps-pill'><span class='fps-lbl'>FPS</span>"
         "<input id='fpsBar' type='range' min='15' max='" + max_s + "' value='" + cur_s + "' oninput='syncNum(this.value)' onchange='applyFps(this.value)'>"
         "<input id='fpsNum' class='fps-num' type='number' min='15' max='" + max_s + "' value='" + cur_s + "' oninput='syncBar(this.value)' onchange='applyFps(this.value)'>"
         "</div></div><div class='grid'>";

    for (const auto& it : items) {
        h += "<div class='c";
        if (it.id == cur_id) h += " act";
        h += "' data-id='";
        append_html_esc(h, it.id);
        h += "' data-n='";
        append_html_esc(h, it.title);
        h += "' data-k='";
        h += it.kind;
        h += "' onclick='runWp(this.dataset.id,this.dataset.n)'>";
        if (!it.res.empty()) {
            h += "<span class='res'>";
            append_html_esc(h, it.res);
            h += "</span>";
        }
        h += "<button class='del' title='Hide to *_hidden' onclick='rm(event,this.parentElement)'>✕</button><div class='th'>";
        if (it.thumb.empty()) {
            h += "No preview";
        } else {
            h += "<img src='";
            h += it.thumb;
            h += "' class='";
            if (it.anim) h += "anim";
            h += "' loading='lazy' decoding='async'>";
        }
        h += "</div><div class='ft'><div class='nm'>";
        append_html_esc(h, it.title);
        h += "</div><span class='kind'>";
        h += it.kind;
        h += "</span></div></div>";
    }

    h += "</div><div id='msg'></div><" "script>let cur='";
    append_json_esc(h, cur_id);
    h += "';const modes=['scene','video','photo'];const labels={'scene':'Scenes','video':'Videos','photo':'Photos'};let tab='" + init_tab + "';const maxHz=" + max_s + ";"
         "const bar=document.getElementById('fpsBar'),num=document.getElementById('fpsNum');"
         "function paintBar(v){let p=((v-15)/(maxHz-15))*100;bar.style.background='linear-gradient(90deg,#58a6ff '+p+'%,#30363d '+p+'%)';}"
         "paintBar(bar.value);function clamp(v){v=parseInt(v)||60;return Math.max(15,Math.min(maxHz,v));}"
         "function syncNum(v){num.value=v;paintBar(v);}function syncBar(v){if(!v)return;let c=clamp(v);bar.value=c;paintBar(c);}"
         "function toast(t){let m=document.getElementById('msg');m.textContent=t;m.style.display='block';setTimeout(()=>m.style.display='none',1800);}"
         "function setTab(k){tab=k;document.querySelectorAll('.tab').forEach(x=>x.classList.toggle('on',x.dataset.k===k));flt();}"
         ""
         "function flt(){let q=document.getElementById('q').value.toLowerCase();document.querySelectorAll('.c').forEach(x=>{let ok=(x.dataset.k===tab)&&x.dataset.n.toLowerCase().includes(q);x.style.display=ok?'':'none';});}"
         "setTab(tab);"
         "async function applyFps(v){let c=clamp(v);bar.value=c;num.value=c;paintBar(c);if(cur)await runWp(cur,cur.split(':').slice(1).join(':'));else{await fetch('/fps?v='+c);toast('FPS: '+c);}}"
         "async function runWp(id,n){cur=id;let f=clamp(num.value);document.querySelectorAll('.c').forEach(x=>x.classList.toggle('act',x.dataset.id===id));await fetch('/set?s='+encodeURIComponent(id)+'&fps='+f);toast(n+' • '+f+' FPS');}"
         "async function rm(e,el){e.stopPropagation();await fetch('/hide?s='+encodeURIComponent(el.dataset.id));el.remove();}"
         "</" "script></" "body></" "html>";

    return h;
}

}

