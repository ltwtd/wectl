export module wectl.web.sec_hero;
import <string>;

export namespace wectl::web::sec {
    std::string render_hero() {
        return R"HTML(
<nav class="hud-nav">
  <div class="hud-brand kinetic-text" data-theme="cyan">WECTL // PHENOMENON SPEC v0.1</div>
  <div class="hud-controls">
    <button class="mode-btn active" data-mode="0">ASTRAL MONO</button>
    <button class="mode-btn" data-mode="1">CRIMSON</button>
    <button class="mode-btn" data-mode="2">VIOLET</button>
    <button class="mode-btn" data-mode="3">CYAN</button>
  </div>
</nav>

<header class="container hero">
  <span class="sec-tag neon-title-crimson kinetic-text" data-theme="crimson">// WAYLAND LIVE WALLPAPER ENGINE WRAPPER</span>
  <h1><span class="kinetic-text" data-theme="crimson">Zero-Overhead</span><br><span class="kinetic-text" data-theme="cyan">Phenomenon Render</span></h1>
  <p class="kinetic-text" data-theme="violet">Ultra-lightweight C++20 daemon and CLI orchestrator for linux-wallpaperengine on Arch Linux &amp; Wayland. Designed for instant fullscreen auto-pause, strict process lifecycle binding, and sub-20MB deployment footprint.</p>
</header>
)HTML";
    }
}
