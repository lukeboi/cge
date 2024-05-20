#pragma once
/*
    #version:1# (machine generated, don't edit!)

    Generated by sokol-shdc (https://github.com/floooh/sokol-tools)

    Cmdline:
        sokol-shdc --input shader/volume.glsl --output shader/volume.glsl.h --slang glsl430:hlsl5:metal_macos -f sokol

    Overview:
    =========
    Shader program: 'volume':
        Get shader desc: volume_shader_desc(sg_query_backend());
        Vertex shader: vs_volume
            Attributes:
                ATTR_vs_volume_position => 0
                ATTR_vs_volume_texCoords => 1
        Fragment shader: fs_volume
            Uniform block 'fs_vol_params':
                C struct: fs_vol_params_t
                Bind slot: SLOT_fs_vol_params => 0
            Image 'tex':
                Image type: SG_IMAGETYPE_3D
                Sample type: SG_IMAGESAMPLETYPE_FLOAT
                Multisampled: false
                Bind slot: SLOT_tex => 0
            Sampler 'smp':
                Type: SG_SAMPLERTYPE_FILTERING
                Bind slot: SLOT_smp => 0
            Image Sampler Pair 'tex_smp':
                Image: tex
                Sampler: smp
*/
#if !defined(SOKOL_GFX_INCLUDED)
#error "Please include sokol_gfx.h before volume.glsl.h"
#endif
#if !defined(SOKOL_SHDC_ALIGN)
#if defined(_MSC_VER)
#define SOKOL_SHDC_ALIGN(a) __declspec(align(a))
#else
#define SOKOL_SHDC_ALIGN(a) __attribute__((aligned(a)))
#endif
#endif
#define ATTR_vs_volume_position (0)
#define ATTR_vs_volume_texCoords (1)
#define SLOT_fs_vol_params (0)
#define SLOT_tex (0)
#define SLOT_smp (0)
#pragma pack(push,1)
SOKOL_SHDC_ALIGN(16) typedef struct fs_vol_params_t {
    float camera_pos[3];
    uint8_t _pad_12[4];
} fs_vol_params_t;
#pragma pack(pop)
/*
    #version 430

    layout(location = 0) out vec3 TexCoords;
    layout(location = 1) in vec3 texCoords;
    layout(location = 0) in vec3 position;

    void main()
    {
        TexCoords = texCoords;
        gl_Position = vec4(position, 1.0);
    }

*/
static const uint8_t vs_volume_source_glsl430[219] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x34,0x33,0x30,0x0a,0x0a,0x6c,0x61,
    0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,
    0x30,0x29,0x20,0x6f,0x75,0x74,0x20,0x76,0x65,0x63,0x33,0x20,0x54,0x65,0x78,0x43,
    0x6f,0x6f,0x72,0x64,0x73,0x3b,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,
    0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x31,0x29,0x20,0x69,0x6e,0x20,0x76,
    0x65,0x63,0x33,0x20,0x74,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x73,0x3b,0x0a,0x6c,
    0x61,0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,
    0x20,0x30,0x29,0x20,0x69,0x6e,0x20,0x76,0x65,0x63,0x33,0x20,0x70,0x6f,0x73,0x69,
    0x74,0x69,0x6f,0x6e,0x3b,0x0a,0x0a,0x76,0x6f,0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,
    0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x54,0x65,0x78,0x43,0x6f,0x6f,0x72,
    0x64,0x73,0x20,0x3d,0x20,0x74,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x73,0x3b,0x0a,
    0x20,0x20,0x20,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,
    0x3d,0x20,0x76,0x65,0x63,0x34,0x28,0x70,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x2c,
    0x20,0x31,0x2e,0x30,0x29,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
/*
    #version 430

    uniform vec4 fs_vol_params[1];
    layout(binding = 0) uniform sampler3D tex_smp;

    layout(location = 0) in vec3 TexCoords;
    layout(location = 0) out vec4 FragColor;

    void main()
    {
        vec3 _28 = normalize(TexCoords - fs_vol_params[0].xyz) * 0.00999999977648258209228515625;
        vec4 accumulated_color = vec4(0.0);
        vec3 current_pos = TexCoords;
        for (int i = 0; i < 1000; i++)
        {
            vec3 _59 = current_pos;
            vec4 _70 = accumulated_color;
            vec4 _78 = _70 + (vec4(texture(tex_smp, _59).x) * (1.0 - _70.w));
            accumulated_color = _78;
            current_pos = _59 + _28;
            if (_78.w >= 1.0)
            {
                break;
            }
        }
        FragColor = accumulated_color;
    }

*/
static const uint8_t fs_volume_source_glsl430[718] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x34,0x33,0x30,0x0a,0x0a,0x75,0x6e,
    0x69,0x66,0x6f,0x72,0x6d,0x20,0x76,0x65,0x63,0x34,0x20,0x66,0x73,0x5f,0x76,0x6f,
    0x6c,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,0x31,0x5d,0x3b,0x0a,0x6c,0x61,0x79,
    0x6f,0x75,0x74,0x28,0x62,0x69,0x6e,0x64,0x69,0x6e,0x67,0x20,0x3d,0x20,0x30,0x29,
    0x20,0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x20,0x73,0x61,0x6d,0x70,0x6c,0x65,0x72,
    0x33,0x44,0x20,0x74,0x65,0x78,0x5f,0x73,0x6d,0x70,0x3b,0x0a,0x0a,0x6c,0x61,0x79,
    0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x30,
    0x29,0x20,0x69,0x6e,0x20,0x76,0x65,0x63,0x33,0x20,0x54,0x65,0x78,0x43,0x6f,0x6f,
    0x72,0x64,0x73,0x3b,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,
    0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x30,0x29,0x20,0x6f,0x75,0x74,0x20,0x76,0x65,
    0x63,0x34,0x20,0x46,0x72,0x61,0x67,0x43,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x0a,0x76,
    0x6f,0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,
    0x20,0x76,0x65,0x63,0x33,0x20,0x5f,0x32,0x38,0x20,0x3d,0x20,0x6e,0x6f,0x72,0x6d,
    0x61,0x6c,0x69,0x7a,0x65,0x28,0x54,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x73,0x20,
    0x2d,0x20,0x66,0x73,0x5f,0x76,0x6f,0x6c,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,
    0x30,0x5d,0x2e,0x78,0x79,0x7a,0x29,0x20,0x2a,0x20,0x30,0x2e,0x30,0x30,0x39,0x39,
    0x39,0x39,0x39,0x39,0x39,0x37,0x37,0x36,0x34,0x38,0x32,0x35,0x38,0x32,0x30,0x39,
    0x32,0x32,0x38,0x35,0x31,0x35,0x36,0x32,0x35,0x3b,0x0a,0x20,0x20,0x20,0x20,0x76,
    0x65,0x63,0x34,0x20,0x61,0x63,0x63,0x75,0x6d,0x75,0x6c,0x61,0x74,0x65,0x64,0x5f,
    0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x76,0x65,0x63,0x34,0x28,0x30,0x2e,0x30,
    0x29,0x3b,0x0a,0x20,0x20,0x20,0x20,0x76,0x65,0x63,0x33,0x20,0x63,0x75,0x72,0x72,
    0x65,0x6e,0x74,0x5f,0x70,0x6f,0x73,0x20,0x3d,0x20,0x54,0x65,0x78,0x43,0x6f,0x6f,
    0x72,0x64,0x73,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6f,0x72,0x20,0x28,0x69,0x6e,
    0x74,0x20,0x69,0x20,0x3d,0x20,0x30,0x3b,0x20,0x69,0x20,0x3c,0x20,0x31,0x30,0x30,
    0x30,0x3b,0x20,0x69,0x2b,0x2b,0x29,0x0a,0x20,0x20,0x20,0x20,0x7b,0x0a,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x76,0x65,0x63,0x33,0x20,0x5f,0x35,0x39,0x20,0x3d,
    0x20,0x63,0x75,0x72,0x72,0x65,0x6e,0x74,0x5f,0x70,0x6f,0x73,0x3b,0x0a,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x76,0x65,0x63,0x34,0x20,0x5f,0x37,0x30,0x20,0x3d,
    0x20,0x61,0x63,0x63,0x75,0x6d,0x75,0x6c,0x61,0x74,0x65,0x64,0x5f,0x63,0x6f,0x6c,
    0x6f,0x72,0x3b,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x76,0x65,0x63,0x34,
    0x20,0x5f,0x37,0x38,0x20,0x3d,0x20,0x5f,0x37,0x30,0x20,0x2b,0x20,0x28,0x76,0x65,
    0x63,0x34,0x28,0x74,0x65,0x78,0x74,0x75,0x72,0x65,0x28,0x74,0x65,0x78,0x5f,0x73,
    0x6d,0x70,0x2c,0x20,0x5f,0x35,0x39,0x29,0x2e,0x78,0x29,0x20,0x2a,0x20,0x28,0x31,
    0x2e,0x30,0x20,0x2d,0x20,0x5f,0x37,0x30,0x2e,0x77,0x29,0x29,0x3b,0x0a,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x61,0x63,0x63,0x75,0x6d,0x75,0x6c,0x61,0x74,0x65,
    0x64,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x5f,0x37,0x38,0x3b,0x0a,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x63,0x75,0x72,0x72,0x65,0x6e,0x74,0x5f,0x70,
    0x6f,0x73,0x20,0x3d,0x20,0x5f,0x35,0x39,0x20,0x2b,0x20,0x5f,0x32,0x38,0x3b,0x0a,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x69,0x66,0x20,0x28,0x5f,0x37,0x38,0x2e,
    0x77,0x20,0x3e,0x3d,0x20,0x31,0x2e,0x30,0x29,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x7b,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x62,0x72,0x65,0x61,0x6b,0x3b,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x7d,
    0x0a,0x20,0x20,0x20,0x20,0x7d,0x0a,0x20,0x20,0x20,0x20,0x46,0x72,0x61,0x67,0x43,
    0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x61,0x63,0x63,0x75,0x6d,0x75,0x6c,0x61,0x74,
    0x65,0x64,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
/*
    static float4 gl_Position;
    static float3 TexCoords;
    static float3 texCoords;
    static float3 position;

    struct SPIRV_Cross_Input
    {
        float3 position : TEXCOORD0;
        float3 texCoords : TEXCOORD1;
    };

    struct SPIRV_Cross_Output
    {
        float3 TexCoords : TEXCOORD0;
        float4 gl_Position : SV_Position;
    };

    void vert_main()
    {
        TexCoords = texCoords;
        gl_Position = float4(position, 1.0f);
    }

    SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
    {
        texCoords = stage_input.texCoords;
        position = stage_input.position;
        vert_main();
        SPIRV_Cross_Output stage_output;
        stage_output.gl_Position = gl_Position;
        stage_output.TexCoords = TexCoords;
        return stage_output;
    }
*/
static const uint8_t vs_volume_source_hlsl5[694] = {
    0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x67,0x6c,
    0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x3b,0x0a,0x73,0x74,0x61,0x74,0x69,
    0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x33,0x20,0x54,0x65,0x78,0x43,0x6f,0x6f,0x72,
    0x64,0x73,0x3b,0x0a,0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,
    0x33,0x20,0x74,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x73,0x3b,0x0a,0x73,0x74,0x61,
    0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x33,0x20,0x70,0x6f,0x73,0x69,0x74,
    0x69,0x6f,0x6e,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x53,0x50,0x49,
    0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x49,0x6e,0x70,0x75,0x74,0x0a,0x7b,
    0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x33,0x20,0x70,0x6f,0x73,0x69,
    0x74,0x69,0x6f,0x6e,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,0x52,0x44,0x30,
    0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x33,0x20,0x74,0x65,0x78,
    0x43,0x6f,0x6f,0x72,0x64,0x73,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,0x52,
    0x44,0x31,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x53,
    0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,0x74,0x70,0x75,
    0x74,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x33,0x20,0x54,
    0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x73,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,
    0x4f,0x52,0x44,0x30,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,
    0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x3a,0x20,0x53,
    0x56,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,
    0x76,0x6f,0x69,0x64,0x20,0x76,0x65,0x72,0x74,0x5f,0x6d,0x61,0x69,0x6e,0x28,0x29,
    0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x54,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x73,
    0x20,0x3d,0x20,0x74,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x73,0x3b,0x0a,0x20,0x20,
    0x20,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,
    0x66,0x6c,0x6f,0x61,0x74,0x34,0x28,0x70,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x2c,
    0x20,0x31,0x2e,0x30,0x66,0x29,0x3b,0x0a,0x7d,0x0a,0x0a,0x53,0x50,0x49,0x52,0x56,
    0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,0x74,0x70,0x75,0x74,0x20,0x6d,0x61,
    0x69,0x6e,0x28,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x49,
    0x6e,0x70,0x75,0x74,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,0x74,
    0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x74,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,
    0x73,0x20,0x3d,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,0x74,0x2e,
    0x74,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x73,0x3b,0x0a,0x20,0x20,0x20,0x20,0x70,
    0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,
    0x69,0x6e,0x70,0x75,0x74,0x2e,0x70,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x3b,0x0a,
    0x20,0x20,0x20,0x20,0x76,0x65,0x72,0x74,0x5f,0x6d,0x61,0x69,0x6e,0x28,0x29,0x3b,
    0x0a,0x20,0x20,0x20,0x20,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,
    0x5f,0x4f,0x75,0x74,0x70,0x75,0x74,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,0x75,
    0x74,0x70,0x75,0x74,0x3b,0x0a,0x20,0x20,0x20,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,
    0x6f,0x75,0x74,0x70,0x75,0x74,0x2e,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,
    0x6f,0x6e,0x20,0x3d,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,
    0x3b,0x0a,0x20,0x20,0x20,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,0x75,0x74,0x70,
    0x75,0x74,0x2e,0x54,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x73,0x20,0x3d,0x20,0x54,
    0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x73,0x3b,0x0a,0x20,0x20,0x20,0x20,0x72,0x65,
    0x74,0x75,0x72,0x6e,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,0x75,0x74,0x70,0x75,
    0x74,0x3b,0x0a,0x7d,0x0a,0x00,
};
/*
    cbuffer fs_vol_params : register(b0)
    {
        float3 _15_camera_pos : packoffset(c0);
    };

    Texture3D<float4> tex : register(t0);
    SamplerState smp : register(s0);

    static float3 TexCoords;
    static float4 FragColor;

    struct SPIRV_Cross_Input
    {
        float3 TexCoords : TEXCOORD0;
    };

    struct SPIRV_Cross_Output
    {
        float4 FragColor : SV_Target0;
    };

    void frag_main()
    {
        float3 _28 = normalize(TexCoords - _15_camera_pos) * 0.00999999977648258209228515625f;
        float4 accumulated_color = 0.0f.xxxx;
        float3 current_pos = TexCoords;
        for (int i = 0; i < 1000; i++)
        {
            float3 _59 = current_pos;
            float4 _70 = accumulated_color;
            float4 _78 = _70 + (tex.Sample(smp, _59).x.xxxx * (1.0f - _70.w));
            accumulated_color = _78;
            current_pos = _59 + _28;
            if (_78.w >= 1.0f)
            {
                break;
            }
        }
        FragColor = accumulated_color;
    }

    SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
    {
        TexCoords = stage_input.TexCoords;
        frag_main();
        SPIRV_Cross_Output stage_output;
        stage_output.FragColor = FragColor;
        return stage_output;
    }
*/
static const uint8_t fs_volume_source_hlsl5[1114] = {
    0x63,0x62,0x75,0x66,0x66,0x65,0x72,0x20,0x66,0x73,0x5f,0x76,0x6f,0x6c,0x5f,0x70,
    0x61,0x72,0x61,0x6d,0x73,0x20,0x3a,0x20,0x72,0x65,0x67,0x69,0x73,0x74,0x65,0x72,
    0x28,0x62,0x30,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,
    0x33,0x20,0x5f,0x31,0x35,0x5f,0x63,0x61,0x6d,0x65,0x72,0x61,0x5f,0x70,0x6f,0x73,
    0x20,0x3a,0x20,0x70,0x61,0x63,0x6b,0x6f,0x66,0x66,0x73,0x65,0x74,0x28,0x63,0x30,
    0x29,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x54,0x65,0x78,0x74,0x75,0x72,0x65,0x33,0x44,
    0x3c,0x66,0x6c,0x6f,0x61,0x74,0x34,0x3e,0x20,0x74,0x65,0x78,0x20,0x3a,0x20,0x72,
    0x65,0x67,0x69,0x73,0x74,0x65,0x72,0x28,0x74,0x30,0x29,0x3b,0x0a,0x53,0x61,0x6d,
    0x70,0x6c,0x65,0x72,0x53,0x74,0x61,0x74,0x65,0x20,0x73,0x6d,0x70,0x20,0x3a,0x20,
    0x72,0x65,0x67,0x69,0x73,0x74,0x65,0x72,0x28,0x73,0x30,0x29,0x3b,0x0a,0x0a,0x73,
    0x74,0x61,0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x33,0x20,0x54,0x65,0x78,
    0x43,0x6f,0x6f,0x72,0x64,0x73,0x3b,0x0a,0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,
    0x6c,0x6f,0x61,0x74,0x34,0x20,0x46,0x72,0x61,0x67,0x43,0x6f,0x6c,0x6f,0x72,0x3b,
    0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,
    0x72,0x6f,0x73,0x73,0x5f,0x49,0x6e,0x70,0x75,0x74,0x0a,0x7b,0x0a,0x20,0x20,0x20,
    0x20,0x66,0x6c,0x6f,0x61,0x74,0x33,0x20,0x54,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,
    0x73,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,0x52,0x44,0x30,0x3b,0x0a,0x7d,
    0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x53,0x50,0x49,0x52,0x56,0x5f,
    0x43,0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,0x74,0x70,0x75,0x74,0x0a,0x7b,0x0a,0x20,
    0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x46,0x72,0x61,0x67,0x43,0x6f,
    0x6c,0x6f,0x72,0x20,0x3a,0x20,0x53,0x56,0x5f,0x54,0x61,0x72,0x67,0x65,0x74,0x30,
    0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x76,0x6f,0x69,0x64,0x20,0x66,0x72,0x61,0x67,0x5f,
    0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,
    0x61,0x74,0x33,0x20,0x5f,0x32,0x38,0x20,0x3d,0x20,0x6e,0x6f,0x72,0x6d,0x61,0x6c,
    0x69,0x7a,0x65,0x28,0x54,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x73,0x20,0x2d,0x20,
    0x5f,0x31,0x35,0x5f,0x63,0x61,0x6d,0x65,0x72,0x61,0x5f,0x70,0x6f,0x73,0x29,0x20,
    0x2a,0x20,0x30,0x2e,0x30,0x30,0x39,0x39,0x39,0x39,0x39,0x39,0x39,0x37,0x37,0x36,
    0x34,0x38,0x32,0x35,0x38,0x32,0x30,0x39,0x32,0x32,0x38,0x35,0x31,0x35,0x36,0x32,
    0x35,0x66,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x61,
    0x63,0x63,0x75,0x6d,0x75,0x6c,0x61,0x74,0x65,0x64,0x5f,0x63,0x6f,0x6c,0x6f,0x72,
    0x20,0x3d,0x20,0x30,0x2e,0x30,0x66,0x2e,0x78,0x78,0x78,0x78,0x3b,0x0a,0x20,0x20,
    0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x33,0x20,0x63,0x75,0x72,0x72,0x65,0x6e,0x74,
    0x5f,0x70,0x6f,0x73,0x20,0x3d,0x20,0x54,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x73,
    0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6f,0x72,0x20,0x28,0x69,0x6e,0x74,0x20,0x69,
    0x20,0x3d,0x20,0x30,0x3b,0x20,0x69,0x20,0x3c,0x20,0x31,0x30,0x30,0x30,0x3b,0x20,
    0x69,0x2b,0x2b,0x29,0x0a,0x20,0x20,0x20,0x20,0x7b,0x0a,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x33,0x20,0x5f,0x35,0x39,0x20,0x3d,0x20,
    0x63,0x75,0x72,0x72,0x65,0x6e,0x74,0x5f,0x70,0x6f,0x73,0x3b,0x0a,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x5f,0x37,0x30,0x20,
    0x3d,0x20,0x61,0x63,0x63,0x75,0x6d,0x75,0x6c,0x61,0x74,0x65,0x64,0x5f,0x63,0x6f,
    0x6c,0x6f,0x72,0x3b,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,
    0x61,0x74,0x34,0x20,0x5f,0x37,0x38,0x20,0x3d,0x20,0x5f,0x37,0x30,0x20,0x2b,0x20,
    0x28,0x74,0x65,0x78,0x2e,0x53,0x61,0x6d,0x70,0x6c,0x65,0x28,0x73,0x6d,0x70,0x2c,
    0x20,0x5f,0x35,0x39,0x29,0x2e,0x78,0x2e,0x78,0x78,0x78,0x78,0x20,0x2a,0x20,0x28,
    0x31,0x2e,0x30,0x66,0x20,0x2d,0x20,0x5f,0x37,0x30,0x2e,0x77,0x29,0x29,0x3b,0x0a,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x61,0x63,0x63,0x75,0x6d,0x75,0x6c,0x61,
    0x74,0x65,0x64,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x5f,0x37,0x38,0x3b,
    0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x63,0x75,0x72,0x72,0x65,0x6e,0x74,
    0x5f,0x70,0x6f,0x73,0x20,0x3d,0x20,0x5f,0x35,0x39,0x20,0x2b,0x20,0x5f,0x32,0x38,
    0x3b,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x69,0x66,0x20,0x28,0x5f,0x37,
    0x38,0x2e,0x77,0x20,0x3e,0x3d,0x20,0x31,0x2e,0x30,0x66,0x29,0x0a,0x20,0x20,0x20,
    0x20,0x20,0x20,0x20,0x20,0x7b,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x62,0x72,0x65,0x61,0x6b,0x3b,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x7d,0x0a,0x20,0x20,0x20,0x20,0x7d,0x0a,0x20,0x20,0x20,0x20,0x46,0x72,
    0x61,0x67,0x43,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x61,0x63,0x63,0x75,0x6d,0x75,
    0x6c,0x61,0x74,0x65,0x64,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x7d,0x0a,0x0a,
    0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,0x74,0x70,
    0x75,0x74,0x20,0x6d,0x61,0x69,0x6e,0x28,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,
    0x6f,0x73,0x73,0x5f,0x49,0x6e,0x70,0x75,0x74,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,
    0x69,0x6e,0x70,0x75,0x74,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x54,0x65,0x78,
    0x43,0x6f,0x6f,0x72,0x64,0x73,0x20,0x3d,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x69,
    0x6e,0x70,0x75,0x74,0x2e,0x54,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x73,0x3b,0x0a,
    0x20,0x20,0x20,0x20,0x66,0x72,0x61,0x67,0x5f,0x6d,0x61,0x69,0x6e,0x28,0x29,0x3b,
    0x0a,0x20,0x20,0x20,0x20,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,
    0x5f,0x4f,0x75,0x74,0x70,0x75,0x74,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,0x75,
    0x74,0x70,0x75,0x74,0x3b,0x0a,0x20,0x20,0x20,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,
    0x6f,0x75,0x74,0x70,0x75,0x74,0x2e,0x46,0x72,0x61,0x67,0x43,0x6f,0x6c,0x6f,0x72,
    0x20,0x3d,0x20,0x46,0x72,0x61,0x67,0x43,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x20,0x20,
    0x20,0x20,0x72,0x65,0x74,0x75,0x72,0x6e,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,
    0x75,0x74,0x70,0x75,0x74,0x3b,0x0a,0x7d,0x0a,0x00,
};
/*
    #include <metal_stdlib>
    #include <simd/simd.h>

    using namespace metal;

    struct main0_out
    {
        float3 TexCoords [[user(locn0)]];
        float4 gl_Position [[position]];
    };

    struct main0_in
    {
        float3 position [[attribute(0)]];
        float3 texCoords [[attribute(1)]];
    };

    vertex main0_out main0(main0_in in [[stage_in]])
    {
        main0_out out = {};
        out.TexCoords = in.texCoords;
        out.gl_Position = float4(in.position, 1.0);
        return out;
    }

*/
static const uint8_t vs_volume_source_metal_macos[446] = {
    0x23,0x69,0x6e,0x63,0x6c,0x75,0x64,0x65,0x20,0x3c,0x6d,0x65,0x74,0x61,0x6c,0x5f,
    0x73,0x74,0x64,0x6c,0x69,0x62,0x3e,0x0a,0x23,0x69,0x6e,0x63,0x6c,0x75,0x64,0x65,
    0x20,0x3c,0x73,0x69,0x6d,0x64,0x2f,0x73,0x69,0x6d,0x64,0x2e,0x68,0x3e,0x0a,0x0a,
    0x75,0x73,0x69,0x6e,0x67,0x20,0x6e,0x61,0x6d,0x65,0x73,0x70,0x61,0x63,0x65,0x20,
    0x6d,0x65,0x74,0x61,0x6c,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x6d,
    0x61,0x69,0x6e,0x30,0x5f,0x6f,0x75,0x74,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,
    0x6c,0x6f,0x61,0x74,0x33,0x20,0x54,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x73,0x20,
    0x5b,0x5b,0x75,0x73,0x65,0x72,0x28,0x6c,0x6f,0x63,0x6e,0x30,0x29,0x5d,0x5d,0x3b,
    0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x67,0x6c,0x5f,0x50,
    0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x5b,0x5b,0x70,0x6f,0x73,0x69,0x74,0x69,
    0x6f,0x6e,0x5d,0x5d,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,
    0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x69,0x6e,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,
    0x66,0x6c,0x6f,0x61,0x74,0x33,0x20,0x70,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,
    0x5b,0x5b,0x61,0x74,0x74,0x72,0x69,0x62,0x75,0x74,0x65,0x28,0x30,0x29,0x5d,0x5d,
    0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x33,0x20,0x74,0x65,0x78,
    0x43,0x6f,0x6f,0x72,0x64,0x73,0x20,0x5b,0x5b,0x61,0x74,0x74,0x72,0x69,0x62,0x75,
    0x74,0x65,0x28,0x31,0x29,0x5d,0x5d,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x76,0x65,0x72,
    0x74,0x65,0x78,0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x6f,0x75,0x74,0x20,0x6d,0x61,
    0x69,0x6e,0x30,0x28,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x69,0x6e,0x20,0x69,0x6e,0x20,
    0x5b,0x5b,0x73,0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x5d,0x5d,0x29,0x0a,0x7b,0x0a,
    0x20,0x20,0x20,0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x6f,0x75,0x74,0x20,0x6f,0x75,
    0x74,0x20,0x3d,0x20,0x7b,0x7d,0x3b,0x0a,0x20,0x20,0x20,0x20,0x6f,0x75,0x74,0x2e,
    0x54,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x73,0x20,0x3d,0x20,0x69,0x6e,0x2e,0x74,
    0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x73,0x3b,0x0a,0x20,0x20,0x20,0x20,0x6f,0x75,
    0x74,0x2e,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,
    0x66,0x6c,0x6f,0x61,0x74,0x34,0x28,0x69,0x6e,0x2e,0x70,0x6f,0x73,0x69,0x74,0x69,
    0x6f,0x6e,0x2c,0x20,0x31,0x2e,0x30,0x29,0x3b,0x0a,0x20,0x20,0x20,0x20,0x72,0x65,
    0x74,0x75,0x72,0x6e,0x20,0x6f,0x75,0x74,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
/*
    #include <metal_stdlib>
    #include <simd/simd.h>

    using namespace metal;

    struct fs_vol_params
    {
        float3 camera_pos;
    };

    struct main0_out
    {
        float4 FragColor [[color(0)]];
    };

    struct main0_in
    {
        float3 TexCoords [[user(locn0)]];
    };

    fragment main0_out main0(main0_in in [[stage_in]], constant fs_vol_params& _15 [[buffer(0)]], texture3d<float> tex [[texture(0)]], sampler smp [[sampler(0)]])
    {
        main0_out out = {};
        float3 _28 = fast::normalize(in.TexCoords - _15.camera_pos) * 0.00999999977648258209228515625;
        float4 accumulated_color = float4(0.0);
        float3 current_pos = in.TexCoords;
        for (int i = 0; i < 1000; i++)
        {
            float3 _59 = current_pos;
            float4 _70 = accumulated_color;
            float4 _78 = _70 + (float4(tex.sample(smp, _59).x) * (1.0 - _70.w));
            accumulated_color = _78;
            current_pos = _59 + _28;
            if (_78.w >= 1.0)
            {
                break;
            }
        }
        out.FragColor = accumulated_color;
        return out;
    }

*/
static const uint8_t fs_volume_source_metal_macos[995] = {
    0x23,0x69,0x6e,0x63,0x6c,0x75,0x64,0x65,0x20,0x3c,0x6d,0x65,0x74,0x61,0x6c,0x5f,
    0x73,0x74,0x64,0x6c,0x69,0x62,0x3e,0x0a,0x23,0x69,0x6e,0x63,0x6c,0x75,0x64,0x65,
    0x20,0x3c,0x73,0x69,0x6d,0x64,0x2f,0x73,0x69,0x6d,0x64,0x2e,0x68,0x3e,0x0a,0x0a,
    0x75,0x73,0x69,0x6e,0x67,0x20,0x6e,0x61,0x6d,0x65,0x73,0x70,0x61,0x63,0x65,0x20,
    0x6d,0x65,0x74,0x61,0x6c,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x66,
    0x73,0x5f,0x76,0x6f,0x6c,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x0a,0x7b,0x0a,0x20,
    0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x33,0x20,0x63,0x61,0x6d,0x65,0x72,0x61,
    0x5f,0x70,0x6f,0x73,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,
    0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x6f,0x75,0x74,0x0a,0x7b,0x0a,0x20,0x20,0x20,
    0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x46,0x72,0x61,0x67,0x43,0x6f,0x6c,0x6f,
    0x72,0x20,0x5b,0x5b,0x63,0x6f,0x6c,0x6f,0x72,0x28,0x30,0x29,0x5d,0x5d,0x3b,0x0a,
    0x7d,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x6d,0x61,0x69,0x6e,0x30,
    0x5f,0x69,0x6e,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x33,
    0x20,0x54,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x73,0x20,0x5b,0x5b,0x75,0x73,0x65,
    0x72,0x28,0x6c,0x6f,0x63,0x6e,0x30,0x29,0x5d,0x5d,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,
    0x66,0x72,0x61,0x67,0x6d,0x65,0x6e,0x74,0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x6f,
    0x75,0x74,0x20,0x6d,0x61,0x69,0x6e,0x30,0x28,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x69,
    0x6e,0x20,0x69,0x6e,0x20,0x5b,0x5b,0x73,0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x5d,
    0x5d,0x2c,0x20,0x63,0x6f,0x6e,0x73,0x74,0x61,0x6e,0x74,0x20,0x66,0x73,0x5f,0x76,
    0x6f,0x6c,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x26,0x20,0x5f,0x31,0x35,0x20,0x5b,
    0x5b,0x62,0x75,0x66,0x66,0x65,0x72,0x28,0x30,0x29,0x5d,0x5d,0x2c,0x20,0x74,0x65,
    0x78,0x74,0x75,0x72,0x65,0x33,0x64,0x3c,0x66,0x6c,0x6f,0x61,0x74,0x3e,0x20,0x74,
    0x65,0x78,0x20,0x5b,0x5b,0x74,0x65,0x78,0x74,0x75,0x72,0x65,0x28,0x30,0x29,0x5d,
    0x5d,0x2c,0x20,0x73,0x61,0x6d,0x70,0x6c,0x65,0x72,0x20,0x73,0x6d,0x70,0x20,0x5b,
    0x5b,0x73,0x61,0x6d,0x70,0x6c,0x65,0x72,0x28,0x30,0x29,0x5d,0x5d,0x29,0x0a,0x7b,
    0x0a,0x20,0x20,0x20,0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x6f,0x75,0x74,0x20,0x6f,
    0x75,0x74,0x20,0x3d,0x20,0x7b,0x7d,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,
    0x61,0x74,0x33,0x20,0x5f,0x32,0x38,0x20,0x3d,0x20,0x66,0x61,0x73,0x74,0x3a,0x3a,
    0x6e,0x6f,0x72,0x6d,0x61,0x6c,0x69,0x7a,0x65,0x28,0x69,0x6e,0x2e,0x54,0x65,0x78,
    0x43,0x6f,0x6f,0x72,0x64,0x73,0x20,0x2d,0x20,0x5f,0x31,0x35,0x2e,0x63,0x61,0x6d,
    0x65,0x72,0x61,0x5f,0x70,0x6f,0x73,0x29,0x20,0x2a,0x20,0x30,0x2e,0x30,0x30,0x39,
    0x39,0x39,0x39,0x39,0x39,0x39,0x37,0x37,0x36,0x34,0x38,0x32,0x35,0x38,0x32,0x30,
    0x39,0x32,0x32,0x38,0x35,0x31,0x35,0x36,0x32,0x35,0x3b,0x0a,0x20,0x20,0x20,0x20,
    0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x61,0x63,0x63,0x75,0x6d,0x75,0x6c,0x61,0x74,
    0x65,0x64,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x66,0x6c,0x6f,0x61,0x74,
    0x34,0x28,0x30,0x2e,0x30,0x29,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,
    0x74,0x33,0x20,0x63,0x75,0x72,0x72,0x65,0x6e,0x74,0x5f,0x70,0x6f,0x73,0x20,0x3d,
    0x20,0x69,0x6e,0x2e,0x54,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x73,0x3b,0x0a,0x20,
    0x20,0x20,0x20,0x66,0x6f,0x72,0x20,0x28,0x69,0x6e,0x74,0x20,0x69,0x20,0x3d,0x20,
    0x30,0x3b,0x20,0x69,0x20,0x3c,0x20,0x31,0x30,0x30,0x30,0x3b,0x20,0x69,0x2b,0x2b,
    0x29,0x0a,0x20,0x20,0x20,0x20,0x7b,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x66,0x6c,0x6f,0x61,0x74,0x33,0x20,0x5f,0x35,0x39,0x20,0x3d,0x20,0x63,0x75,0x72,
    0x72,0x65,0x6e,0x74,0x5f,0x70,0x6f,0x73,0x3b,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x5f,0x37,0x30,0x20,0x3d,0x20,0x61,
    0x63,0x63,0x75,0x6d,0x75,0x6c,0x61,0x74,0x65,0x64,0x5f,0x63,0x6f,0x6c,0x6f,0x72,
    0x3b,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,
    0x20,0x5f,0x37,0x38,0x20,0x3d,0x20,0x5f,0x37,0x30,0x20,0x2b,0x20,0x28,0x66,0x6c,
    0x6f,0x61,0x74,0x34,0x28,0x74,0x65,0x78,0x2e,0x73,0x61,0x6d,0x70,0x6c,0x65,0x28,
    0x73,0x6d,0x70,0x2c,0x20,0x5f,0x35,0x39,0x29,0x2e,0x78,0x29,0x20,0x2a,0x20,0x28,
    0x31,0x2e,0x30,0x20,0x2d,0x20,0x5f,0x37,0x30,0x2e,0x77,0x29,0x29,0x3b,0x0a,0x20,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x61,0x63,0x63,0x75,0x6d,0x75,0x6c,0x61,0x74,
    0x65,0x64,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x5f,0x37,0x38,0x3b,0x0a,
    0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x63,0x75,0x72,0x72,0x65,0x6e,0x74,0x5f,
    0x70,0x6f,0x73,0x20,0x3d,0x20,0x5f,0x35,0x39,0x20,0x2b,0x20,0x5f,0x32,0x38,0x3b,
    0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x69,0x66,0x20,0x28,0x5f,0x37,0x38,
    0x2e,0x77,0x20,0x3e,0x3d,0x20,0x31,0x2e,0x30,0x29,0x0a,0x20,0x20,0x20,0x20,0x20,
    0x20,0x20,0x20,0x7b,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x20,0x62,0x72,0x65,0x61,0x6b,0x3b,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
    0x7d,0x0a,0x20,0x20,0x20,0x20,0x7d,0x0a,0x20,0x20,0x20,0x20,0x6f,0x75,0x74,0x2e,
    0x46,0x72,0x61,0x67,0x43,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x61,0x63,0x63,0x75,
    0x6d,0x75,0x6c,0x61,0x74,0x65,0x64,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x20,
    0x20,0x20,0x20,0x72,0x65,0x74,0x75,0x72,0x6e,0x20,0x6f,0x75,0x74,0x3b,0x0a,0x7d,
    0x0a,0x0a,0x00,
};
static inline const sg_shader_desc* volume_shader_desc(sg_backend backend) {
    if (backend == SG_BACKEND_GLCORE) {
        static sg_shader_desc desc;
        static bool valid;
        if (!valid) {
            valid = true;
            desc.attrs[0].name = "position";
            desc.attrs[1].name = "texCoords";
            desc.vs.source = (const char*)vs_volume_source_glsl430;
            desc.vs.entry = "main";
            desc.fs.source = (const char*)fs_volume_source_glsl430;
            desc.fs.entry = "main";
            desc.fs.uniform_blocks[0].size = 16;
            desc.fs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
            desc.fs.uniform_blocks[0].uniforms[0].name = "fs_vol_params";
            desc.fs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
            desc.fs.uniform_blocks[0].uniforms[0].array_count = 1;
            desc.fs.images[0].used = true;
            desc.fs.images[0].multisampled = false;
            desc.fs.images[0].image_type = SG_IMAGETYPE_3D;
            desc.fs.images[0].sample_type = SG_IMAGESAMPLETYPE_FLOAT;
            desc.fs.samplers[0].used = true;
            desc.fs.samplers[0].sampler_type = SG_SAMPLERTYPE_FILTERING;
            desc.fs.image_sampler_pairs[0].used = true;
            desc.fs.image_sampler_pairs[0].image_slot = 0;
            desc.fs.image_sampler_pairs[0].sampler_slot = 0;
            desc.fs.image_sampler_pairs[0].glsl_name = "tex_smp";
            desc.label = "volume_shader";
        }
        return &desc;
    }
    if (backend == SG_BACKEND_D3D11) {
        static sg_shader_desc desc;
        static bool valid;
        if (!valid) {
            valid = true;
            desc.attrs[0].sem_name = "TEXCOORD";
            desc.attrs[0].sem_index = 0;
            desc.attrs[1].sem_name = "TEXCOORD";
            desc.attrs[1].sem_index = 1;
            desc.vs.source = (const char*)vs_volume_source_hlsl5;
            desc.vs.d3d11_target = "vs_5_0";
            desc.vs.entry = "main";
            desc.fs.source = (const char*)fs_volume_source_hlsl5;
            desc.fs.d3d11_target = "ps_5_0";
            desc.fs.entry = "main";
            desc.fs.uniform_blocks[0].size = 16;
            desc.fs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
            desc.fs.images[0].used = true;
            desc.fs.images[0].multisampled = false;
            desc.fs.images[0].image_type = SG_IMAGETYPE_3D;
            desc.fs.images[0].sample_type = SG_IMAGESAMPLETYPE_FLOAT;
            desc.fs.samplers[0].used = true;
            desc.fs.samplers[0].sampler_type = SG_SAMPLERTYPE_FILTERING;
            desc.fs.image_sampler_pairs[0].used = true;
            desc.fs.image_sampler_pairs[0].image_slot = 0;
            desc.fs.image_sampler_pairs[0].sampler_slot = 0;
            desc.label = "volume_shader";
        }
        return &desc;
    }
    if (backend == SG_BACKEND_METAL_MACOS) {
        static sg_shader_desc desc;
        static bool valid;
        if (!valid) {
            valid = true;
            desc.vs.source = (const char*)vs_volume_source_metal_macos;
            desc.vs.entry = "main0";
            desc.fs.source = (const char*)fs_volume_source_metal_macos;
            desc.fs.entry = "main0";
            desc.fs.uniform_blocks[0].size = 16;
            desc.fs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
            desc.fs.images[0].used = true;
            desc.fs.images[0].multisampled = false;
            desc.fs.images[0].image_type = SG_IMAGETYPE_3D;
            desc.fs.images[0].sample_type = SG_IMAGESAMPLETYPE_FLOAT;
            desc.fs.samplers[0].used = true;
            desc.fs.samplers[0].sampler_type = SG_SAMPLERTYPE_FILTERING;
            desc.fs.image_sampler_pairs[0].used = true;
            desc.fs.image_sampler_pairs[0].image_slot = 0;
            desc.fs.image_sampler_pairs[0].sampler_slot = 0;
            desc.label = "volume_shader";
        }
        return &desc;
    }
    return 0;
}
