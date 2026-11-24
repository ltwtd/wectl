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

[![Language](https://shields.io/C%2B%2B-20-00599C?style=for-the-badge&logo=cplusplus)](https://isocpp.org)
[![Target](https://shields.io/Wayland-Hyprland-58E1FF?style=for-the-badge&logo=wayland)](https://hyprland.org)
[![Binary Size](https://shields.io/Binary-~115_KB-00FF64?style=for-the-badge)](#architecture-7-c20-modules)
[![Idle GPU](https://shields.io/Idle_GPU-0%25-FF0033?style=for-the-badge)](#philosophy)
[![Release](https://shields.io/Release-v--0.1-780019?style=for-the-badge)](https://github.com/ltwtd/wectl/releases/tag/v-0.1)
[![Assets](https://shields.io/Assets-v--0.1-140005?style=for-the-badge)](https://github.com/ltwtd/wectl/releases/tag/assets-v.0.1)

</div>

---

## Philosophy

**wectl** is engineered exclusively for **Arch / CachyOS + Hyprland** with a strict **zero-bloat, zero-overhead** architecture:

* **Zero Runtime Overhead:** No Python, Node.js, Electron, or background shell loops.
* **Ultra-Compact ~115 KB Stripped Binary:** Built with `-std=c++20 -fmodules-ts -O3 -march=native -flto -fno-rtti`.
* **RAII Tight-Scope Memory:** All HTTP buffers and HTML streams are freed immediately after each transaction, keeping idle daemon RSS under **~3 MB**.
* **Hardware-Level `0% GPU` Auto-Pause:** Listens directly to Hyprland UNIX sockets and suspends (`SIGSTOP`) the renderer the instant any window enters fullscreen.
* **Dual Deployment Model:** Run as a self-contained portable folder (`~/wectl`) with automatic `relink`, or install natively via `pacman` from the `aur` branch.

---

## Installation Matrix (`v-0.1`)

Choose the deployment method that best fits your workflow:

| Method | Target Location | Data & Wallpapers | CPU Optimization | Bundles Engine + 4K Scene |
| :--- | :--- | :--- | :--- | :--- |
| **1. Compilation Launcher** | `~/wectl/wectl` | `~/wectl/wallpapers/` | Native (`-march=native`) | **Yes** (Full auto-deploy) |
| **2. Prebuilt Launcher** | `~/wectl/wectl` | `~/wectl/wallpapers/` | Generic `x86_64` (`-O3`) | **Yes** (Instant deploy) |
| **3. System Package (`aur`)** | `/usr/bin/wectl` | `~/.local/share/wectl/` | Local `/etc/makepkg.conf` | Optional (`optdepends`) |
| **4. Manual Source Build** | `~/wectl/wectl` | `~/wectl/wallpapers/` | Native (`-march=native`) | **No** (Core binary only) |

---

## Quick Install & What Each Installer Does

### 1. Native C++20 Compilation Launcher *(Recommended — Portable & CPU-Tuned)*

```bash
curl -fL https://github.com/ltwtd/wectl/releases/download/v-0.1/wectl-launcher-compile -o /tmp/wectl-setup && chmod +x /tmp/wectl-setup && /tmp/wectl-setup
```

* **What it does under the hood:** Fetches the verified release manifest, checks and installs missing system dependencies via `pacman`, creates the portable directory tree in `~/wectl`, downloads and extracts the headless `linux-wallpaperengine` (No CEF) renderer into `~/wectl/bin/` along with the default **4K SCI-FI Black Hole** scene into `~/wectl/wallpapers/scenes/`, compiles all 7 C++20 modules natively for your CPU (`-march=native -flto`), caches your monitor resolution/refresh rate, registers `wectl.service` in `systemd --user`, and wipes all temporary files.

### 2. Prebuilt Binary Launcher *(Instant Portable Deployment)*

```bash
curl -fL https://github.com/ltwtd/wectl/releases/download/v-0.1/wectl-launcher-prebuilt -o /tmp/wectl-setup && chmod +x /tmp/wectl-setup && /tmp/wectl-setup
```

* **What it does under the hood:** Performs the exact same full-stack deployment into `~/wectl` (dependencies, headless renderer, 4K scene, monitor cache, and `systemd --user` service), but skips C++ compilation by deploying the pre-compiled `x86_64` binary directly from GitHub Releases in seconds.

### 3. Arch Linux / CachyOS System Package (`aur` branch)

```bash
git clone -b aur --single-branch https://github.com/ltwtd/wectl.git /tmp/wectl-aur && cd /tmp/wectl-aur && makepkg -si && rm -rf /tmp/wectl-aur
systemctl --user enable --now wectl.service
```

* **What it does under the hood:** Clones the dedicated `aur` branch (`PKGBUILD` + `.SRCINFO`), compiles the system variant from `src/v-0.1/v-.0.1_aur` using your local `/etc/makepkg.conf` flags, and installs `/usr/bin/wectl` + `/usr/lib/systemd/user/wectl.service` cleanly via `pacman`. User data and scenes are automatically initialized in `~/.local/share/wectl/` on first launch.

### 4. Manual Build (`git clone` — Core Binary Only)

```bash
git clone https://github.com/ltwtd/wectl.git && cd wectl/src/v-0.1/v-0.1_git && make clean-install
```

* **What it does under the hood:** Compiles the 7 C++20 modules directly from source, initializes the portable skeleton in `~/wectl`, binds `~/.local/bin/wectl` and `wectl.service`, and removes the cloned source directory. *(Note: Does not download binary assets automatically — place `linux-wallpaperengine` into `~/wectl/bin/` or install `linux-wallpaperengine-git` via AUR).*

---

## Dependencies & Asset Bundle (`assets-v.0.1`)

Pre-verified packages and binaries hosted in [`assets-v.0.1`](https://github.com/ltwtd/wectl/releases/tag/assets-v.0.1) (resolved automatically by the launchers):

| Component / Package | Version / Build | Role |
| :--- | :--- | :--- |
| **`linux-wallpaperengine`** | `r643.a0ddce58-1` *(No CEF)* | Headless Wayland layer-shell scene & shader renderer |
| **`4K SCI-FI Black Hole`** | `4K Scene Bundle` | Default bundled live wallpaper |
| **`gcc`** | `16.2.1+r23` | C++20 Modules & LTO compiler |
| **`ffmpeg`** | `2:9.1-1.1` | Video texture & audio stream decoding |
| **`mpv`** | `1:0.42.0-3.1` | Hardware-accelerated media backend |
| **`libplacebo`** | `7.387.0-1.1` | GPU tone-mapping & Vulkan/OpenGL shader pipeline |
| **`sdl2-compat`** | `2.32.74-1.1` | Window/context & input abstraction |
| **`glew`** | `2.3.1-1.1` | OpenGL extension loading |
| **`glm`** | `1.0.3-1.1` | Vector/matrix math for 3D scenes |
| **`libjxl`** | `0.13.0-1.1` | JPEG XL texture decoding |
| **`lz4`** | `1:1.11.1-2` | Fast PKG/TEX asset decompression |
| **`wayland`** | `1.26.0-1.1` | Native Wayland compositor protocol |
| **`sndio`** | `1.11.0-1` | Lightweight audio subsystem |
| **`procps-ng` / `xdg-utils`** | `4.0.7` / `1.2.1` | Process signaling & browser launch |

---

## Architecture (7 C++20 Modules)

```text
src/v-0.1/
├── v-0.1_git/                  # Portable build (~/wectl) with self-healing relink
│   ├── Makefile                # Zero-dependency C++20 module build & clean-install rules
│   ├── main.cpp                # CLI dispatcher & automatic path self-healing
│   ├── launcher.cpp            # Self-cleaning native C++20 installer (compile & prebuilt)
│   └── modules/
│       ├── banner.cppm         # ANSI WECTL logo & console telemetry header
│       ├── core.cppm           # Portable root resolution (/proc/self/exe), hardware.env cache, relink
│       ├── ipc.cppm            # Direct UNIX-socket client (.socket.sock / .socket2.sock)
│       ├── render.cppm         # Process lifecycle, instant Stop & SIGSTOP 0% GPU worker
│       ├── ui.cppm             # Minimalist HTML/CSS/JS gallery with player-style FPS slider
│       ├── server.cppm         # Zero-dependency POSIX HTTP server (127.0.0.1:43197)
│       └── menu.cppm           # Interactive clear-screen TUI hub with modular taskbars
└── v-.0.1_aur/                 # System-wide Arch Linux / PKGBUILD target (/usr/bin/wectl)
```

---

## CLI & Interactive TUI Usage

Running `wectl` with no arguments clears the terminal and opens the **interactive TUI hub** with two modular taskbars (**App Control** and **Config & Scenes**). Direct CLI commands are also supported:

| Command | Action |
| :--- | :--- |
| `wectl` | Open interactive clear-screen TUI hub |
| `wectl up` | Start `wectl.service` and open web gallery (`http://127.0.0.1:43197`) |
| `wectl status` | Display root path, cached monitor profile, active scene, FPS, and renderer PID |
| `wectl stop` | Immediately terminate renderer (**0% GPU / 0W**) while keeping daemon idle |
| `wectl down` | Stop both wallpaper renderer and background daemon |
| `wectl restart` | Restart `wectl.service` |
| `wectl restore` | Restore last active scene |
| `wectl relink` | Re-bind `~/.local/bin/wectl` and `wectl.service` after moving `~/wectl` *(Portable)* |
| `wectl uninstall` | Completely remove `wectl`, systemd unit, symlinks, and directory *(Portable)* |

---

## Roadmap

- [x] **v-0.1**: 7-module C++20 core, native Hyprland UNIX-socket `0% GPU` auto-pause, minimalist player-style web gallery, interactive TUI hub, portable `relink`, and `aur` branch packaging.
- [ ] **v-0.2**: Native support for local video loops (`wallpapers/vids/`) and static wallpapers (`wallpapers/photos/`) inside the same zero-overhead gallery.
- [ ] **v-0.3**: Expanded built-in curated wallpaper pack & multi-monitor independent scene profiles.

---

<div align="center">
  <sub><b>prod.ltwtd</b></sub>
</div>
