export module wectl.web.gl_post;
import <string>;

export namespace wectl::web::gl {
	std::string get_post_uniforms() {
		return "uniform float u_bloom_intensity;";
	}
}

