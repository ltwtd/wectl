CXX      ?= g++
CXXFLAGS ?= -O3 -flto
CXXFLAGS += -std=c++20 -fmodules-ts -fno-rtti -pthread -Wall -Wextra -Wno-unused-result

PREFIX   ?= /usr
DESTDIR  ?=
BUILD    := .build
TARGET   := $(BUILD)/wectl

MOD_ORDER := \
	modules/banner.cppm \
	modules/core.cppm \
	modules/ipc.cppm \
	modules/render.cppm \
	modules/ui.cppm \
	modules/server.cppm \
	modules/menu.cppm

OBJS := $(patsubst modules/%.cppm,$(BUILD)/%.o,$(MOD_ORDER)) $(BUILD)/main.o

.PHONY: all build install clean

all: build

build: $(TARGET)

$(BUILD):
	@mkdir -p $(BUILD)

$(BUILD)/%.o: modules/%.cppm | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD)/ipc.o: $(BUILD)/core.o
$(BUILD)/render.o: $(BUILD)/core.o $(BUILD)/ipc.o
$(BUILD)/ui.o: $(BUILD)/banner.o $(BUILD)/core.o $(BUILD)/render.o
$(BUILD)/server.o: $(BUILD)/core.o $(BUILD)/ipc.o $(BUILD)/render.o $(BUILD)/ui.o
$(BUILD)/menu.o: $(BUILD)/banner.o $(BUILD)/core.o $(BUILD)/ipc.o $(BUILD)/render.o $(BUILD)/ui.o $(BUILD)/server.o
$(BUILD)/main.o: main.cpp $(patsubst modules/%.cppm,$(BUILD)/%.o,$(MOD_ORDER)) | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)
	@strip -s $(TARGET)

install: build
	install -Dm755 $(TARGET) "$(DESTDIR)$(PREFIX)/bin/wectl"
	install -Dm644 wectl.service "$(DESTDIR)$(PREFIX)/lib/systemd/user/wectl.service"

clean:
	rm -rf $(BUILD) gcm.cache

