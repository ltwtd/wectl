module;
#include <iostream>
#include <string_view>

export module wectl.banner;

export namespace wectl::banner {

inline void print_logo(std::string_view subtitle = "") {
    std::cout <<
        "\033[1;36m"
        " ██╗    ██╗███████╗ ██████╗████████╗██╗     \n"
        " ██║    ██║██╔════╝██╔════╝╚══██╔══╝██║     \n"
        " ██║ █╗ ██║█████╗  ██║        ██║   ██║     \n"
        " ██║███╗██║██╔══╝  ██║        ██║   ██║     \n"
        " ╚███╔███╔╝███████╗╚██████╗   ██║   ███████╗\n"
        "  ╚══╝╚══╝ ╚══════╝ ╚═════╝   ╚═╝   ╚══════╝\n"
        "\033[0m"
        " \033[1;32m[v-0.2]\033[0m " << subtitle << "\n\n";
}

}

