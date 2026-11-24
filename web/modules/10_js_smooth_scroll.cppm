export module wectl.web.js_scroll;
import <string>;

export namespace wectl::web::js {
    std::string get_smooth_scroll() {
        return R"JS(
const content = document.getElementById('smooth-content');
let currentScroll = 0;
let targetScroll = 0;
let velocity = 0;
const stiffness = 0.038;
const damping = 0.82;

function syncHeight() {
  document.body.style.height = content.getBoundingClientRect().height + 'px';
}
window.addEventListener('resize', syncHeight);
new ResizeObserver(syncHeight).observe(content);

window.addEventListener('scroll', () => {
  targetScroll = window.scrollY;
}, { passive: true });

function renderSmoothScroll() {
  const force = (targetScroll - currentScroll) * stiffness;
  velocity = (velocity + force) * damping;
  currentScroll += velocity;

  if (Math.abs(targetScroll - currentScroll) < 0.05 && Math.abs(velocity) < 0.05) {
    currentScroll = targetScroll;
    velocity = 0;
  }

  const skew = Math.max(-2.2, Math.min(2.2, velocity * 0.045));
  const scale = 1 - Math.min(0.018, Math.abs(velocity) * 0.00035);
  content.style.transform = `translate3d(0, ${-currentScroll}px, 0) skewY(${skew}deg) scale(${scale})`;
  window.__WECTL_SCROLL = currentScroll;
  window.__WECTL_VEL = velocity;
  requestAnimationFrame(renderSmoothScroll);
}
renderSmoothScroll();
)JS";
    }
}
