# wectl v-0.2 — Full Native Source Bundle (`v-0.2_full_src`)

Complete source tree to compile the entire wallpaper stack natively for your CPU (`-O3 -march=native -flto`):
- `engine/linux-wallpaperengine/` — Custom ultra-optimized NOCEF engine fork (disk OpenGL shader binary cache, 240Hz syscall throttling, forced GPU `hwdec=auto-safe` + zero-copy direct rendering).
- `wectl/` — Modular C++20 client & daemon (`~/wectl/wectl`).
- `build_all.sh` — One-command build & install script into `~/wectl/` and `~/.local/bin/`.

## Quick Build & Install
```bash
./build_all.sh
```
