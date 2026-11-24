PREFIX ?= $(HOME)/wectl
BIN_DIR ?= $(HOME)/.local/bin
CXX := g++
CXXFLAGS := -std=c++20 -fmodules-ts -O3 -march=native -flto -fno-rtti -pthread

.PHONY: all build install clean-install uninstall

all: build

build:
	mkdir -p .build
	cd .build && $(CXX) $(CXXFLAGS) -c ../modules/banner.cppm -o banner.o
	cd .build && $(CXX) $(CXXFLAGS) -c ../modules/core.cppm -o core.o
	cd .build && $(CXX) $(CXXFLAGS) -c ../modules/ipc.cppm -o ipc.o
	cd .build && $(CXX) $(CXXFLAGS) -c ../modules/render.cppm -o render.o
	cd .build && $(CXX) $(CXXFLAGS) -c ../modules/ui.cppm -o ui.o
	cd .build && $(CXX) $(CXXFLAGS) -c ../modules/server.cppm -o server.o
	cd .build && $(CXX) $(CXXFLAGS) -c ../modules/menu.cppm -o menu.o
	cd .build && $(CXX) $(CXXFLAGS) -c ../main.cpp -o main.o
	cd .build && $(CXX) $(CXXFLAGS) banner.o core.o ipc.o render.o ui.o server.o menu.o main.o -o ../wectl
	strip -s wectl
	rm -rf .build

install: build
	cp -r skeleton/* $(PREFIX)/ 2>/dev/null || true
	mkdir -p $(PREFIX)/bin $(PREFIX)/state $(PREFIX)/wallpapers/scenes $(PREFIX)/wallpapers/scenes_hidden $(PREFIX)/wallpapers/vids $(PREFIX)/wallpapers/photos $(BIN_DIR)
	find $(PREFIX) -name ".gitkeep" -delete
	cp -f wectl $(PREFIX)/wectl
	$(PREFIX)/wectl relink
	rm -f wectl

clean-install: install
	REPO_ROOT="$$(cd ../.. && pwd)"; \
	cd /tmp && rm -rf "$$REPO_ROOT"

uninstall:
	$(PREFIX)/wectl uninstall
