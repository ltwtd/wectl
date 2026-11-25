# wectl

**wectl** is a lightweight, modular C++20 live wallpaper manager, daemon, and Web Gallery for **Hyprland / Wayland**. It orchestrates a custom ultra-optimized **NOCEF** fork of `linux-wallpaperengine` with zero-GPU fullscreen auto-pause, native multi-monitor layout synchronization, and seamless wallpaper switching.

## Contents

**`wectl`**<br>
├── [**1. Directory Layout**](#1-installed-directory-layout)<br>
├── [**2. v-0.2 (Latest) — Install**](#2-v-02-latest--installation)<br>
│&nbsp;&nbsp;&nbsp;&nbsp;├── [**2.1 Prebuilt Binary + Verified Deps**](#21-prebuilt-binary--pacman-dependencies--custom-engine)<br>
│&nbsp;&nbsp;&nbsp;&nbsp;├── [**2.2 Compile `wectl` + Verified Deps**](#22-compile-wectl-natively--pacman-dependencies--custom-engine)<br>
│&nbsp;&nbsp;&nbsp;&nbsp;└── [**2.3 Full Source Build (All Native)**](#23-full-source-build--engine-dependencies--wectl)<br>
├── [**3. Development Milestones**](#3-development-milestones)<br>
│&nbsp;&nbsp;&nbsp;&nbsp;├── [**3.1 `v-0.1` — C++20 Core & Zero-GPU**](#31-v-01--pure-c20-modular-client--zero-gpu-daemon)<br>
│&nbsp;&nbsp;&nbsp;&nbsp;└── [**3.2 `v-0.2` — Gallery & NOCEF Engine**](#32-v-02--multi-format-gallery-seamless-switch--ultra-optimized-engine)<br>
└── [**4. Previous Versions (`v-0.1`)**](#4-previous-versions)
---

## 1. Installed Directory Layout

All installation modes deploy the exact same self-contained directory layout into `~/wectl/` (symlinked to `~/.local/bin/wectl`):

```text
~/wectl/
├── wectl                        # C++20 modular CLI, TUI & Web Gallery daemon
├── bin/
│   ├── linux-wallpaperengine    # Custom ultra-optimized NOCEF OpenGL engine
│   └── assets/                  # Base Wallpaper Engine shaders, effects & materials
├── lib/                         # Isolated shared runtime libraries (RPATH \$ORIGIN/../lib)
├── wallpapers/
│   ├── scenes/                  # Steam Workshop & custom 2D/3D scenes
│   ├── vids/                    # Video wallpapers (MP4/WebM/MKV)
│   └── photos/                  # Static wallpapers (PNG/JPG/WebP/JXL)
└── state/                       # Runtime sockets, PID locks & active wallpaper state
```

---

## 2. v-0.2 (Latest) — Installation

> **Verified Dependency Matrix:** [`docs/COMPATIBILITY_v0.2.md`](https://github.com/ltwtd/wectl/blob/main/docs/COMPATIBILITY_v0.2.md)

### 2.1 Prebuilt Binary + `pacman` Dependencies + Custom Engine
Installs runtime libraries via `pacman`, downloads the prebuilt `wectl` binary (`v-0.2`), and deploys the custom ultra-optimized `linux-wallpaperengine` bundle (`01_lwe-engine-native-full.tar.zst`) from `assets`.
```bash
sudo pacman -S --needed --noconfirm ffmpeg mpv glew glm libjxl libplacebo lz4 sdl2-compat sndio wayland xdg-utils zstd curl && \
curl -fL https://github.com/ltwtd/wectl/releases/download/v-0.2/wectl-installer-bin -o /tmp/wectl-inst && \
chmod +x /tmp/wectl-inst && /tmp/wectl-inst && rm -f /tmp/wectl-inst
```

### 2.2 Compile `wectl` Natively + `pacman` Dependencies + Custom Engine
Installs runtime and build packages via `pacman`, compiles the modular C++20 `wectl` client locally for your CPU (`-O3 -march=native`), and deploys the prebuilt custom `linux-wallpaperengine` bundle from `assets`.
```bash
sudo pacman -S --needed --noconfirm base-devel gcc cmake ninja ffmpeg mpv glew glm libjxl libplacebo lz4 sdl2-compat sndio wayland xdg-utils zstd curl && \
curl -fL https://github.com/ltwtd/wectl/releases/download/v-0.2/wectl-installer-src -o /tmp/wectl-inst && \
chmod +x /tmp/wectl-inst && /tmp/wectl-inst && rm -f /tmp/wectl-inst
```

### 2.3 Full Source Build — Engine, Dependencies & `wectl`
Clones the complete patched source tree (`engine/linux-wallpaperengine/` + `wectl/`) and compiles the entire stack (`-O3 -march=native -flto=auto`) into `~/wectl/` and `~/.local/bin/`.
```bash
git clone https://github.com/ltwtd/wectl.git /tmp/wectl-src && \
bash /tmp/wectl-src/src/v-0.2/v-0.2_full_src/build_all.sh && \
rm -rf /tmp/wectl-src
```

---

## 3. Development Milestones

### 3.1 `v-0.1` — Pure C++20 Modular Client & Zero-GPU Daemon
- Built a standalone C++20 modular architecture (`banner`, `core`, `ipc`, `menu`, `render`, `server`, `ui` `.cppm` modules) with zero external scripting runtime overhead.
- Implemented event-driven **Hyprland IPC** fullscreen detection to automatically freeze/resume the wallpaper engine with **0% GPU load** while gaming or watching fullscreen video.
- Added interactive terminal UI (`wectl`), multi-monitor detection (`wlr-randr` / `hyprctl`), systemd user service integration, and isolated `RPATH` library bundling.

### 3.2 `v-0.2` — Multi-Format Gallery, Seamless Switch & Ultra-Optimized Engine
- **Custom Ultra-Optimized NOCEF Engine Fork**: Stripped Chromium Embedded Framework (`CEF`) bloat, added a persistent **OpenGL Shader Binary Disk Cache** (`GL_ARB_get_program_binary` in `~/.cache/wectl/gl_shader_cache/`), eliminated 240Hz `localtime()` and D-Bus syscalls from the hot render loop, and forced hardware GPU video decoding (`hwdec=auto-safe` + zero-copy `vd-lavc-dr=yes`).
- **Multi-Format Support & Seamless Switching**: Unified management for 2D/3D Wallpaper Engine scenes (`scenes/`), hardware-decoded videos (`vids/`), and high-resolution images (`photos/`) with flicker-free transitions.
- **Built-in Web Gallery & Full Native Source Tree (`v-0.2_full_src`)**: Added a local web gallery server with live previews, hidden vault folders, and a unified `build_all.sh` pipeline to compile both the engine and client natively with `-O3 -march=native -flto=auto`.

---

<h2 id="4-previous-versions" align="center">Previous Versions</h2>

> **Verified Dependency Matrix:** [`docs/COMPATIBILITY_v0.1.md`](https://github.com/ltwtd/wectl/blob/main/docs/COMPATIBILITY_v0.1.md)

### 1. [`v-0.1`](https://github.com/ltwtd/wectl/releases/tag/v-0.1) — Pure C++20 Modular Client (Hyprland)

**1.1 Prebuilt Binary + `pacman` Dependencies + Custom Engine**
```bash
sudo pacman -S --needed --noconfirm ffmpeg mpv glew glm lz4 sdl2-compat wayland xdg-utils zstd curl && \
curl -fL https://github.com/ltwtd/wectl/releases/download/v-0.1/wectl-installer-bin -o /tmp/wectl-inst && \
chmod +x /tmp/wectl-inst && /tmp/wectl-inst && rm -f /tmp/wectl-inst
```

**1.2 Compile `wectl` Natively + `pacman` Dependencies + Custom Engine**
```bash
sudo pacman -S --needed --noconfirm base-devel gcc ffmpeg mpv glew glm lz4 sdl2-compat wayland xdg-utils zstd curl && \
curl -fL https://github.com/ltwtd/wectl/releases/download/v-0.1/wectl-installer-src -o /tmp/wectl-inst && \
chmod +x /tmp/wectl-inst && /tmp/wectl-inst && rm -f /tmp/wectl-inst
```

**1.3 Full Source Build — Engine, Dependencies & `wectl`**
```bash
mkdir -p /tmp/wectl-v01-full && \
curl -fL https://github.com/ltwtd/wectl/releases/download/assets/04_lwe-full-sources.tar.zst | tar --zstd -xf - -C /tmp/wectl-v01-full && \
curl -fL https://github.com/ltwtd/wectl/releases/download/v-0.1/wectl-src.tar.zst | tar --zstd -xf - -C /tmp/wectl-v01-full && \
cmake -S /tmp/wectl-v01-full/linux-wallpaperengine -B /tmp/wectl-v01-full/build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE="-O3 -march=native -flto=auto -DNDEBUG" && \
ninja -C /tmp/wectl-v01-full/build -j"\$(nproc)" && \
make -C /tmp/wectl-v01-full -j"\$(nproc)" && \
rm -rf /tmp/wectl-v01-full
```

<p align="right"><sub>by ltwtd</sub></p>
