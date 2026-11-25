module;
#include <array>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

export module wectl.ipc;

export namespace wectl::ipc {

inline std::string query_socket(std::string_view cmd) {
    const char* his = std::getenv("HYPRLAND_INSTANCE_SIGNATURE");
    const char* xdg = std::getenv("XDG_RUNTIME_DIR");
    if (!his || !*his || !xdg || !*xdg) return {};

    const int fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (fd < 0) return {};

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::snprintf(addr.sun_path, sizeof(addr.sun_path), "%s/hypr/%s/.socket.sock", xdg, his);

    if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        close(fd);
        return {};
    }

    if (write(fd, cmd.data(), cmd.size()) < 0) {
        close(fd);
        return {};
    }

    std::string out;
    out.reserve(512);
    std::array<char, 512> buf{};
    ssize_t n;
    while ((n = read(fd, buf.data(), buf.size())) > 0) {
        out.append(buf.data(), static_cast<size_t>(n));
    }
    close(fd);
    return out;
}

inline bool is_fullscreen_active() {
    const std::string ws = query_socket("j/activeworkspace");
    return ws.find("\"hasfullscreen\": true") != std::string::npos ||
           ws.find("\"hasfullscreen\":true") != std::string::npos;
}

inline int open_event_stream() {
    const char* his = std::getenv("HYPRLAND_INSTANCE_SIGNATURE");
    const char* xdg = std::getenv("XDG_RUNTIME_DIR");
    if (!his || !*his || !xdg || !*xdg) return -1;

    const int fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (fd < 0) return -1;

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::snprintf(addr.sun_path, sizeof(addr.sun_path), "%s/hypr/%s/.socket2.sock", xdg, his);

    if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        close(fd);
        return -1;
    }
    return fd;
}

}

