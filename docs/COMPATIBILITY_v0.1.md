# Compatibility Matrix — `wectl v-0.1`

Tested and verified toolchain and runtime library versions for building and running `wectl v-0.1` + custom `linux-wallpaperengine` (NOCEF):

## Build Toolchain

| Component | Verified Version | Minimum / Notes |
| :--- | :--- | :--- |
| **OS / Kernel** | CachyOS / Arch Linux (`7.3.5-2-cachyos`) | Any modern rolling Linux (Wayland) |
| **Compiler (C++20)** | `gcc` `16.2.1+r23+gd564253eb6c8-3` | GCC 13+ or Clang 17+ (`-std=c++20 -fmodules-ts`) |
| **CMake** | `4.5.1-1.1` | CMake 3.20+ (policies `CMP0077`, `CMP0148`) |
| **Ninja** | `1.13.2-3.1` | Ninja 1.11+ |
| **Python** | `3.14.10-2` | Python 3.10+ |

## Core Runtime & Engine Libraries

| Library | Verified Version | Purpose |
| :--- | :--- | :--- |
| **Wayland / Protocols** | `wayland` `1.26.0-1.1` / `wayland-protocols` `1.50-1` | Layer-shell & EGL surface output |
| **OpenGL / GLEW / GLM** | `glew` `2.3.1-1.1` / `glm` `1.0.3-1.1` | Core 3.3+ shader pipeline & math |
| **FFmpeg** | `2:9.1-1.1` | Audio/video demuxing & hardware decoding |
| **libmpv** | `1:0.42.0-3.1` | Zero-copy GPU video wallpaper playback (`hwdec=auto-safe`) |
| **SDL2** | `2.32.74-1.1` | Audio stream & input backend |
| **LZ4** | `1:1.11.1-2` | Fast `.pkg` / `.tex` asset decompression |
| **FreeType / libpng / FFTW3** | `freetype2` `2.15.0-1.1` / `libpng` `1.6.59-1.1` / `fftw` `3.3.11-1.1` | Text rendering, textures & audio spectrum |
