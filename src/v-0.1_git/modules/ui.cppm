module;
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

export module wectl.ui;
import wectl.core;

export namespace wectl::ui {
namespace fs = std::filesystem;
using namespace wectl::core;

inline std::string url_encode_path(std::string_view s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (unsigned char c : s) {
        if (c == ' ') out += "%20";
        else if (c == '#') out += "%23";
        else if (c == '?') out += "%3F";
        else out.push_back(static_cast<char>(c));
    }
    return out;
}

inline std::string html_esc(std::string_view s) {
    std::string o;
    o.reserve(s.size() + 8);
    for (char c : s) {
        if (c == '&') o += "&amp;";
        else if (c == '<') o += "&lt;";
        else if (c == '>') o += "&gt;";
        else if (c == '"') o += "&quot;";
        else if (c == '\'') o += "&#39;";
        else o.push_back(c);
    }
    return o;
}

inline std::string json_esc(std::string_view s) {
    std::string o;
    o.reserve(s.size() + 4);
    for (char c : s) {
        if (c == '\\') o += "\\\\";
        else if (c == '"') o += "\\\"";
        else if (c == '\n') o += "\\n";
        else o.push_back(c);
    }
    return o;
}

inline std::string find_thumb(const fs::path& dir) {
    for (const char* name : {"preview.gif", "preview.jpg", "preview.png", "preview.webp"}) {
        if (fs::exists(dir / name)) {
            return "/thumb/" + url_encode_path(dir.filename().string()) + "/" + name;
        }
    }
    return {};
}

inline std::string render_gallery() {
    const int max_hz = load_hw().max_hz;
    const int cur_fps = clamp_fps(std::atoi(read_file(state_dir() / "fps", "60").c_str()));
    const std::string cur_scene = read_file(state_dir() / "current", "");

    std::vector<fs::path> scenes;
    if (fs::exists(scenes_dir())) {
        for (const auto& e : fs::directory_iterator(scenes_dir())) {
            if (e.is_directory()) scenes.push_back(e.path());
        }
    }
    std::sort(scenes.begin(), scenes.end());

    std::ostringstream h;
    h << "<!doctype html><html><head><meta charset='utf-8'><title>wectl</title>"
      << "<style>*{box-sizing:border-box}body{margin:0;background:#0d1117;color:#e6edf3;font-family:system-ui,-apple-system,sans-serif}"
      << ".bar{position:sticky;top:0;z-index:10;display:flex;justify-content:space-between;align-items:center;gap:14px;padding:12px 20px;background:rgba(22,27,34,.92);backdrop-filter:blur(10px);border-bottom:1px solid #30363d}"
      << ".search{width:260px;max-width:45vw;background:#0d1117;color:#e6edf3;border:1px solid #30363d;border-radius:10px;padding:8px 14px;font-size:13px;outline:none;transition:.15s}"
      << ".search:focus{border-color:#58a6ff;box-shadow:0 0 0 3px rgba(88,166,255,.15)}"
      << ".ctrl{display:flex;align-items:center;gap:12px}"
      << ".fps-pill{display:flex;align-items:center;gap:10px;background:#0d1117;border:1px solid #30363d;border-radius:10px;padding:5px 12px;transition:.15s}"
      << ".fps-pill:focus-within{border-color:#58a6ff;box-shadow:0 0 0 3px rgba(88,166,255,.15)}"
      << ".fps-lbl{font-size:11px;font-weight:600;letter-spacing:.04em;color:#8b949e}"
      << "input[type=range]{-webkit-appearance:none;appearance:none;width:110px;height:4px;border-radius:99px;background:#30363d;outline:none;cursor:pointer}"
      << "input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;appearance:none;width:14px;height:14px;border-radius:50%;background:#58a6ff;border:2px solid #0d1117;cursor:pointer}"
      << ".fps-num{width:46px;background:transparent;border:none;border-left:1px solid #21262d;padding:2px 0 2px 8px;color:#e6edf3;font-size:13px;font-weight:600;text-align:right;outline:none;-moz-appearance:textfield}"
      << ".fps-num::-webkit-inner-spin-button,.fps-num::-webkit-outer-spin-button{-webkit-appearance:none;margin:0}"
      << ".stop-btn{background:rgba(218,54,51,.15);color:#ff7b72;border:1px solid rgba(248,81,73,.4);border-radius:10px;padding:8px 16px;font-size:13px;font-weight:600;cursor:pointer;transition:.15s}"
      << ".stop-btn:hover{background:#da3633;color:#fff;border-color:#f85149}"
      << ".grid{display:grid;grid-template-columns:repeat(auto-fill,minmax(240px,1fr));gap:16px;padding:20px}"
      << ".c{position:relative;background:#161b22;border:2px solid #30363d;border-radius:12px;overflow:hidden;cursor:pointer;transition:.15s}"
      << ".c:hover{transform:translateY(-2px);border-color:#58a6ff}.c.act{border-color:#3fb950}"
      << ".th{height:140px;background:#010409;display:flex;align-items:center;justify-content:center;color:#484f58;font-size:12px}"
      << ".th img{width:100%;height:100%;object-fit:cover}"
      << ".nm{padding:10px 12px;font-size:13px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}"
      << ".del{position:absolute;top:8px;right:8px;width:24px;height:24px;display:flex;align-items:center;justify-content:center;background:rgba(13,17,23,.8);color:#8b949e;border:1px solid rgba(255,255,255,.1);border-radius:6px;font-size:11px;cursor:pointer}"
      << ".del:hover{color:#fff;border-color:#f85149}"
      << "#msg{position:fixed;bottom:18px;right:18px;background:#238636;color:#fff;padding:9px 15px;border-radius:9px;display:none;font-size:13px}"
      << "</style></head><body><div class='bar'>"
      << "<input id='q' class='search' placeholder='Search...' oninput='flt()'>"
      << "<div class='ctrl'><div class='fps-pill'><span class='fps-lbl'>FPS</span>"
      << "<input id='fpsBar' type='range' min='15' max='" << max_hz << "' value='" << cur_fps << "' oninput='syncNum(this.value)' onchange='applyFps(this.value)'>"
      << "<input id='fpsNum' class='fps-num' type='number' min='15' max='" << max_hz << "' value='" << cur_fps << "' oninput='syncBar(this.value)' onchange='applyFps(this.value)'>"
      << "</div><button class='stop-btn' onclick='stopWp()'>Stop</button></div></div><div class='grid'>";

    for (const auto& sp : scenes) {
        const std::string name = sp.filename().string();
        const std::string th = find_thumb(sp);
        h << "<div class='c" << (name == cur_scene ? " act" : "") << "' data-n='" << html_esc(name) << "' onclick='runWp(this.dataset.n)'>"
          << "<button class='del' title='Hide scene' onclick='rm(event,this.parentElement)'>✕</button>"
          << "<div class='th'>" << (th.empty() ? "No preview" : "<img src='" + th + "' loading='lazy'>") << "</div>"
          << "<div class='nm'>" << html_esc(name) << "</div></div>";
    }
    scenes.clear();
    scenes.shrink_to_fit();

    h << "</div><div id='msg'></div><script>"
      << "let cur='" << json_esc(cur_scene) << "';const maxHz=" << max_hz << ";"
      << "const bar=document.getElementById('fpsBar'),num=document.getElementById('fpsNum');"
      << "function paintBar(v){let p=((v-15)/(maxHz-15))*100;bar.style.background='linear-gradient(90deg,#58a6ff '+p+'%,#30363d '+p+'%)';}"
      << "paintBar(bar.value);function clamp(v){v=parseInt(v)||60;return Math.max(15,Math.min(maxHz,v));}"
      << "function syncNum(v){num.value=v;paintBar(v);}function syncBar(v){if(!v)return;let c=clamp(v);bar.value=c;paintBar(c);}"
      << "function toast(t){let m=document.getElementById('msg');m.textContent=t;m.style.display='block';setTimeout(()=>m.style.display='none',1800);}"
      << "function flt(){let q=document.getElementById('q').value.toLowerCase();document.querySelectorAll('.c').forEach(x=>{x.style.display=x.dataset.n.toLowerCase().includes(q)?'':'none';});}"
      << "async function applyFps(v){let c=clamp(v);bar.value=c;num.value=c;paintBar(c);if(cur)await runWp(cur);else{await fetch('/fps?v='+c);toast('FPS: '+c);}}"
      << "async function runWp(n){cur=n;let f=clamp(num.value);document.querySelectorAll('.c').forEach(x=>x.classList.toggle('act',x.dataset.n===n));await fetch('/set?s='+encodeURIComponent(n)+'&fps='+f);toast(n+' • '+f+' FPS');}"
      << "async function rm(e,el){e.stopPropagation();await fetch('/hide?s='+encodeURIComponent(el.dataset.n));el.remove();}"
      << "async function stopWp(){cur='';document.querySelectorAll('.c').forEach(x=>x.classList.remove('act'));await fetch('/stop');toast('Stopped (0% GPU)');}"
      << "</script></body></html>";

    std::string out = std::move(h).str();
    out.shrink_to_fit();
    return out;
}

}
