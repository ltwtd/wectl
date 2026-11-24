export module wectl.web.gl_vert;
import <string>;

export namespace wectl::web::gl {
    std::string get_vertex_shader() {
        return R"GLSL(#version 300 es
precision highp float;
layout(location = 0) in vec2 a_pos;
out vec2 v_uv;
void main() {
    v_uv = a_pos * 0.5 + 0.5;
    gl_Position = vec4(a_pos, 0.0, 1.0);
}
)GLSL";
    }
}
