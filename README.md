```text
██╗    ██╗███████╗ ██████╗████████╗██╗     
██║    ██║██╔════╝██╔════╝╚══██╔══╝██║     
██║ █╗ ██║█████╗  ██║        ██║   ██║     
██║███╗██║██╔══╝  ██║        ██║   ██║     
╚███╔███╔╝███████╗╚██████╗   ██║   ███████╗
 ╚══╝╚══╝ ╚══════╝ ╚═════╝   ╚═╝   ╚══════╝
:: C++20 WAYLAND LIVE WALLPAPER ENGINE & GALLERY FOR ARCH / HYPRLAND ::
```

---

## 01 // PHILOSOPHY & GOALS

`wectl` is a minimalist live wallpaper daemon and local web gallery written in pure **C++20** with zero external runtimes, virtual machines, or heavy framework bloat.

* **Zero Overhead:** Stripped binary footprint is **~115 KB**. Resident memory usage stays **< 3 MB** while idle.
* **Direct Hyprland IPC:** Connects directly to `$HYPRLAND_INSTANCE_SIGNATURE/.socket2.sock` via raw UNIX domain sockets—no polling, no shell wrappers.
* **0% GPU Freeze:** Automatically suspends rendering (`SIGSTOP` / IPC pause) the instant any window enters `fullscreen` or occludes the workspace, dropping GPU load to **0%**.
* **Portability:** Self-contained XDG-compliant layout with no hard systemd unit dependencies and single-file state persistence.

---

## 02 // QUICK INSTALL (v-0.1)

### Option 1: Native C++20 Compilation Launcher
Fetches the interactive setup binary, detects your C++20 toolchain (`clang++` / `g++`), and compiles natively with `-O3 -march=native`.

```bash
curl -fL https://github.com/ltwtd/wectl/releases/download/v-0.1/wectl-launcher-compile -o /tmp/wectl-setup && chmod +x /tmp/wectl-setup && /tmp/wectl-setup
```

```text
~/.local/
├── bin/
│   └── wectl                 # Native compiled binary (~115 KB)
└── share/
    └── wectl/
        ├── web/              # Embedded local web gallery assets
        └── build.log         # C++20 compilation log
~/.config/
└── wectl/
    ├── config.toml           # Daemon, IPC, and wallpaper directory config
    └── state.json            # Active wallpaper and output state cache
```

### Option 2: Prebuilt Binary Launcher
Downloads and deploys a pre-compiled static `x86_64` binary (Arch Linux / glibc) without requiring a local compiler toolchain.

```bash
curl -fL https://github.com/ltwtd/wectl/releases/download/v-0.1/wectl-launcher-prebuilt -o /tmp/wectl-setup && chmod +x /tmp/wectl-setup && /tmp/wectl-setup
```

```text
~/.local/
├── bin/
│   └── wectl                 # Prebuilt stripped binary (~115 KB)
└── share/
    └── wectl/
        └── web/              # Web gallery static files
~/.config/
└── wectl/
    ├── config.toml           # Default configuration generated on first run
    └── state.json            # Session state persistence
```

### Option 3: Manual Source Build
Clones the repository, cleans previous build artifacts, and builds directly via `Makefile`.

```bash
git clone https://github.com/ltwtd/wectl.git && cd wectl/src/v-0.1/v-0.1_git && make clean-install
```

```text
wectl/
└── src/
    └── v-0.1/
        └── v-0.1_git/
            ├── build/        # Intermediate object files (*.o)
            └── wectl         # Local build output
~/.local/
└── bin/
    └── wectl                 # Installed executable
~/.config/
└── wectl/
    └── config.toml           # Default configuration
```

---

## 03 // ARCHITECTURE

The codebase is structured around 2 entry points and 7 isolated **C++20 modules** with strict separation of concerns:

```text
src/v-0.1/v-0.1_git/
├── main.cpp                  # Daemon entry point, CLI argument parser, POSIX signal dispatcher
├── launcher.cpp              # Interactive TUI launcher, gallery selector, and terminal preview
└── modules/
    ├── ipc_hyprland.cppm     # Direct UNIX socket client for Hyprland (.socket.sock / .socket2.sock)
    ├── daemon_core.cppm      # Background process lifecycle, PID lockfile, child process supervisor
    ├── render_backend.cppm   # Abstraction over Wayland layer-shell backends (mpvpaper / swww)
    ├── power_guard.cppm      # Monitors fullscreen/activewindow events to enforce 0% GPU freeze
    ├── web_gallery.cppm      # Lightweight epoll-driven HTTP/1.1 server for the local web gallery
    ├── config_parser.cppm    # Zero-copy TOML configuration parser and JSON state serializer
    └── tui_engine.cppm       # Raw-mode terminal renderer (termios, ANSI escape sequences)
```

### Developer Notes
* **Standard:** `-std=c++20` (requires GCC 13+ or Clang 16+ for full C++20 module support).
* **Concurrency:** Single-threaded `epoll` event loop multiplexing Hyprland IPC events, signal file descriptors (`signalfd`), and local HTTP gallery sockets.
* **Hyprland Autostart:** Add `exec-once = wectl up` to `~/.config/hypr/hyprland.conf`.

---

## 04 // DEPENDENCIES & CLI / TUI REFERENCE

### System Dependencies (Arch Linux)

| Package | Requirement | Purpose |
| :--- | :--- | :--- |
| `hyprland` | Required | Wayland compositor and UNIX IPC event source |
| `mpvpaper` | Required | Hardware-accelerated video backend on `zwlr_layer_shell_v1` |
| `gcc` / `clang` | Build-only | Compiler toolchain supporting `-std=c++20` modules |
| `make` | Build-only | Build automation via `Makefile` |
| `curl` | Optional | Used by the bootstrap setup script |
| `jq` | Optional | External inspection of Hyprland JSON IPC payloads |

### CLI & TUI Commands

| Command | Mode | Description |
| :--- | :--- | :--- |
| `wectl` | TUI | Launches the interactive terminal UI gallery and wallpaper switcher |
| `wectl up` | CLI | Starts the background daemon and restores/applies the active wallpaper |
| `wectl status` | CLI | Prints daemon PID, RSS memory usage, GPU freeze state, and active asset |
| `wectl stop` | CLI | Halts wallpaper rendering while keeping the background daemon alive |
| `wectl down` | CLI | Gracefully terminates the daemon, child processes, and UNIX sockets |
| `wectl restart` | CLI | Restarts the daemon and reloads `~/.config/wectl/config.toml` |
| `wectl restore` | CLI | Re-applies the last active wallpaper recorded in `state.json` |
| `wectl relink` | CLI | Rebuilds symlinks for wallpaper directories and web gallery assets |
| `wectl uninstall` | CLI | Stops the daemon and removes the binary, cache, and config directories |

---

## 05 // ROADMAP

* **`[v-0.1]` (Current)** — Core C++20 engine, direct Hyprland IPC, 0% GPU fullscreen freeze, CLI control suite, raw-mode TUI, and local web gallery.
* **`[v-0.2]`** — Per-monitor independent profiles (`DP-*`, `HDMI-*`) and live `inotify` config hot-reloading.
* **`[v-0.3]`** — Native OpenGL ES / Vulkan GLSL shader renderer directly bound to `zwlr_layer_shell_v1` without external video wrappers.
* **`[v-0.4]`** — Real-time color palette extraction and automatic theme syncing for `hyprland`, `waybar`, `kitty`, and `rofi`.

---

<div align="right">
  <code>by ltwtd</code>
</div>

