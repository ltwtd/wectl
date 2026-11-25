# wectl

Ultra-fast C++20 live wallpaper daemon, CLI/TUI controller, and Web Gallery for **Hyprland / Wayland**, powered by a custom ultra-optimized **NOCEF** fork of `linux-wallpaperengine` (disk OpenGL shader binary cache, throttled 240Hz syscalls, forced GPU `hwdec=auto-safe` + zero-copy direct rendering).

---

## v-0.2 (Latest)

### 1. Prebuilt Binary + `pacman` Dependencies + Custom Optimized Engine
Installs system runtime libraries via `pacman`, downloads the prebuilt `wectl` client (`v-0.2`), and deploys the custom ultra-optimized `linux-wallpaperengine` bundle (`01_lwe-engine-native-full.tar.zst`) from `assets`.
```bash
sudo pacman -S --needed --noconfirm ffmpeg mpv glew glm libjxl libplacebo lz4 sdl2-compat sndio wayland xdg-utils zstd curl && \
curl -fL https://github.com -o /tmp/wectl-inst && \
chmod +x /tmp/wectl-inst && /tmp/wectl-inst && rm -f /tmp/wectl-inst
```

### 2. Compile `wectl` Natively (`-O3 -march=native`) + `pacman` Dependencies + Custom Engine
Installs runtime & build packages via `pacman`, compiles the modular C++20 `wectl` client locally for your CPU, and deploys the prebuilt custom `linux-wallpaperengine` bundle from `assets`.
```bash
sudo pacman -S --needed --noconfirm base-devel gcc cmake ninja ffmpeg mpv glew glm libjxl libplacebo lz4 sdl2-compat sndio wayland xdg-utils zstd curl && \
curl -fL https://github.com -o /tmp/wectl-inst && \
chmod +x /tmp/wectl-inst && /tmp/wectl-inst && rm -f /tmp/wectl-inst
```

### 3. Full Source Build — Compile Engine, Dependencies & `wectl` Natively for Your CPU
Downloads the complete patched source archive (`04_lwe-full-sources.tar.zst` + `wectl` C++20 source tree) and compiles the entire stack (`-O3 -march=native -flto=auto`) into `~/wectl/` and `~/.local/bin/`.
```bash
git clone https://github.com /tmp/wectl-src && \
bash /tmp/wectl-src/src/v-0.2/v-0.2_full_src/build_all.sh && \
rm -rf /tmp/wectl-src
```

---

## v-0.1

```bash
# 1. Prebuilt binary + pacman dependencies + custom engine from shared assets
sudo pacman -S --needed --noconfirm ffmpeg mpv glew glm lz4 sdl2-compat wayland xdg-utils zstd curl && curl -fL https://github.com -o /tmp/wectl-inst && chmod +x /tmp/wectl-inst && /tmp/wectl-inst && rm -f /tmp/wectl-inst

# 2. Native wectl compilation + pacman dependencies + custom engine from shared assets
sudo pacman -S --needed --noconfirm base-devel gcc ffmpeg mpv glew glm lz4 sdl2-compat wayland xdg-utils zstd curl && curl -fL https://github.com -o /tmp/wectl-inst && chmod +x /tmp/wectl-inst && /tmp/wectl-inst && rm -f /tmp/wectl-inst

# 3. Full source compilation of engine + dependencies + wectl for your CPU
mkdir -p /tmp/wectl-v01-full && curl -fL https://github.com | tar --zstd -xf - -C /tmp/wectl-v01-full && curl -fL https://github.com | tar --zstd -xf - -C /tmp/wectl-v01-full && cmake -S /tmp/wectl-v01-full/linux-wallpaperengine -B /tmp/wectl-v01-full/build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS_RELEASE="-O3 -march=native -flto=auto -DNDEBUG" && ninja -C /tmp/wectl-v01-full/build -j"\((nproc)" && make -C /tmp/wectl-v01-full -j"\)(nproc)" && rm -rf /tmp/wectl-v01-full
```

<p align="right"><sub>by ltwtd</sub></p>
