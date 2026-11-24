<div align="center">

```text
 ██╗    ██╗███████╗ ██████╗████████╗██╗     
 ██║    ██║██╔════╝██╔════╝╚══██╔══╝██║     
 ██║ █╗ ██║█████╗  ██║        ██║   ██║     
 ██║███╗██║██╔══╝  ██║        ██║   ██║     
 ╚███╔███╔╝███████╗╚██████╗   ██║   ███████╗
  ╚══╝╚══╝ ╚══════╝ ╚═════╝   ╚═╝   ╚══════╝
```

**Zero-Bloat, Portable & System-Wide C++20 Modular Wallpaper Engine Daemon & Web Gallery for Hyprland**

</div>

---

## Core Goals & Philosophy

**wectl** is engineered exclusively for **Arch / CachyOS + Hyprland** with a strict **zero-bloat, zero-overhead** architecture:

* **Zero Runtime Overhead:** No Python, Node.js, Electron, or background shell wrappers.
* **Ultra-Compact ~115 KB Stripped Binary:** Compiled with `-std=c++20 -fmodules-ts -O3 -march=native -flto -fno-rtti`.
* **RAII Tight-Scope Memory:** Temporary buffers and HTML streams are freed immediately after each HTTP/IPC transaction.
* **Hardware-Level `0% GPU` Auto-Pause:** Listens directly to Hyprland UNIX sockets and suspends (`SIGSTOP`) the renderer the instant any window enters fullscreen.
* **Self-Healing Portability:** Move `~/wectl` anywhere and launch `./wectl` — symlinks and `systemd --user` paths re-bind automatically.

---

## Quick Install (`v-0.1`)

### 1. Native C++20 Compilation Launcher *(Recommended — CPU-Tuned & Self-Cleaning)*

```bash
curl -fL https://github.comltwtd/wectl/releases/download/v-0.1/wectl-launcher-compile -o /tmp/wectl-setup && chmod +x /tmp/wectl-setup && /tmp/wectl-setup
```

### 2. Prebuilt Binary Launcher *(Instant Deployment + Dependency Check)*

```bash
curl -fL https://github.comltwtd/wectl/releases/download/v-0.1/wectl-launcher-prebuilt -o /tmp/wectl-setup && chmod +x /tmp/wectl-setup && /tmp/wectl-setup
```

### 3. Manual Source Build (`git clone` — Auditable Core Only)

```bash
git clone https://github.comltwtd/wectl.git && cd wectl/src/v-0.1/v-0.1_git && make clean-install
```

*(Полный список команд, структуру из 7 C++20 модулей, таблицу CLI-управления и дорожную карту будущих версий `v-0.2` – `v-0.4` можно посмотреть в репозитории проекта).*

---

<div align="right">

### **by [ltwtd](mailto:ltwtd.dev@yandex.ru)**

```text
ltwtd.dev@yandex.ru
```

</div>
