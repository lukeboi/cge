@ctype vec3 hmm_vec3
@ctype mat4 hmm_mat4

@vs vs
in vec3 a_pos;
in vec2 a_tex_coords;

out vec2 tex_coords;

uniform vs_params {
    mat4 model;
    mat4 view;
    mat4 projection;
};

void main() {
    gl_Position = projection * view * model * vec4(a_pos, 1.0);
    tex_coords = a_tex_coords;
}
@end

@fs fs
uniform texture2D tex;
uniform sampler smp;

in vec2 tex_coords;

out vec4 frag_color;

void main() {
    frag_color = texture(sampler2D(tex, smp), tex_coords);
}
@end

@program phong vs fs