module;
#include <iostream>
#include <string_view>

export module wectl.banner;

export namespace wectl::banner {

inline void print_logo(std::string_view sub = "C++20 Modular Wallpaper Engine [Hyprland]") {
    std::cout <<
        "\033[1;36m"
        " ██╗    ██╗███████╗ ██████╗████████╗██╗     \n"
        " ██║    ██║██╔════╝██╔════╝╚══██╔══╝██║     \n"
        " ██║ █╗ ██║█████╗  ██║        ██║   ██║     \n"
        " ██║███╗██║██╔══╝  ██║        ██║   ██║     \n"
        " ╚███╔███╔╝███████╗╚██████╗   ██║   ███████╗\n"
        "  ╚══╝╚══╝ ╚══════╝ ╚═════╝   ╚═╝   ╚══════╝\n"
        "\033[0m"
        " \033[1;32m[v-0.1]\033[0m " << sub << "\n\n";
}

}
