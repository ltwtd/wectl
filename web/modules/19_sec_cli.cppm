export module wectl.web.sec_cli;
import <string>;

export namespace wectl::web::sec {
    std::string render_cli() {
        return R"HTML(
<section class="container section">
  <span class="sec-tag neon-title-violet kinetic-text" data-theme="violet">// MODULE 04 — CLI REFERENCE &amp; SYSTEM LIMITS</span>
  <h2 class="sec-title kinetic-text" data-theme="violet">Daemon Control &amp; v-0.1 Constraints</h2>
  <div class="grid-2">
    <div class="mica-panel mod-violet copy-block">
      <span class="copy-badge">CLICK TO COPY</span>
      <h3 class="neon-title-violet kinetic-text" data-theme="violet">CLI Command Matrix</h3>
      <pre class="code-pre">wectl up                  # Start daemon &amp; restore last scene
wectl down                # Pause rendering &amp; release GPU context
wectl stop                # Kill daemon + terminate linux-wallpaperengine
wectl status              # Show PID, active scene, FPS, VRAM &amp; pause state
wectl set &lt;id&gt; --fps 144  # Bind scene with custom refresh rate cap</pre>
    </div>
    <div class="mica-panel mod-violet copy-block">
      <span class="copy-badge">CLICK TO COPY</span>
      <h3 class="neon-title-violet kinetic-text" data-theme="violet">v-0.1 Technical Limitations</h3>
      <ul class="spec-list">
        <li><strong>Scene-Only Engine:</strong> v-0.1 renders Workshop <code>scenes/</code> exclusively; <code>vids/</code> and <code>photos/</code> directories are pre-scaffolded for v-0.2.</li>
        <li><strong>Single-Output Lock:</strong> Multi-monitor independent per-display scene binding is deferred until v-0.3.</li>
        <li><strong>Strict Process Ownership:</strong> Stopping <code>wectl</code> immediately sends <code>SIGTERM/SIGKILL</code> to child render processes to prevent zombie GPU locks.</li>
      </ul>
    </div>
  </div>
</section>
)HTML";
    }
}
