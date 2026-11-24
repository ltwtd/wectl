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

## Philosophy

**wectl** is built exclusively for **Arch / CachyOS + Hyprland** with zero runtime overhead:
* **No Python, Node.js, Electron, or shell loops** at runtime.
* **~115 KB stripped C++20 binary** (`-std=c++20 -fmodules-ts -O3 -march=native -flto -fno-rtti`).
* **RAII tight-scope memory:** idle daemon RSS stays under **~3 MB**.
* **`0% GPU` Fullscreen Auto-Pause:** listens directly to Hyprland UNIX sockets and suspends (`SIGSTOP`) the renderer when any window goes fullscreen.

---

## Quick Install (`v-0.1`)

* **1. Native C++20 Compilation Launcher** *(Recommended — Portable & CPU-Tuned)*: Downloads and executes the setup script to compile all 7 C++20 modules natively, deploying to `~/wectl/`, `~/.local/bin/wectl`, and `~/.config/systemd/user/wectl.service`.
* **2. Prebuilt Binary Launcher** *(Instant Portable Deployment)*: Extracts the prebuilt `x86_64` binary into `~/wectl/wectl` using the same directory structure.
* **3. Arch Linux / CachyOS System Package (`aur` branch)**: Clones the AUR branch, builds via `makepkg -si`, and installs system-wide to `/usr/bin/wectl` and `/usr/lib/systemd/user/wectl.service`.
* **4. Manual Build (`git clone` — Core Binary Only)**: Clones the repository to build only the C++20 modules from source without downloading binary assets.

*(Полный список команд установки, таблица зависимостей в релизе `assets-v.0.1`, описание 7 модулей архитектуры, CLI-команды и дорожная карта проекта доступны в официальном репозитории).*

---

<div align="center">
  <sub><b>prod.ltwtd</b></sub>
</div>

