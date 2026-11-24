export module wectl.web.sec_arch;
import <string>;

export namespace wectl::web::sec {
    std::string render_arch() {
        return R"HTML(
<section class="container section">
  <span class="sec-tag neon-title-crimson kinetic-text" data-theme="crimson">// MODULE 01 — CORE ARCHITECTURE &amp; REQUIREMENTS</span>
  <h2 class="sec-title kinetic-text" data-theme="crimson">Hardware Specs &amp; Repository Links</h2>
  <div class="grid-2">
    <div class="mica-panel mod-crimson copy-block">
      <span class="copy-badge">CLICK TO COPY</span>
      <h3 class="neon-title-crimson kinetic-text" data-theme="crimson">Minimum Hardware &amp; OS Stack</h3>
      <ul class="spec-list">
        <li><strong>OS / Compositor:</strong> Arch Linux (or Arch-based) with Wayland compositor (Hyprland, Sway, KDE Plasma Wayland).</li>
        <li><strong>GPU / Graphics:</strong> Vulkan / OpenGL ES 3.0+ compatible GPU with Mesa or proprietary NVIDIA drivers.</li>
        <li><strong>Compiler Stack:</strong> Clang 17+ or GCC 14+ with full C++20 Modules (<code>.cppm</code>) and <code>zstd</code> compression support.</li>
        <li><strong>Footprint Target:</strong> &lt; 20 MB binary distribution; 0% GPU load during active fullscreen windows.</li>
      </ul>
    </div>
    <div class="mica-panel mod-crimson copy-block">
      <span class="copy-badge">CLICK TO COPY</span>
      <h3 class="neon-title-crimson kinetic-text" data-theme="crimson">Git Repositories &amp; Upstream Dependencies</h3>
      <pre class="code-pre">Main Repository:   https://github.com
Upstream Engine:   https://github.com
System Packages:   wayland-protocols libxkbcommon cmake clang zstd
Target Runtime:    /home/dev/zapret/.system/wectl/</pre>
    </div>
  </div>
</section>
)HTML";
    }
}
