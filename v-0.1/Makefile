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
	rm -rf .build

install: build
	mkdir -p $(PREFIX)/bin $(PREFIX)/state $(PREFIX)/wallpapers/scenes $(PREFIX)/wallpapers/scenes_hidden $(PREFIX)/wallpapers/vids $(PREFIX)/wallpapers/photos $(BIN_DIR)
	cp -f wectl $(PREFIX)/wectl
	$(PREFIX)/wectl relink
	rm -f wectl

clean-install: install
	REPO_ROOT="$$(cd .. && pwd)"; \
	cd /tmp && rm -rf "$$REPO_ROOT"

uninstall:
	$(PREFIX)/wectl uninstall
