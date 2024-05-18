@ctype mat4 hmm_mat4

@vs vs_volume
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;

out vec3 TexCoords;

void main()
{
    TexCoords = vec3(texCoords, 0.5); // We set the z-coordinate of the 3D texture to 0.5
    gl_Position = vec4(position, 1.0);
}
@end

@fs fs_volume
in vec3 TexCoords;

out vec4 FragColor;

uniform texture3D tex;
uniform sampler smp;

void main()
{
    FragColor = texture(sampler3D(tex, smp), TexCoords);
}
@end


@program volume vs_volume fs_volume