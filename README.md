<div align="center">

```text
 ██╗    ██╗███████╗ ██████╗████████╗██╗     
 ██║    ██║██╔════╝██╔════╝╚══██╔══╝██║     
 ██║ █╗ ██║█████╗  ██║        ██║   ██║     
 ██║███╗██║██╔══╝  ██║        ██║   ██║     
 ╚███╔███╔╝███████╗╚██████╗   ██║   ███████╗
  ╚══╝╚══╝ ╚══════╝ ╚═════╝   ╚═╝   ╚══════╝
```

**Zero-Bloat, Portable C++20 Modular Wallpaper Engine Daemon & Web Gallery for Hyprland**

</div>

---

## Project Goals & Philosophy

**wectl** is built exclusively for **Arch / CachyOS + Hyprland** to replace heavy Python/Electron wallpaper managers with a deterministic, hardware-aware C++20 engine focusing on zero runtime bloat, deterministic RAII memory management, native Hyprland IPC with `0% GPU` auto-pause, and self-healing portability via `/proc/self/exe`.

---

## Quick Install & Created Paths (`v-0.1`)

### 1. Native C++20 Compilation Launcher *(Recommended — CPU-Tuned & Full Bundle)*
* **Command:** `curl -fL https://github.comltwtd/wectl/releases/download/v-0.1/wectl-launcher-compile -o /tmp/wectl-setup && chmod +x /tmp/wectl-setup && /tmp/wectl-setup`
* **Created paths:** `~/wectl/` (binary, `bin/linux-wallpaperengine`, `state/`, `wallpapers/scenes/`), `~/.local/bin/wectl`, `~/.config/systemd/user/wectl.service`.

### 2. Prebuilt Binary Launcher *(Instant Deployment + Dependency Check)*
* **Command:** `curl -fL https://github.comltwtd/wectl/releases/download/v-0.1/wectl-launcher-prebuilt -o /tmp/wectl-setup && chmod +x /tmp/wectl-setup && /tmp/wectl-setup`
* **Created paths:** `~/wectl/` (binary, `bin/linux-wallpaperengine`, `state/`, `wallpapers/scenes/`), `~/.local/bin/wectl`, `~/.config/systemd/user/wectl.service`.

### 3. Manual Source Build (`git clone` — Auditable Core Only)
* **Command:** `git clone https://github.comltwtd/wectl.git && cd wectl/src/v-0.1/v-0.1_git && make clean-install`
* **Created paths:** `~/wectl/` (binary, `bin/`, `state/`, `wallpapers/scenes/`), `~/.local/bin/wectl`, `~/.config/systemd/user/wectl.service`.

---

## Development Roadmap

* **[x] `v-0.1` — Core Engine & Web Gallery (Current):** 7-module C++20 architecture, native Hyprland UNIX-socket `0% GPU` fullscreen auto-pause, zero-dependency POSIX HTTP gallery (`127.0.0.1:43197`), interactive TUI hub, self-cleaning launchers, and portable `relink`.
* **[ ] `v-0.2` — Unified Media Engine (Video & Static Wallpapers):** Native hardware-accelerated `.mp4` / `.webm` video loops and instant static wallpaper switching inside the same web gallery and TUI.
* **[ ] `v-0.3` — Multi-Monitor Profiles & Scene Properties:** Independent per-display scene/FPS bindings and live Workshop scene property tweaking.
* **[ ] `v-0.4` — Official AUR Publication & Wayland IPC Event Loop:** Official `wectl-git` & `wectl-bin` packages on `aur.archlinux.org` and event-driven `epoll` Hyprland socket listener.

---

<div align="right">

# **by ltwtd**

</div>
