module;
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

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) return {};

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::snprintf(addr.sun_path, sizeof(addr.sun_path), "%s/hypr/%s/.socket.sock", xdg, his);

    if (connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        close(fd);
        return {};
    }

    write(fd, cmd.data(), cmd.size());
    std::string out;
    out.reserve(512);
    {
        char buf[512];
        ssize_t n;
        while ((n = read(fd, buf, sizeof(buf))) > 0) {
            out.append(buf, static_cast<size_t>(n));
        }
    }
    close(fd);
    out.shrink_to_fit();
    return out;
}

inline bool is_fullscreen_active() {
    {
        const std::string ws = query_socket("j/activeworkspace");
        if (ws.find("\"hasfullscreen\": true") != std::string::npos ||
            ws.find("\"hasfullscreen\":true") != std::string::npos) return true;
    }
    {
        const std::string win = query_socket("j/activewindow");
        return win.find("\"fullscreen\": 1") != std::string::npos ||
               win.find("\"fullscreen\": 2") != std::string::npos ||
               win.find("\"fullscreen\":1") != std::string::npos ||
               win.find("\"fullscreen\":2") != std::string::npos;
    }
}

inline int open_event_stream() {
    const char* his = std::getenv("HYPRLAND_INSTANCE_SIGNATURE");
    const char* xdg = std::getenv("XDG_RUNTIME_DIR");
    if (!his || !*his || !xdg || !*xdg) return -1;

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
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
