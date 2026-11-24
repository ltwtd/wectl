export module wectl.web.js_copy;
import <string>;

export namespace wectl::web::js {
    std::string get_block_copy() {
        return R"JS(
const toast = document.getElementById('toast-hud');
let toastTimer;

function showToast(msg) {
  toast.textContent = msg;
  toast.classList.add('show');
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => toast.classList.remove('show'), 2100);
}

document.querySelectorAll('.copy-block').forEach(block => {
  block.addEventListener('click', () => {
    const raw = block.dataset.copy || block.innerText.replace('CLICK TO COPY', '').trim();
    navigator.clipboard.writeText(raw).then(() => {
      const badge = block.querySelector('.copy-badge');
      if (badge) {
        const old = badge.textContent;
        badge.textContent = 'COPIED TO CLIPBOARD';
        setTimeout(() => badge.textContent = old, 1400);
      }
      showToast('>> COPIED BLOCK TO CLIPBOARD');
    });
  });
});
)JS";
    }
}
