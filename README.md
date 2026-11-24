<div align="center">

# wectl

**Ultra-fast 6-module C++20 Wallpaper Engine client & minimalist web gallery for Hyprland (`~/wectl`) with native 0% GPU fullscreen auto-pause.**

</div>

---

## Install (`v-0.1` — Native C++20 Compilation)

Downloads `wectl-launcher`, installs missing dependencies, compiles 6 C++20 modules (`-O3 -march=native -flto -fno-rtti`) into portable **`~/wectl`**, launches **4K SCI-FI Black Hole**, and deletes all temporary files & itself:

```bash
curl -fL https://github.com/ltwtd/wectl/releases/download/v-0.1/wectl-launcher -o /tmp/wectl-launcher && chmod +x /tmp/wectl-launcher && /tmp/wectl-launcher
```

Or via `git clone`:
```bash
git clone https://github.com/ltwtd/wectl.git && cd wectl/v-0.1 && make clean-install
```

---

## Complete Uninstall

Removes `wectl.service`, symlinks, and the entire `~/wectl` directory:
```bash
wectl uninstall
```
