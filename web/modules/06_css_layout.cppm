export module wectl.web.css_layout;
import <string>;

export namespace wectl::web::css {
    std::string get_layout() {
        return R"CSS(
.hud-nav {
  position: fixed;
  top: 16px;
  left: 50%;
  transform: translateX(-50%);
  width: min(1240px, 94vw);
  z-index: 100;
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 0.8rem 1.5rem;
  border-radius: 14px;
  background: rgba(8, 8, 12, 0.65);
  backdrop-filter: blur(26px) saturate(180%);
  border: 1px solid rgba(255, 255, 255, 0.1);
}
.hud-brand { font-family: var(--mono); font-weight: 800; font-size: 1.15rem; letter-spacing: 0.06em; }
.hud-controls { display: flex; gap: 0.6rem; }
.mode-btn {
  font-family: var(--mono);
  font-size: 0.75rem;
  padding: 0.4rem 0.85rem;
  border-radius: 8px;
  border: 1px solid rgba(255,255,255,0.14);
  background: rgba(255,255,255,0.04);
  color: var(--text);
  cursor: pointer;
  transition: all 0.25s ease;
}
.mode-btn:hover { transform: scale(1.06); }
.mode-btn[data-mode="0"].active { border-color: var(--crimson); box-shadow: 0 0 15px rgba(255,30,66,0.45); color: #ff6680; }
.mode-btn[data-mode="1"].active { border-color: var(--violet);  box-shadow: 0 0 15px rgba(184,41,255,0.45); color: #d980ff; }
.mode-btn[data-mode="2"].active { border-color: var(--cyan);    box-shadow: 0 0 15px rgba(0,240,255,0.45); color: #80f8ff; }

.container { max-width: 1240px; margin: 0 auto; padding: 0 1.5rem; }
.hero { min-height: 92vh; display: flex; flex-direction: column; justify-content: center; padding-top: 90px; }
.hero h1 { font-size: clamp(2.8rem, 6.5vw, 5.4rem); font-weight: 900; line-height: 1.05; letter-spacing: -0.03em; margin-bottom: 1.2rem; }
.hero p { max-width: 720px; font-size: 1.12rem; color: var(--muted); line-height: 1.7; margin-bottom: 2rem; }
.section { margin-top: 6rem; }
.sec-tag { font-family: var(--mono); font-size: 0.82rem; letter-spacing: 0.16em; text-transform: uppercase; margin-bottom: 0.4rem; display: block; }
.sec-title { font-size: 2.2rem; font-weight: 800; margin-bottom: 2rem; }
.grid-2 { display: grid; grid-template-columns: repeat(auto-fit, minmax(460px, 1fr)); gap: 1.5rem; }
.grid-3 { display: grid; grid-template-columns: repeat(auto-fit, minmax(320px, 1fr)); gap: 1.5rem; }
@media (max-width: 540px) { .grid-2 { grid-template-columns: 1fr; } }
.spec-list { list-style: none; display: flex; flex-direction: column; gap: 0.85rem; margin-top: 1rem; }
.spec-list li { color: #d2d2e0; font-size: 0.94rem; line-height: 1.55; padding-left: 1rem; border-left: 2px solid rgba(255,255,255,0.16); }
)CSS";
    }
}
