# wectl (AUR)

**System-wide C++20 Modular Wallpaper Engine Daemon & TUI/CLI for Arch / Hyprland.**

Zero bloat. Native `pacman` integration. Automatic `0% GPU` pause in fullscreen.

## Install

Build and install directly from the `aur` branch using `makepkg`:

```bash
git clone -b aur --single-branch https://github.com /tmp/wectl-aur
cd /tmp/wectl-aur && makepkg -si && rm -rf /tmp/wectl-aur
```

Enable the user background service:

```bash
systemctl --user enable --now wectl.service
```

## Difference: `aur` vs `git` branch

* **`aur` branch (System Package):** Installs via `pacman` into `/usr/bin/wectl` with a systemd user unit in `/usr/lib/systemd/user/wectl.service`. Wallpapers and state are stored cleanly in `~/.local/share/wectl/`.
* **`git` / `main` branch (Portable):** Self-contained portable build living inside a single directory (`~/wectl`) with prebuilt/compiled release launchers.

## System Paths

```text
/usr/bin/wectl                          # Compiled C++20 binary
/usr/lib/systemd/user/wectl.service     # Systemd user daemon unit
~/.local/share/wectl/
├── state/                              # Monitor, FPS, active scene config
└── wallpapers/
    └── scenes/                         # Drop Steam Workshop scenes here
```

## CLI & Controls

Run `wectl` with no arguments to open the interactive TUI, or use direct commands:

| Command | Action |
| :--- | :--- |
| `wectl` | Open interactive TUI menu |
| `wectl up` | Enable and start background daemon |
| `wectl status` | Show active scene, FPS, and renderer PID/RAM |
| `wectl stop` | Pause rendering (`0% GPU`) |
| `wectl down` | Stop and disable background daemon |
| `wectl restore` | Reload last active scene |
| `wectl list` | List available scenes |
| `wectl set <id> [--fps N]` | Switch scene and optionally lock FPS |
| `wectl fps <N>` | Change target frame rate on the fly |

## Dependencies

* **Required:** `hyprland`, `gcc-libs`, `procps-ng`, `psmisc`, `xdg-utils`
* **Renderer Backend (Optional):** `linux-wallpaperengine-git` (or prebuilt binary from [Releases](https://github.com))

