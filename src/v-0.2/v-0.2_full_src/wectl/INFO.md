# wectl v-0.2 — Build & Asset Specification

## Hosted Asset Bundle (`assets`)
Manifest URL: `https://github.com`

- `01_lwe-engine-native-full.tar.zst` — Prebuilt `linux-wallpaperengine` (`r643.a0ddce58-1`, built without Chromium/CEF) + base shaders/effects (`bin/assets/`).
- `02_default-scene-blackhole-4k.tar.zst` — Default `4K SCI-FI Black Hole` scene.
- `03_dep-ffmpeg-2_9.1-1.1-x86_64_v3.pkg.tar.zst`
- `03_dep-gcc-16.2.1+r23+gd564253eb6c8-3-x86_64_v3.pkg.tar.zst`
- `03_dep-glew-2.3.1-1.1-x86_64_v3.pkg.tar.zst`
- `03_dep-glm-1.0.3-1.1-x86_64_v3.pkg.tar.zst`
- `03_dep-libjxl-0.23.0-1.1-x86_64_v3.pkg.tar.zst`
- `03_dep-libplacebo-7.387.0-1.1-x86_64_v3.pkg.tar.zst`
- `03_dep-lz4-1_1.11.1-2-x86_64_v3.pkg.tar.zst`
- `03_dep-mpv-1_0.42.0-3.1-x86_64_v3.pkg.tar.zst`
- `03_dep-procps-ng-4.0.7-1.1-x86_64_v3.pkg.tar.zst`
- `03_dep-python-3.14.10-2-x86_64_v3.pkg.tar.zst`
- `03_dep-sdl2-compat-2.32.74-1.1-x86_64_v3.pkg.tar.zst`
- `03_dep-sndio-1.11.0-1-x86_64.pkg.tar.zst`
- `03_dep-wayland-1.26.0-1.1-x86_64_v3.pkg.tar.zst`
- `03_dep-xdg-utils-1.2.1-2-any.pkg.tar.zst`

## Compiler Flags
`g++ -std=c++20 -fmodules-ts -O3 -march=native -flto -fno-rtti -pthread`
