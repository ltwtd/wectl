# Compatibility Matrix — `wectl v-0.2`

Tested and verified toolchain and runtime library versions for building and running `wectl v-0.2` (Multi-Format Gallery, Seamless Switch & Ultra-Optimized NOCEF Engine):

## Build Toolchain

| Component | Verified Version | Minimum / Notes |
| :--- | :--- | :--- |
| **OS / Kernel** | CachyOS / Arch Linux (`7.3.5-2-cachyos`) | Any modern rolling Linux (Hyprland / wlroots) |
| **Compiler (C++20)** | `gcc` `16.2.1+r23+gd564253eb6c8-3` | GCC 14+ or Clang 18+ (`-std=c++20 -fmodules-ts -flto=auto`) |
| **CMake** | `4.5.1-1.1` | CMake 3.25+ |
| **Ninja** | `1.13.2-3.1` | Ninja 1.11+ |
| **Python** | `3.14.10-2` | Python 3.10+ |

## Core Runtime & Multi-Format Gallery Libraries

| Library | Verified Version | Purpose |
| :--- | :--- | :--- |
| **Hyprland / Wayland** | `wayland` `1.26.0-1.1` / `wayland-protocols` `1.50-1` | Hyprland IPC fullscreen auto-pause & `wlr-layer-shell` |
| **OpenGL / GLEW / GLM** | `glew` `2.3.1-1.1` / `glm` `1.0.3-1.1` | OpenGL 3.3+ pipeline + `GL_ARB_get_program_binary` shader disk cache |
| **FFmpeg** | `2:9.1-1.1` | Hardware-accelerated video decoding & thumbnail generation |
| **libmpv** | `1:0.42.0-3.1` | `hwdec=auto-safe` + zero-copy `vd-lavc-dr=yes` video rendering |
| **libplacebo** | `7.387.0-1.1` | High-precision tone mapping & color management |
| **libjxl** | `0.13.0-1.1` | JPEG XL static wallpaper & gallery support |
| **SDL2 / sndio** | `sdl2-compat` `2.32.74-1.1` / `sndio` `1.11.0-1` | Audio capture & playback backend |
| **LZ4 / Zstd** | `lz4` `1:1.11.1-2` / `zstd` `1.5.7-4` | `.pkg` asset decompression & release archive extraction |
| **FreeType / libpng / FFTW3** | `freetype2` `2.15.0-1.1` / `libpng` `1.6.59-1.1` / `fftw` `3.3.11-1.1` | Font rendering, image decoding & audio visualizer |
