#!/usr/bin/env bash
set -e
systemctl --user stop wectl.service 2>/dev/null || true
systemctl --user disable wectl.service 2>/dev/null || true
pkill -9 -f wallpaperengine 2>/dev/null || true
killall -9 linux-wallpaperengine 2>/dev/null || true
pkill -9 -f "wectl daemon" 2>/dev/null || true
rm -f "$HOME/.config/systemd/user/wectl.service" "$HOME/.local/bin/wectl"
systemctl --user daemon-reload 2>/dev/null || true
rm -rf "$HOME/wectl" "$HOME/.local/share/wectl"
echo "wectl, systemd service, and ~/wectl directory completely removed."
