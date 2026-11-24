export module wectl.web.sec_roadmap;
import <string>;

export namespace wectl::web::sec {
    std::string render_roadmap() {
        return R"HTML(
<section class="container section">
  <span class="sec-tag neon-title-cyan kinetic-text" data-theme="cyan">// MODULE 05 — COMPLETED MILESTONES &amp; FUTURE ROADMAP</span>
  <h2 class="sec-title kinetic-text" data-theme="cyan">Development Trajectory</h2>
  <div class="grid-2">
    <div class="mica-panel mod-cyan copy-block">
      <span class="copy-badge">CLICK TO COPY</span>
      <h3 class="neon-title-cyan kinetic-text" data-theme="cyan">Completed in v-0.1</h3>
      <ul class="spec-list">
        <li><strong>[DONE] Modular C++20 Core:</strong> Split engine into 7 isolated <code>.cppm</code> modules (<code>banner, core, ipc, menu, render, server, ui</code>).</li>
        <li><strong>[DONE] Self-Cleaning Installers:</strong> Built <code>wectl-launcher-prebuilt</code> and <code>wectl-launcher-compile</code> with automated cleanup.</li>
        <li><strong>[DONE] Fullscreen Auto-Pause:</strong> Wayland compositor hook freezes scene rendering when games or media enter fullscreen.</li>
        <li><strong>[DONE] Hard Process Termination:</strong> Guaranteed teardown of <code>linux-wallpaperengine</code> upon <code>wectl stop</code>.</li>
      </ul>
    </div>
    <div class="mica-panel mod-cyan copy-block">
      <span class="copy-badge">CLICK TO COPY</span>
      <h3 class="neon-title-cyan kinetic-text" data-theme="cyan">Future Releases (v-0.2 &amp; v-0.3)</h3>
      <ul class="spec-list">
        <li><strong>[v-0.2] Video &amp; Photo Wallpapers:</strong> Native selection and playback support for <code>wallpapers/vids/</code> (mp4/webm) and <code>wallpapers/photos/</code> alongside scenes.</li>
        <li><strong>[v-0.2] Systemd Reboot Persistence:</strong> Seamless user-unit auto-restore after system reboot.</li>
        <li><strong>[v-0.3] Expanded Shader Compatibility:</strong> Custom GLSL translation patches to unlock complex Workshop post-processing shaders.</li>
        <li><strong>[v-0.3] Multi-Monitor Orchestration:</strong> Independent scene, scaling, and Hz rules per Wayland output.</li>
      </ul>
    </div>
  </div>
</section>
)HTML";
    }
}
