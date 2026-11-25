#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INSTALL_DIR="$HOME/wectl"
BUILD_TMP="/tmp/wectl_full_build_$$"
trap 'rm -rf "$BUILD_TMP"' EXIT

mkdir -p "$INSTALL_DIR/bin" "$INSTALL_DIR/lib" "$HOME/.local/bin" "$BUILD_TMP"

echo "[1/3] Compiling ultra-optimized linux-wallpaperengine (NOCEF + Shader Disk Cache + GPU hwdec)..."
cmake -S "$ROOT_DIR/engine/linux-wallpaperengine" -B "$BUILD_TMP/lwe" -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_FLAGS_RELEASE="-O3 -march=native -flto=auto -fno-math-errno -fno-trapping-math -fomit-frame-pointer -DNDEBUG" \
  -DCMAKE_CXX_FLAGS_RELEASE="-O3 -march=native -flto=auto -fno-math-errno -fno-trapping-math -fomit-frame-pointer -DNDEBUG" \
  -DCMAKE_EXE_LINKER_FLAGS="-flto=auto -Wl,-rpath,\$ORIGIN/../lib"

ninja -C "$BUILD_TMP/lwe" -j"$(nproc)"
cp -f "$BUILD_TMP/lwe/output/linux-wallpaperengine" "$INSTALL_DIR/bin/"
cp -rf "$ROOT_DIR/engine/linux-wallpaperengine/assets" "$INSTALL_DIR/bin/"
strip -s "$INSTALL_DIR/bin/linux-wallpaperengine"

ldd "$INSTALL_DIR/bin/linux-wallpaperengine" | awk '/=> \// {print $3}' | sort -u | xargs -r -I{} cp -L "{}" "$INSTALL_DIR/lib/" 2>/dev/null || true
rm -f "$INSTALL_DIR/lib"/libc.so* "$INSTALL_DIR/lib"/libm.so* "$INSTALL_DIR/lib"/libpthread.so* "$INSTALL_DIR/lib"/libdl.so* "$INSTALL_DIR/lib"/librt.so* "$INSTALL_DIR/lib"/ld-linux* "$INSTALL_DIR/lib"/libstdc++.so* "$INSTALL_DIR/lib"/libgcc_s.so* "$INSTALL_DIR/lib"/libGL* "$INSTALL_DIR/lib"/libEGL* "$INSTALL_DIR/lib"/libwayland* "$INSTALL_DIR/lib"/libX11* "$INSTALL_DIR/lib"/libxcb* "$INSTALL_DIR/lib"/libdrm* "$INSTALL_DIR/lib"/libvulkan*

echo "[2/3] Compiling wectl C++20 native client..."
make -C "$ROOT_DIR/wectl" -j"$(nproc)"
cp -f "$ROOT_DIR/wectl/wectl" "$INSTALL_DIR/wectl"
chmod +x "$INSTALL_DIR/wectl" "$INSTALL_DIR/bin/linux-wallpaperengine"

echo "[3/3] Integrating into ~/wectl/ and ~/.local/bin/..."
ln -sf "$INSTALL_DIR/wectl" "$HOME/.local/bin/wectl"
ln -sf "$INSTALL_DIR/bin/linux-wallpaperengine" "$HOME/.local/bin/linux-wallpaperengine"

echo "Done! Everything built natively for $(uname -m) and installed to $INSTALL_DIR"
