PREFIX ?= $(HOME)/wectl
BIN_DIR ?= $(HOME)/.local/bin
CXX := g++
CXXFLAGS := -std=c++20 -fmodules-ts -O3 -march=native -flto -fno-rtti -pthread

.PHONY: all build install clean-install uninstall

all: build

build:
	mkdir -p .build
	cd .build && $(CXX) $(CXXFLAGS) -c ../src/modules/banner.cppm -o banner.o
	cd .build && $(CXX) $(CXXFLAGS) -c ../src/modules/core.cppm -o core.o
	cd .build && $(CXX) $(CXXFLAGS) -c ../src/modules/ipc.cppm -o ipc.o
	cd .build && $(CXX) $(CXXFLAGS) -c ../src/modules/render.cppm -o render.o
	cd .build && $(CXX) $(CXXFLAGS) -c ../src/modules/ui.cppm -o ui.o
	cd .build && $(CXX) $(CXXFLAGS) -c ../src/modules/server.cppm -o server.o
	cd .build && $(CXX) $(CXXFLAGS) -c ../src/modules/menu.cppm -o menu.o
	cd .build && $(CXX) $(CXXFLAGS) -c ../src/main.cpp -o main.o
	cd .build && $(CXX) $(CXXFLAGS) banner.o core.o ipc.o render.o ui.o server.o menu.o main.o -o ../wectl
	strip -s wectl
	$(CXX) -std=c++20 -O3 -flto -fno-rtti ../src/relink.cpp -o ../relink && strip -s ../relink
	rm -rf .build

install: build
	mkdir -p $(PREFIX)/bin $(PREFIX)/state $(PREFIX)/wallpapers/scenes $(PREFIX)/wallpapers/scenes_hidden $(PREFIX)/wallpapers/vids $(PREFIX)/wallpapers/photos $(BIN_DIR)
	cp -f wectl relink $(PREFIX)/
	ln -sf $(PREFIX)/wectl $(BIN_DIR)/wectl
	@if [ ! -f $(PREFIX)/bin/linux-wallpaperengine ]; then \
		curl -fsSL https://github.com | tar -C $(PREFIX) --zstd -x; \
	fi
	@if [ ! -d "$(PREFIX)/wallpapers/scenes/4K SCI-FI Black Hole" ]; then \
		curl -fsSL https://github.com | tar -C $(PREFIX)/wallpapers/scenes --zstd -x; \
		echo "4K SCI-FI Black Hole" > $(PREFIX)/state/current; \
		echo "60" > $(PREFIX)/state/fps; \
		echo "fill" > $(PREFIX)/state/scale; \
	fi
	@MON=$$(hyprctl monitors 2>/dev/null | awk '/^Monitor /{print $$2; exit}'); \
	RES=$$(hyprctl monitors 2>/dev/null | awk '/^[[:space:]]+[0-9]+x[0-9]+@/{split($$1,a,"@"); print a[1]; exit}'); \
	HZ=$$(hyprctl monitors 2>/dev/null | awk '/^[[:space:]]+[0-9]+x[0-9]+@/{split($$1,a,"@"); printf "%.0f", a[2]; exit}'); \
	printf "MONITOR=%s\nRESOLUTION=%s\nMAX_HZ=%s\n" "$${MON:-eDP-1}" "$${RES:-1920x1080}" "$${HZ:-60}" > $(PREFIX)/state/hardware.env
	mkdir -p $(HOME)/.config/systemd/user
	printf "[Unit]\nDescription=Pure C++20 Wallpaper Engine Gallery Daemon (Hyprland)\nAfter=graphical-session.target\n\n[Service]\nPassEnvironment=WAYLAND_DISPLAY XDG_RUNTIME_DIR HYPRLAND_INSTANCE_SIGNATURE\nEnvironment=\"PATH=$(PREFIX)/bin:$(BIN_DIR):/usr/local/bin:/usr/bin:/bin\"\nExecStart=$(PREFIX)/wectl daemon\nRestart=on-failure\nRestartSec=1\n\n[Install]\nWantedBy=default.target\n" > $(HOME)/.config/systemd/user/wectl.service
	systemctl --user import-environment WAYLAND_DISPLAY XDG_RUNTIME_DIR HYPRLAND_INSTANCE_SIGNATURE PATH 2>/dev/null || true
	systemctl --user daemon-reload
	systemctl --user enable --now wectl.service
	systemctl --user restart wectl.service
	rm -f wectl relink

clean-install: install
	REPO_ROOT="$$(cd .. && pwd)"; \
	cd /tmp && rm -rf "$$REPO_ROOT"

uninstall:
	systemctl --user stop wectl.service 2>/dev/null || true
	systemctl --user disable wectl.service 2>/dev/null || true
	pkill -9 -f wallpaperengine 2>/dev/null || true
	killall -9 linux-wallpaperengine 2>/dev/null || true
	rm -f $(HOME)/.config/systemd/user/wectl.service $(BIN_DIR)/wectl
	rm -rf $(PREFIX)
