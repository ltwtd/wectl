export module wectl.web.css_cards;
import <string>;

export namespace wectl::web::css {
    std::string get_cards() {
        return R"CSS(
.copy-block {
  cursor: pointer;
  padding: 1.6rem;
  transition: transform 0.28s cubic-bezier(0.16, 1, 0.3, 1), border-color 0.25s ease, box-shadow 0.25s ease;
  user-select: none;
}
.copy-block:hover { transform: translateY(-4px) scale(1.008); }
.copy-block:active { transform: scale(0.99); }
.copy-badge {
  position: absolute;
  top: 14px;
  right: 16px;
  font-family: var(--mono);
  font-size: 0.7rem;
  letter-spacing: 0.1em;
  padding: 3px 8px;
  border-radius: 6px;
  background: rgba(255,255,255,0.06);
  color: var(--muted);
  border: 1px solid rgba(255,255,255,0.1);
  transition: all 0.2s ease;
  z-index: 3;
}
.copy-block:hover .copy-badge { color: #fff; border-color: rgba(255,255,255,0.35); }
.code-pre {
  font-family: var(--mono);
  font-size: 0.86rem;
  line-height: 1.65;
  color: #e6e6f2;
  background: rgba(2, 2, 5, 0.65);
  border: 1px solid rgba(255,255,255,0.07);
  border-radius: 10px;
  padding: 1rem 1.2rem;
  margin-top: 0.9rem;
  overflow-x: auto;
  white-space: pre;
}
#toast-hud {
  position: fixed;
  bottom: 28px;
  right: 28px;
  z-index: 999;
  padding: 12px 22px;
  font-family: var(--mono);
  font-size: 0.85rem;
  color: #fff;
  background: rgba(10, 10, 16, 0.85);
  backdrop-filter: blur(20px);
  border: 1px solid var(--cyan);
  box-shadow: 0 0 25px rgba(0, 240, 255, 0.4);
  border-radius: 10px;
  opacity: 0;
  transform: translateY(20px);
  pointer-events: none;
  transition: all 0.3s cubic-bezier(0.16, 1, 0.3, 1);
}
#toast-hud.show { opacity: 1; transform: translateY(0); }
)CSS";
    }
}
