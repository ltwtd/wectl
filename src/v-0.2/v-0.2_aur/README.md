# wectl

Ultra-lightweight, pure C++20 Wallpaper Engine manager and web gallery daemon for **Hyprland** (Arch Linux / CachyOS).

## Features
- **Zero-overhead C++20 daemon & web UI** (`http://127.0.0.1:43197`) — no Electron, no Python, no heavy toolkits.
- **Unified Media Support**:
  - **Scenes** (`linux-wallpaperengine`) with seamless transitions and live FPS control.
  - **Videos** (`mpvpaper`) with `hwdec=auto-safe` hardware acceleration and instant thumbnail generation.
  - **Photos** (`swaybg`) with automatic resolution detection.
- **Smart Fullscreen Auto-Pause (`0% GPU`)**: listens to Hyprland IPC events (`.socket2.sock`) and freezes (`SIGSTOP` / `SIGCONT`) active wallpapers whenever any window enters fullscreen.
- **Interactive CLI Hub & Direct Commands**: `wectl up`, `wectl status`, `wectl freeze`, `wectl down`, `wectl restart`, `wectl uninstall`.

---

## Quick Install — Latest Release (`v-0.2`)

### Option 1: Prebuilt Binary Installer (Fastest)
Downloads the precompiled native `x86_64` binary, engine, default 4K scene, and configures `systemd --user`:
```bash
curl -fL https://://github.com/releases/download/v-0.2/wectl-installer-bin -o /tmp/wectl-inst && chmod +x /tmp/wectl-inst && /tmp/wectl-inst
```

### Option 2: Native Source-Compiling Installer (`-O3 -march=native -flto`)
Fetches the 7 C++20 modules and compiles `wectl` directly on your machine with full CPU-specific optimizations:
```bash
curl -fL https://://github.com/releases/download/v-0.2/wectl-installer-src -o /tmp/wectl-inst && chmod +x /tmp/wectl-inst && /tmp/wectl-inst
```

---

## What's New in `v-0.2`
- **Multi-format Gallery**: dedicated tabs and badges for **Scenes**, **Videos**, and **Photos** with resolution tags (`3840x2160`, `2560x1440`, etc.) and animated GIF previews.
- **Seamless Scene Transitions**: overlapping background launch eliminates blank screen flashes when switching between scenes.
- **Instant Freeze / Resume Toggle**: `wectl freeze` (and menu option `3`) toggles `SIGSTOP` / `SIGCONT` with `0%` GPU load while paused.
- **Safe Uninstall**: tracks newly installed wallpaper tools (`mpvpaper`, `swaybg`) in `state/installed_deps.list` and removes only isolated dependencies without touching system graphics libraries.

---

## Usage & CLI Commands

Run `wectl` without arguments to open the interactive terminal hub, or use direct commands:

| Command | Description |
| :--- | :--- |
| `wectl` | Open interactive terminal control hub |
| `wectl up` | Start `wectl.service` and open the web gallery in browser |
| `wectl status` | Show display profile, daemon status, active media, FPS, PID, and RSS memory |
| `wectl freeze` | Toggle freeze/resume on current frame (`0%` GPU usage when frozen) |
| `wectl down` | Stop both daemon and active wallpaper renderer |
| `wectl restart` | Restart `wectl.service` and reload active wallpaper |
| `wectl uninstall` | Cleanly remove `wectl`, systemd unit, symlinks, and isolated wallpaper packages |

### Adding Your Own Wallpapers
Place your media into the corresponding subdirectories inside your installation root (default `~/wectl/wallpapers/`):
- `wallpapers/scenes/` — Wallpaper Engine scene folders (containing `scene.pkg` / `project.json`)
- `wallpapers/vids/` — `.mp4`, `.webm`, `.mkv`, `.mov`, `.gif`
- `wallpapers/photos/` — `.jpg`, `.png`, `.webp`, `.jxl`

---

## Previous Releases

### `v-0.1`
#### Prebuilt Binary Installer (`v-0.1`)
```bash
curl -fL https://://github.com/releases/download/v-0.1/wectl-installer-bin -o /tmp/wectl-inst && chmod +x /tmp/wectl-inst && /tmp/wectl-inst
```

#### Source-Compiling Installer (`v-0.1`)
```bash
curl -fL https://://github.com/releases/download/v-0.1/wectl-installer-src -o /tmp/wectl-inst && chmod +x /tmp/wectl-inst && /tmp/wectl-inst
```
