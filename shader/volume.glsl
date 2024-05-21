@ctype mat4 hmm_mat4

@vs vs_volume
layout(location=0) in vec3 pos;

uniform vs_vol_params {
    mat4 proj_view;
    vec3 eye_pos;
    vec3 volume_scale;
};

out vec3 vray_dir;
flat out vec3 transformed_eye;

void main(void) {
	// Translate the cube to center it at the origin.
	vec3 volume_translation = vec3(0.5) - volume_scale * 0.5;
	gl_Position = proj_view * vec4(pos * volume_scale + volume_translation, 1);

	// Compute eye position and ray directions in the unit cube space
	transformed_eye = (eye_pos - volume_translation) / volume_scale;
	vray_dir = pos - transformed_eye;
}
@end

@fs fs_volume
precision highp int;
precision highp float;

uniform fs_vol_params {
    ivec3 volume_dims;
    float dt_scale;
    float near_clip;
    float far_clip;
    vec3 new_box_min;
    vec3 new_box_max;
};

uniform texture3D volume;
uniform sampler volume_smp;
uniform texture2D colormap;
uniform sampler colormap_smp;

in vec3 vray_dir;
flat in vec3 transformed_eye;

out vec4 FragColor;

vec2 intersect_box(vec3 orig, vec3 dir) {
    vec3 box_min = new_box_min;
    vec3 box_max = new_box_max;
    vec3 inv_dir = 1.0 / dir;
    vec3 tmin_tmp = (box_min - orig) * inv_dir;
    vec3 tmax_tmp = (box_max - orig) * inv_dir;
    vec3 tmin = min(tmin_tmp, tmax_tmp);
    vec3 tmax = max(tmin_tmp, tmax_tmp);
    float t0 = max(tmin.x, max(tmin.y, tmin.z));
    float t1 = min(tmax.x, min(tmax.y, tmax.z));
    return vec2(t0, t1);
}

float wang_hash(int seed) {
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return float(seed % 2147483647) / float(2147483647);
}

float linear_to_srgb(float x) {
    if (x <= 0.0031308f) {
        return 12.92f * x;
    }
    return 1.055f * pow(x, 1.f / 2.4f) - 0.055f;
}

void main(void) {
    //FragColor = vec4(1.0f, 0.5f, 0.5f, 0.5f);
    vec3 ray_dir = normalize(vray_dir);
    vec2 t_hit = intersect_box(transformed_eye, ray_dir);
    if (t_hit.x > t_hit.y) {
        discard;
    }
    t_hit.x = max(t_hit.x, near_clip); // near clip
    t_hit.y = min(t_hit.y, far_clip); // far clip
    vec3 dt_vec = 1.0 / (vec3(volume_dims) * abs(ray_dir));
    float dt = dt_scale * min(dt_vec.x, min(dt_vec.y, dt_vec.z));
    float offset = wang_hash(int(gl_FragCoord.x + 1280.0 * gl_FragCoord.y));
    vec3 p = transformed_eye + (t_hit.x + offset * dt) * ray_dir;
    for (float t = t_hit.x; t < t_hit.y; t += dt) {
        ivec3 p_ivec = ivec3(floor(p * vec3(volume_dims)));
        //float val = texelFetch(volume, p_ivec, 0).r;
        float val = texture(sampler3D(volume, volume_smp), p_ivec).r;
        vec4 val_color = vec4(texture(sampler2D(colormap, colormap_smp), vec2(val, 0.5)).rgb, val);
        val_color.a = 1.0 - pow(1.0 - val_color.a, dt_scale);
        FragColor.rgb += (1.0 - FragColor.a) * val_color.a * val_color.rgb;
        FragColor.a += (1.0 - FragColor.a) * val_color.a;
        if (FragColor.a >= 0.99) {
            break;
        }
        p += ray_dir * dt;
    }
    //FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    //FragColor.r = linear_to_srgb(FragColor.r);
    //FragColor.g = linear_to_srgb(FragColor.g);
    //FragColor.b = linear_to_srgb(FragColor.b);
}
@end


@program volume vs_volume fs_volume