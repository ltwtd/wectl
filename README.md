# wectl — Ultra-Fast Wayland / Hyprland Live Wallpaper Engine & Client

Modular C++20 wallpaper daemon and CLI/TUI manager for **Hyprland / Wayland**, powered by a custom ultra-optimized **NOCEF** fork of `linux-wallpaperengine`.

## Key Optimizations (`v-0.2`)
- **Disk OpenGL Shader Binary Cache (`GL_ARB_get_program_binary`)**: Compiled shaders are cached in `~/.cache/wectl/gl_shader_cache/` for near-instant scene startup.
- **240Hz Loop Overhead Elimination**: Throttled D-Bus media polling and system clock queries out of the hot render loop.
- **Forced GPU Video Decoding**: Automatic `hwdec=auto-safe` + zero-copy direct rendering (`vd-lavc-dr=yes`) for embedded video textures.
- **Pure NOCEF Native Build**: Compiled with `-O3 -march=native -flto=auto` without Chromium overhead.

## Repository Layout
- `src/v-0.2/v-0.2_git/` — Modular C++20 client (`git` release)
- `src/v-0.2/v-0.2_aur/` — Arch Linux `PKGBUILD` & systemd unit (`aur` release)
- `src/v-0.2/v-0.2_full_src/` — Complete source tree (`engine/linux-wallpaperengine/` + `wectl/` + `build_all.sh`) to compile the entire stack natively for your CPU.

## Quick Start

### Option A: Prebuilt Native Bundle
Download from the [`v-0.2`](https://github.com) and shared [`assets`](https://github.com) releases.

### Option B: Compile Everything Natively for Your CPU (`-O3 -march=native -flto`)
```bash
git clone https://github.com
cd wectl/src/v-0.2/v-0.2_full_src
./build_all.sh
```
This compiles the custom `linux-wallpaperengine` core and `wectl` client for your exact processor architecture and installs everything into `~/wectl/` and `~/.local/bin/`.
