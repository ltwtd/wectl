export module wectl.web.js_char;
import <string>;

export namespace wectl::web::js {
    std::string get_char_physics() {
        return R"JS(
document.querySelectorAll('.kinetic-text').forEach(el => {
  const theme = el.dataset.theme || 'crimson';
  const text = el.textContent;
  el.textContent = '';
  [...text].forEach(ch => {
    const span = document.createElement('span');
    span.className = 'k-char';
    span.textContent = ch === ' ' ? '\u00A0' : ch;
    span.dataset.theme = theme;
    el.appendChild(span);
  });
});

const chars = Array.from(document.querySelectorAll('.k-char'));
let mouseX = -9999, mouseY = -9999;

window.addEventListener('mousemove', e => {
  mouseX = e.clientX;
  mouseY = e.clientY;
});

function animateChars() {
  const radius = 95;
  for (let i = 0; i < chars.length; i++) {
    const c = chars[i];
    const rect = c.getBoundingClientRect();
    if (rect.bottom < -50 || rect.top > window.innerHeight + 50) continue;
    const cx = rect.left + rect.width * 0.5;
    const cy = rect.top + rect.height * 0.5;
    const dx = cx - mouseX;
    const dy = cy - mouseY;
    const dist = Math.hypot(dx, dy);

    if (dist < radius) {
      const power = (1 - dist / radius);
      const tx = (dx / (dist || 1)) * power * 10;
      const ty = (dy / (dist || 1)) * power * 12 - power * 4;
      const rot = (dx / radius) * power * 18;
      c.style.transform = `translate3d(${tx}px, ${ty}px, 0) rotate(${rot}deg) scale(${1 + power * 0.22})`;
      c.classList.add('active-' + c.dataset.theme);
    } else if (c.style.transform) {
      c.style.transform = '';
      c.classList.remove('active-crimson', 'active-violet', 'active-cyan');
    }
  }
  requestAnimationFrame(animateChars);
}
animateChars();
)JS";
    }
}
