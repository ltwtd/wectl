export module wectl.web.assembler;
import <string>;
import <cmath>;
import <cstdio>;
import wectl.web.css_reset;
import wectl.web.css_mica;
import wectl.web.css_neon;
import wectl.web.css_cards;
import wectl.web.css_layout;
import wectl.web.sec_hero;
import wectl.web.sec_arch;
import wectl.web.sec_trees;
import wectl.web.sec_deploy;
import wectl.web.sec_cli;
import wectl.web.sec_roadmap;
import wectl.web.js_scroll;
import wectl.web.js_char;
import wectl.web.js_copy;
import wectl.web.js_kinetics;
import wectl.web.js_gl;

namespace {
    std::string generate_deep_space_atlas() {
        std::string atlas;
        atlas.reserve(10600000);
        atlas += "\nwindow.__DEEP_SPACE_ATLAS = [\n";
        char buf[64];
        constexpr int TOTAL_VECTORS = 365000;
        for (int i = 0; i < TOTAL_VECTORS; ++i) {
            double phi = i * 2.399963229728653;
            double r = std::sqrt(static_cast<double>(i) / static_cast<double>(TOTAL_VECTORS));
            double x = std::cos(phi) * r;
            double y = std::sin(phi) * r;
            double z = std::sin(i * 0.017) * 0.5 + 0.5;
            int len = std::snprintf(buf, sizeof(buf), "[%.5f,%.5f,%.4f],", x, y, z);
            atlas.append(buf, len);
            if ((i & 15) == 15) atlas.push_back('\n');
        }
        atlas += "];\n";
        return atlas;
    }
}

export namespace wectl::web {
    std::string build_full_html() {
        std::string html;
        html.reserve(10800000);
        html += "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n";
        html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        html += "<title>wectl // Architectural Specification &amp; Roadmap</title>\n<style>\n";
        html += css::get_reset();
        html += css::get_mica();
        html += css::get_neon_text();
        html += css::get_cards();
        html += css::get_layout();
        html += "</style>\n</head>\n<body>\n";
        html += "<canvas id=\"gl-canvas\"></canvas>\n";
        html += "<canvas id=\"fx-canvas\"></canvas>\n";
        html += "<div id=\"smooth-wrapper\"><div id=\"smooth-content\">\n";
        html += sec::render_hero();
        html += sec::render_arch();
        html += sec::render_trees();
        html += sec::render_deploy();
        html += sec::render_cli();
        html += sec::render_roadmap();
        html += "</div></div>\n<div id=\"toast-hud\"></div>\n<script>\n";
        html += generate_deep_space_atlas();
        html += js::get_smooth_scroll();
        html += js::get_char_physics();
        html += js::get_block_copy();
        html += js::get_card_kinetics();
        html += js::get_webgl_engine();
        html += "</script>\n</body>\n</html>\n";
        return html;
    }
}
