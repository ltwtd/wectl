export module wectl.web.sec_trees;
import <string>;

export namespace wectl::web::sec {
    std::string render_trees() {
        return R"HTML(
<section class="container section">
  <span class="sec-tag neon-title-crimson kinetic-text" data-theme="crimson">// MODULE 02 — SOURCE &amp; RUNTIME FILESYSTEM TREES</span>
  <h2 class="sec-title kinetic-text" data-theme="crimson">C++20 Module Architecture</h2>
  <div class="grid-2">
    <div class="mica-panel mod-crimson copy-block">
      <span class="copy-badge">CLICK TO COPY</span>
      <h3 class="neon-title-crimson kinetic-text" data-theme="crimson">Development Repository Tree</h3>
      <pre class="code-pre">/home/dev/zapret/dev/wectl
├── README.md
├── releases/v-0.1/
│   ├── wectl-bin-x86_64.tar.zst
│   ├── wectl-launcher-compile
│   ├── wectl-launcher-prebuilt
│   └── wectl-src.tar.zst
└── src/v-0.1/
    ├── INFO.md | launcher.cpp | main.cpp | Makefile
    ├── modules/
    │   ├── banner.cppm   # ASCII HUD &amp; version telemetry
    │   ├── core.cppm     # Config parser &amp; path resolver
    │   ├── ipc.cppm      # UNIX domain socket IPC bridge
    │   ├── menu.cppm     # Interactive TUI scene selector
    │   ├── render.cppm   # Wayland fullscreen state &amp; engine spawn
    │   ├── server.cppm   # Background daemon event loop
    │   └── ui.cppm       # Terminal formatting &amp; status tables
    └── skeleton/ (bin, state, wallpapers/{photos,scenes,scenes_hidden,vids})</pre>
    </div>
    <div class="mica-panel mod-crimson copy-block">
      <span class="copy-badge">CLICK TO COPY</span>
      <h3 class="neon-title-crimson kinetic-text" data-theme="crimson">Installed System Runtime Tree</h3>
      <pre class="code-pre">/home/dev/zapret/.system/wectl/
├── bin/
│   └── assets/           # Pinned linux-wallpaperengine shaders &amp; libs
├── state/                # Active PID lock, UNIX socket &amp; last scene state
└── wallpapers/
    ├── photos/           # Static image wallpapers (v-0.2 target)
    ├── scenes/           # Active Workshop animated scenes
    ├── scenes_hidden/    # Quarantined / incompatible shader scenes
    └── vids/             # Hardware-decoded video loops (v-0.2 target)</pre>
    </div>
  </div>
</section>
)HTML";
    }
}
