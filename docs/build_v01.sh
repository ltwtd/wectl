#!/usr/bin/env bash
set -euo pipefail

BASE="https://github.com/ltwtd/wectl/releases/download"
TMP="/tmp/wectl-v01-full"
trap 'rm -rf "$TMP"' EXIT

rm -rf "$TMP"
mkdir -p "$TMP" ~/wectl/bin ~/wectl/wallpapers/scenes ~/wectl/state ~/.local/bin

curl -fL "$BASE/assets/04_lwe-full-sources.tar.zst" | tar --zstd -xf - -C "$TMP"
curl -fL "$BASE/v-0.1/wectl-src.tar.zst" | tar --zstd -xf - -C "$TMP"
curl -fL "$BASE/assets/02_default-scene-blackhole-4k.tar.zst" | tar --zstd -xf - -C ~/wectl/wallpapers/scenes

cmake -S "$TMP/linux-wallpaperengine" -B "$TMP/build" -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_FLAGS_RELEASE="-O3 -march=native -flto=auto -DNDEBUG"
ninja -C "$TMP/build"

cp -f "$TMP/build/output/linux-wallpaperengine" ~/wectl/bin/
cp -rf "$TMP/linux-wallpaperengine/assets" ~/wectl/bin/

make -C "$TMP"
cp -f "$TMP/wectl" ~/wectl/wectl
ln -sf ~/wectl/wectl ~/.local/bin/wectl

~/wectl/wectl up
