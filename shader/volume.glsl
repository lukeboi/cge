@ctype mat4 hmm_mat4

@vs vs_volume
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 texCoords;

out vec3 TexCoords;

void main()
{
    //TexCoords = vec3(texCoords, 0.5); // We set the z-coordinate of the 3D texture to 0.5
    TexCoords = texCoords;
    gl_Position = vec4(position, 1.0);
}
@end

@fs fs_volume
uniform fs_vol_params {
    vec3 camera_pos; // Camera position in world space
};

in vec3 TexCoords;

uniform texture3D tex;
uniform sampler smp;

out vec4 FragColor;

void main() {
    vec3 ray_dir = normalize(TexCoords - camera_pos);
    vec3 ray_origin = TexCoords;
    vec3 ray_step = ray_dir * 0.01; // Adjust the step size for ray marching

    vec4 accumulated_color = vec4(0.0);
    vec3 current_pos = ray_origin;

    for (int i = 0; i < 1000; i++) { // Limit the number of steps to prevent infinite loops
        float sample_value = texture(sampler3D(tex, smp), current_pos).r;

        vec4 color_sample = vec4(sample_value, sample_value, sample_value, sample_value); // Grayscale volume, adjust as needed
        accumulated_color = accumulated_color + (1.0 - accumulated_color.a) * color_sample;

        current_pos += ray_step;

        if (accumulated_color.a >= 1.0) {
            break; // Stop if fully opaque
        }
    }

    FragColor = accumulated_color;
}
@end


@program volume vs_volume fs_volume