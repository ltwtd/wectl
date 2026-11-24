export module wectl.web.css_neon;
import <string>;

export namespace wectl::web::css {
    std::string get_neon_text() {
        return R"CSS(
.kinetic-text { display: inline-block; cursor: default; }
.k-char {
  display: inline-block;
  transition: transform 0.22s cubic-bezier(0.2, 0.9, 0.3, 1.4), color 0.2s ease, text-shadow 0.2s ease;
  will-change: transform;
}
.k-char.active-crimson {
  color: #fff;
  text-shadow: 0 0 8px #ff1e42, 0 0 20px #ff1e42, -2px 0 4px #00f0ff;
}
.k-char.active-violet {
  color: #fff;
  text-shadow: 0 0 8px #b829ff, 0 0 20px #b829ff, 2px 0 4px #ff1e42;
}
.k-char.active-cyan {
  color: #fff;
  text-shadow: 0 0 8px #00f0ff, 0 0 20px #00f0ff, -2px 0 4px #b829ff;
}
.neon-title-crimson { text-shadow: 0 0 24px rgba(255, 30, 66, 0.55); color: #ff4766; }
.neon-title-violet  { text-shadow: 0 0 24px rgba(184, 41, 255, 0.55); color: #cc66ff; }
.neon-title-cyan    { text-shadow: 0 0 24px rgba(0, 240, 255, 0.55); color: #4df4ff; }
)CSS";
    }
}
