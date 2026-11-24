export module wectl.web.sec_deploy;
import <string>;

export namespace wectl::web::sec {
    std::string render_deploy() {
        return R"HTML(
<section class="container section">
  <span class="sec-tag neon-title-violet kinetic-text" data-theme="violet">// MODULE 03 — DEPLOYMENT, REINSTALL &amp; PURGE</span>
  <h2 class="sec-title kinetic-text" data-theme="violet">Lifecycle Automation Scripts</h2>
  <div class="grid-3">
    <div class="mica-panel mod-violet copy-block">
      <span class="copy-badge">CLICK TO COPY</span>
      <h3 class="neon-title-violet kinetic-text" data-theme="violet">One-Line Prebuilt Install</h3>
      <pre class="code-pre">curl -fsSL https://githubusercontent.com | bash</pre>
    </div>
    <div class="mica-panel mod-violet copy-block">
      <span class="copy-badge">CLICK TO COPY</span>
      <h3 class="neon-title-violet kinetic-text" data-theme="violet">Source Compile Install</h3>
      <pre class="code-pre">curl -fsSL https://githubusercontent.com | bash</pre>
    </div>
    <div class="mica-panel mod-violet copy-block">
      <span class="copy-badge">CLICK TO COPY</span>
      <h3 class="neon-title-violet kinetic-text" data-theme="violet">Force Reinstall &amp; Purge</h3>
      <pre class="code-pre"># Clean wipe &amp; reinstall:
wectl reinstall --force
# Complete removal:
wectl uninstall --purge</pre>
    </div>
  </div>
</section>
)HTML";
    }
}
