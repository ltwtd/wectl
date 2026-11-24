export module wectl.web.css_reset;
import <string>;

export namespace wectl::web::css {
    std::string get_reset() {
        return R"CSS(
:root {
  --bg: #030306;
  --crimson: #ff1e42;
  --violet: #b829ff;
  --cyan: #00f0ff;
  --text: #f4f4fa;
  --muted: #8b8b9e;
  --mono: 'JetBrains Mono', 'Fira Code', monospace;
  --sans: 'Inter', system-ui, -apple-system, sans-serif;
}
* { box-sizing: border-box; margin: 0; padding: 0; }
html, body {
  background: var(--bg);
  color: var(--text);
  font-family: var(--sans);
  overflow-x: hidden;
  scrollbar-width: none;
}
body::-webkit-scrollbar { display: none; }
#gl-canvas, #fx-canvas {
  position: fixed;
  inset: 0;
  width: 100vw;
  height: 100vh;
  pointer-events: none;
}
#gl-canvas { z-index: 0; }
#fx-canvas { z-index: 1; mix-blend-mode: screen; }
#smooth-wrapper {
  position: fixed;
  inset: 0;
  overflow: hidden;
  z-index: 2;
}
#smooth-content {
  will-change: transform;
  transform-origin: center top;
  padding-bottom: 160px;
}
)CSS";
    }
}
