<div align="center">

# wectl

**Portable C++20 Modular Wallpaper Engine CLI & minimalist web gallery for Hyprland (`~/wectl`) with native 0% GPU fullscreen auto-pause.**

</div>

---

## Install (`v-0.1`)

### 1. Native C++20 Compilation Launcher (Recommended)
```bash
curl -fL https://github.com/v-0.1/wectl-launcher-compile -o /tmp/wectl-setup && chmod +x /tmp/wectl-setup && /tmp/wectl-setup
```

### 2. Prebuilt Binary Launcher
```bash
curl -fL https://github.com/v-0.1/wectl-launcher-prebuilt -o /tmp/wectl-setup && chmod +x /tmp/wectl-setup && /tmp/wectl-setup
```

### 3. Manual Build (`git clone`)
```bash
git clone https://github.com && cd wectl/v-0.1 && make clean-install
```

---

## Uninstall

Removes `wectl.service`, symlinks, and the entire `wectl` directory:
```bash
wectl uninstall
```
