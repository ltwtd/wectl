CXX = g++
CXXFLAGS = -std=c++20 -O3 -fmodules-ts
MODS = \
	modules/01_config.cppm \
	modules/02_css_reset.cppm \
	modules/03_css_mica.cppm \
	modules/04_css_neon_text.cppm \
	modules/05_css_cards.cppm \
	modules/06_css_layout.cppm \
	modules/07_gl_shaders_vertex.cppm \
	modules/08_gl_shaders_eye.cppm \
	modules/09_gl_shaders_post.cppm \
	modules/10_js_smooth_scroll.cppm \
	modules/11_js_char_physics.cppm \
	modules/12_js_block_copy.cppm \
	modules/13_js_card_kinetics.cppm \
	modules/14_js_webgl_engine.cppm \
	modules/15_sec_hero.cppm \
	modules/16_sec_arch.cppm \
	modules/17_sec_trees.cppm \
	modules/18_sec_deploy.cppm \
	modules/19_sec_cli.cppm \
	modules/20_sec_roadmap.cppm \
	modules/21_html_assembler.cppm \
	modules/22_file_writer.cppm

all: build run

build:
	$(CXX) $(CXXFLAGS) -c -xc++-system-header string string_view fstream iostream cmath cstdio
	@for m in $(MODS); do \
		echo "[CXX] Compiling module $$m"; \
		$(CXX) $(CXXFLAGS) -c $$m -o $${m%.cppm}.o || exit 1; \
	done
	$(CXX) $(CXXFLAGS) main.cpp modules/*.o -o web_builder

run:
	./web_builder

clean:
	rm -rf gcm.cache modules/*.o web_builder index.html
