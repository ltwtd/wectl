export module wectl.web.css_mica;
import <string>;

export namespace wectl::web::css {
    std::string get_mica() {
        return R"CSS(
.mica-panel {
  background: linear-gradient(135deg, rgba(16, 16, 24, 0.58) 0%, rgba(8, 8, 13, 0.72) 100%);
  backdrop-filter: blur(34px) saturate(210%) contrast(108%);
  -webkit-backdrop-filter: blur(34px) saturate(210%) contrast(108%);
  border: 1px solid rgba(255, 255, 255, 0.09);
  box-shadow: 0 24px 60px rgba(0, 0, 0, 0.75), inset 0 1px 0 rgba(255, 255, 255, 0.14);
  border-radius: 18px;
  position: relative;
  overflow: hidden;
}
.mica-panel::after {
  content: '';
  position: absolute;
  inset: 0;
  background: radial-gradient(600px circle at var(--mx, 50%) var(--my, 50%), var(--glow-color, rgba(255,30,66,0.14)), transparent 45%);
  pointer-events: none;
  z-index: 1;
}
.mod-crimson { --glow-color: rgba(255, 30, 66, 0.18); border-color: rgba(255, 30, 66, 0.28); }
.mod-violet  { --glow-color: rgba(184, 41, 255, 0.18); border-color: rgba(184, 41, 255, 0.28); }
.mod-cyan    { --glow-color: rgba(0, 240, 255, 0.18); border-color: rgba(0, 240, 255, 0.28); }
)CSS";
    }
}
