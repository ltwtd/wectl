export module wectl.web.js_kinetics;
import <string>;

export namespace wectl::web::js {
    std::string get_card_kinetics() {
        return R"JS(
document.querySelectorAll('.mica-panel').forEach(card => {
  card.style.transition = 'transform 0.35s cubic-bezier(0.16, 1, 0.3, 1), border-color 0.3s ease, box-shadow 0.3s ease';
  card.addEventListener('mousemove', e => {
    const r = card.getBoundingClientRect();
    const x = e.clientX - r.left;
    const y = e.clientY - r.top;
    card.style.setProperty('--mx', x + 'px');
    card.style.setProperty('--my', y + 'px');
    const rx = ((y - r.height * 0.5) / (r.height * 0.5)) * -5.5;
    const ry = ((x - r.width * 0.5) / (r.width * 0.5)) * 5.5;
    card.style.transform = `perspective(900px) rotateX(${rx}deg) rotateY(${ry}deg) translateY(-6px) scale(1.015)`;
  });
  card.addEventListener('mouseleave', () => {
    card.style.transform = '';
  });
});

document.querySelectorAll('.spec-list li').forEach(li => {
  li.style.transition = 'transform 0.25s cubic-bezier(0.2, 0.9, 0.3, 1.3), color 0.2s ease, border-left-color 0.2s ease';
  li.addEventListener('mouseenter', () => {
    li.style.transform = 'translateX(10px) scale(1.01)';
    li.style.color = '#ffffff';
    li.style.borderLeftColor = '#00f0ff';
  });
  li.addEventListener('mouseleave', () => {
    li.style.transform = '';
    li.style.color = '';
    li.style.borderLeftColor = '';
  });
});

const secObserver = new IntersectionObserver(entries => {
  entries.forEach(entry => {
    if (entry.isIntersecting) {
      entry.target.style.opacity = '1';
      entry.target.style.transform = 'translateY(0) scale(1)';
    }
  });
}, { threshold: 0.08 });

document.querySelectorAll('.section').forEach(sec => {
  sec.style.opacity = '0';
  sec.style.transform = 'translateY(45px) scale(0.97)';
  sec.style.transition = 'opacity 0.8s cubic-bezier(0.16, 1, 0.3, 1), transform 0.8s cubic-bezier(0.16, 1, 0.3, 1)';
  secObserver.observe(sec);
});
)JS";
    }
}
