<div align="center">

```text
 ██╗    ██╗███████╗ ██████╗████████╗██╗     
 ██║    ██║██╔════╝██╔════╝╚══██╔══╝██║     
 ██║ █╗ ██║█████╗  ██║        ██║   ██║     
 ██║███╗██║██╔══╝  ██║        ██║   ██║     
 ╚███╔███╔╝███████╗╚██████╗   ██║   ███████╗
  ╚══╝╚══╝ ╚══════╝ ╚═════╝   ╚═╝   ╚══════╝
```

**Zero-Bloat, Portable C++20 Modular Wallpaper Engine Client & Web Gallery for Hyprland**

[![Language](https://shields.ioC%2B%2B-20-00599C?style=for-the-badge&logo=cplusplus)](https://isocpp.org)
[![Target](https://shields.ioWayland-Hyprland-58E1FF?style=for-the-badge&logo=wayland)](https://hyprland.org)
[![Binary Size](https://shields.ioBinary-~100_KB-00FF64?style=for-the-badge)](#architecture)
[![Idle GPU](https://shields.ioIdle_GPU-0%25-FF0033?style=for-the-badge)](#features)
[![Release](https://shields.ioRelease-v--0.1-780019?style=for-the-badge)](https://github.com/ltwtd/wectl/releases/tag/v-0.1)
[![Assets](https://shields.ioAssets-v--0.1-140005?style=for-the-badge)](https://github.com/ltwtd/wectl/releases/tag/assets-v.0.1)

</div>

---

## Philosophy

**wectl** is built exclusively for **Arch / CachyOS + Hyprland** with a strict **zero-bloat, zero-overhead** design:
- **No Python, Node.js, Electron, or shell wrappers at runtime.**
- **Single ~115 KB stripped C++20 binary** compiled with `-std=c++20 -fmodules-ts -O3 -march=native -flto -fno-rtti`.
- **RAII tight-scope memory management:** temporary buffers and HTML streams are freed immediately after each HTTP/IPC transaction, keeping idle daemon RSS under **~3 MB**.
- **Fully Portable Directory (`~/wectl`):** move the folder anywhere and run `./wectl relink` (or simply launch `./wectl`) to automatically re-bind symlinks and `systemd --user`.

---

## Quick Install (`v-0.1`)

All installers run silently with **zero prompts**, auto-install missing system packages, deploy the headless render engine + bundled **4K SCI-FI Black Hole** scene, register `wectl.service`, and **self-delete** all temporary source/build artifacts.

### 1. Native C++20 Compilation Launcher *(Recommended — CPU Optimized)*
Compiles all 7 C++20 modules natively on your CPU (`-march=native -flto`) into `~/wectl`:
```bash
curl -fL https://github.com/ltwtd/wectl/releases/download/v-0.1/wectl-launcher-compile -o /tmp/wectl-setup && chmod +x /tmp/wectl-setup && /tmp/wectl-setup
```

### 2. Prebuilt Binary Launcher *(Instant Deployment)*
Deploys the precompiled `x86_64` binary + dependencies into `~/wectl`:
```bash
curl -fL https://github.com/ltwtd/wectl/releases/download/v-0.1/wectl-launcher-prebuilt -o /tmp/wectl-setup && chmod +x /tmp/wectl-setup && /tmp/wectl-setup
```

### 3. Manual Build (`git clone`)
```bash
git clone https://github.com/ltwtd/wectl.git && cd wectl/src/v-0.1/v-0.1_git && make clean-install
```

---

## Dependencies & Asset Bundle (`assets-v.0.1`)

Pre-verified packages and binaries hosted in [`assets-v.0.1`](https://github.com/ltwtd/wectl/releases/tag/assets-v.0.1) (automatically resolved and installed only if missing):

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
v-0.1/
├── Makefile                    # Zero-dependency C++20 module build & clean-install rules
└── src/
    ├── main.cpp                # CLI dispatcher & automatic path self-healing
    ├── launcher.cpp            # Self-cleaning native C++20 installer (compile & prebuilt)
    └── modules/
        ├── banner.cppm         # ANSI WECTL logo & console telemetry header
        ├── core.cppm           # Portable root resolution (/proc/self/exe), hardware.env cache, relink
        ├── ipc.cppm            # Direct UNIX-socket client (.socket.sock / .socket2.sock)
        ├── render.cppm         # Process lifecycle, instant killall Stop & SIGSTOP 0% GPU worker
        ├── ui.cppm             # Minimalist HTML/CSS/JS gallery with player-style FPS slider
        ├── server.cppm         # Zero-dependency POSIX HTTP server (127.0.0.1:43197)
        └── menu.cppm           # Interactive clear-screen TUI hub with modular taskbars
```

---

## CLI & Interactive TUI Usage

Running `wectl` without arguments clears the terminal and opens the **interactive TUI hub** with two focused module taskbars (**App Control** and **Config & Scenes**). Direct CLI commands are also available:

| Command | Action |
| :--- | :--- |
| `wectl` | Open interactive clear-screen TUI hub |
| `wectl up` | Start `wectl.service` and open web gallery (`http://127.0.0.1:43197`) |
| `wectl status` | Display portable root, cached monitor profile, active scene, FPS, and renderer PID |
| `wectl stop` | Immediately terminate renderer (**0% GPU / 0W**) while keeping daemon idle |
| `wectl down` | Stop both wallpaper renderer and background daemon |
| `wectl restart` | Restart `wectl.service` |
| `wectl restore` | Restore last active scene |
| `wectl relink` | Re-bind `~/.local/bin/wectl` and `wectl.service` after moving the `wectl` folder |
| `wectl uninstall` | Completely remove `wectl`, systemd unit, symlinks, and directory |

---

## Roadmap

- [x] **v-0.1**: 7-module C++20 core, native Hyprland UNIX-socket 0% GPU auto-pause, minimalist player-style web gallery, interactive TUI hub, and portable `relink`.
- [ ] **v-0.2**: Native support for local video loops (`wallpapers/vids/`) and static wallpapers (`wallpapers/photos/`) inside the same zero-overhead gallery.
- [ ] **v-0.3**: Expanded built-in curated wallpaper pack & multi-monitor independent scene profiles.
