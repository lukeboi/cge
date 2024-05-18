#include <assert.h>
#include <stdbool.h>
#include <string.h>

#define HANDMADE_MATH_IMPLEMENTATION
#define HANDMADE_MATH_NO_SSE
#include "lib/hmm/HandmadeMath.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"

#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"

#define SOKOL_IMPL
#define SOKOL_METAL
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_log.h"
#include "sokol_glue.h"
#include "sokol_imgui.h"

#include "lib/stb/stb_image.h"

#include "shader/cube.glsl.h"
// #include "shader/volume.glsl.h"
// #include "shader/phong.glsl.h"

// ECS
// TODO this is all ugly
#define NUM_COMPONENTS 32

// Define component structs
typedef struct {
    hmm_vec3 position;
    hmm_vec3 rotation; // Euler angles
    hmm_mat4 _transform; // Transform of the object based on position and rotation.
    // When they disagree, the pos/rot always takes precedence and this tranform is just used for per-frame calculations
} transform_c_t;

typedef struct {
    float _vertices[10000 * 3 * 8]; // Random big number, *should* be 10k verts, TODO malloc
    uint16_t _indices[10000 * 8]; // Index values, random big number, TODO malloc
    unsigned _verticies_size;
    unsigned _indices_size;
    unsigned face_count;
    sg_buffer vbuf;
    sg_buffer ibuf;
    sg_bindings binding;
} mesh_c_t;

typedef struct {
    uint8_t _volume[100 * 100 * 100]; // All volumes come in increments of 500x500x500 // TODO malloc
    hmm_vec3 position; // Position of the volume
} volume_c_t;

// ECS Global struct
static struct {
    // Transforms of each object
    bool transforms_valid[NUM_COMPONENTS];
    transform_c_t transforms[NUM_COMPONENTS];
    
    // Object mesh buffer
    // TODO this should be malloced
    bool mesh_valid[NUM_COMPONENTS];
    mesh_c_t meshes[NUM_COMPONENTS];

    // 3D volume buffer
    // TODO this should be malloced
    bool volume_valid[NUM_COMPONENTS];
    volume_c_t volumes[NUM_COMPONENTS];
} ecs = {
    .transforms_valid   = { [0 ... NUM_COMPONENTS-1] = false },
    .mesh_valid         = { [0 ... NUM_COMPONENTS-1] = false },
    .volume_valid       = { [0 ... NUM_COMPONENTS-1] = false }
};

// Component-specific functions
void update_transform(int index, transform_c_t *data) {
    ecs.transforms_valid[index] = true;
    memcpy(&ecs.transforms[index], data, sizeof(transform_c_t));
}

// Global state
static struct {
    ImFont* default_font;
    ImFont* main_font;
    bool show_imgui_demo;
} gui;

static struct {
    float cam_rx, cam_ry; // Latlong-esque camera rotations
    float cam_fov;
    bool cam_drift;
    hmm_vec3 cam_pos;
    sg_pipeline pip;
    sg_bindings bind;
    sg_pipeline mesh_pip;
    sg_bindings mesh_bind;
    sg_pass_action pass_action;
    bool key_down[256]; // keeps track of keypresses
} state = {
    .cam_fov = 60.0f,
    .cam_drift = false
};

fastObjMesh* mesh;

// Sokol init
void init(void) {
    memset(state.key_down, 0, sizeof(state.key_down)); // set key down to zero

    for (int i = 0; i < 5; i++) {
        update_transform((float)i, &(transform_c_t){
            .position = HMM_Vec3((float)i * 3.1f - 6.0f, (float)i * 1.1f - 5.0f, -10.0f),
            .rotation = HMM_Vec3((float)i * 20.0f, (float)i * 20.0f, 0.0f)
        });

        printf("%d\n", i);
    }
    // update_transform(0, &(transform_c_t){
    //     .position = HMM_Vec3(0.0f, 0.0f, 0.0f)
    // });
    // update_transform(1, &(transform_c_t){
    //     .position = HMM_Vec3(-2.0f, 0.0f, 0.0f)
    // });

    // Camera set inital position
    state.cam_pos = HMM_Vec3(0.0f, 1.5f, 20.0f);

    sg_setup(&(sg_desc){
        .context = sapp_sgcontext(),
        .logger.func = slog_func,
    });

    // simgui_setup(&(simgui_desc_t){ 0 });
    simgui_setup(&(simgui_desc_t) {
        // .dpi_scale = sapp_dpi_scale(),
        .no_default_font = true,
    });

    // Load a custom font in imgui
    // TODO: Make this it's own function
    // TODO: Fix antialiasing/appears blurry on text
    ImGuiIO *io = igGetIO();
    gui.main_font = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "assets/NotoSans-Regular.ttf", 16, NULL, NULL);
    // gui.main_font = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "lib/cimgui/imgui/misc/fonts/Roboto-Medium.ttf", 16, NULL, NULL); // This works but is also blurry
    
    // Create font atlas
    unsigned char* font_pixels;
    int font_width, font_height;
    int bytes_per_pixel;
    ImFontAtlas_GetTexDataAsRGBA32(io->Fonts, &font_pixels, &font_width, &font_height, &bytes_per_pixel);
    sg_image_desc font_img_desc;
    _simgui_clear(&font_img_desc, sizeof(font_img_desc));
    font_img_desc.width = font_width;
    font_img_desc.height = font_height;
    font_img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    font_img_desc.data.subimage[0][0].ptr = font_pixels;
    font_img_desc.data.subimage[0][0].size = (size_t)(font_width * font_height) * sizeof(uint32_t);
    font_img_desc.label = "sokol-imgui-font-image";
    _simgui.font_img = sg_make_image(&font_img_desc);

    // Set atlas to right texture
    simgui_image_desc_t img_desc;
    _simgui_clear(&img_desc, sizeof(img_desc));
    img_desc.image = _simgui.font_img;
    img_desc.sampler = _simgui.font_smp;
    _simgui.default_font = simgui_make_image(&img_desc);
    io->Fonts->TexID = simgui_imtextureid(_simgui.default_font);

    mesh = fast_obj_read("assets/monkey.obj");
    int face_count = mesh->face_count;

    for(int i = 0; i < mesh->index_count; i++) {
        printf("= %d\n", mesh->indices[i].p);
        ecs.meshes[0]._indices[i] = mesh->indices[i].p;
    }
    ecs.meshes[0]._indices_size = mesh->index_count;
    ecs.meshes[0].face_count = mesh->face_count;

    {
        unsigned int i = 0; // Keep i def outside loop to use it to get size
        // for (i = 0; i < mesh->face_count * 3; ++i) {
        //     fastObjIndex vertex = mesh->indices[i];

        //     // unsigned int v_pos = vertex.p * 3;
        //     unsigned int v_pos = vertex.p;
        //     unsigned int n_pos = vertex.n * 3;
        //     unsigned int t_pos = vertex.t * 2;

        //     // // From opengl tutorial
        //     // unsigned int pos = i * 8;

        //     // memcpy(ecs.meshes[0].vertices + pos, mesh->positions + v_pos, 3 * sizeof(float));
        //     // memcpy(ecs.meshes[0].vertices + pos + 3, mesh->normals + n_pos, 3 * sizeof(float));
        //     // memcpy(ecs.meshes[0].vertices + pos + 6, mesh->texcoords + t_pos, 2 * sizeof(float));

        //     // From cube shader
        //     unsigned int pos = i * 7;
        //     float red_rgba[] = { 1.0f, 0.0f, 0.0f, 1.0f };
        //     float testval[] = { 0.8f };
        //     memcpy(ecs.meshes[0].vertices + pos, mesh->positions + v_pos, 3 * sizeof(float)); // XYZ point
        //     memcpy(ecs.meshes[0].vertices + pos + 3, red_rgba, 4 * sizeof(float)); // Color, just make it all red
        //     // memcpy(ecs.meshes[0].vertices + pos, testval, 1 * sizeof(float)); // Color, just make it all red
        //     // printf("= pos %d\n", pos);
        // }

        for (int j = 0; j < mesh->position_count * 3 + 3; j++) {
            printf("%.2f\n", mesh->positions[j]);
        }

        for (i = 0; i < mesh->position_count; i++) {
            printf("i is %d\n", i);
            unsigned int pos = i * 7;
            float red_rgba[] = { 1.0f, 0.0f, 0.0f, 1.0f };
            // float v_pos[] = mesh->positions[pos];
            ecs.meshes[0]._vertices[pos] = mesh->positions[i * 3];
            ecs.meshes[0]._vertices[pos + 1] = mesh->positions[i * 3 + 1];
            ecs.meshes[0]._vertices[pos + 2] = mesh->positions[i * 3 + 2];
            // memcpy(ecs.meshes[0].vertices + pos, mesh->positions + pos, 3 * sizeof(float)); // XYZ point
            memcpy(ecs.meshes[0]._vertices + pos + 3, red_rgba, 4 * sizeof(float)); // Color, just make it all red
        }
        
        ecs.mesh_valid[0] = true;

        for (int k = 0; k < i * 7; k++) {
            if (k % 7 == 0) {
                printf("\n");
            }
            if (ecs.meshes[0]._vertices[k] >= 0.0f)
                printf(" "); 
            printf("%.2f, ", ecs.meshes[0]._vertices[k]);
        }

        ecs.meshes[0].vbuf = sg_make_buffer(&(sg_buffer_desc){
            .data = (sg_range){ &ecs.meshes[0]._vertices, i * 7 * sizeof(float) },
            .label = "mesh-vertices"
        });

        ecs.meshes[0].ibuf = sg_make_buffer(&(sg_buffer_desc){
            .type = SG_BUFFERTYPE_INDEXBUFFER,
            .data = (sg_range){ &ecs.meshes[0]._indices, ecs.meshes[0]._indices_size * sizeof(uint16_t) },
            .label = "mesh-indices"
        });
    }
    
    // From opengl tutorial
    // | pos     | | norm    | | tex |
    // [f] [f] [f] [f] [f] [f] [f] [f]
    // |-----16b-----| |-----16b-----|
    // Thus stride is 32b

    // From basic cube shader below
    // | pos     | | rgba        |
    // [f] [f] [f] [f] [f] [f] [f]
    // |-----16b-----| |---12b---|
    // Thus stride is 28b
    
    // This is an obj loader which is compatable with the cube.glsl shader:

    // Clear screen values
    sg_pass_action pass_action = {
        .colors[0] = { .load_action = SG_LOADACTION_CLEAR, .clear_value = { 0.25f, 0.5f, 0.75f, 1.0f } }
    };

    /* cube vertex buffer */
    float vertices[] = {
        -1.0, -1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
         1.0, -1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
         1.0,  1.0, -1.0,   1.0, 0.0, 0.0, 1.0,
        -1.0,  1.0, -1.0,   1.0, 0.0, 0.0, 1.0,

        -1.0, -1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
         1.0, -1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
         1.0,  1.0,  1.0,   0.0, 1.0, 0.0, 1.0,
        -1.0,  1.0,  1.0,   0.0, 1.0, 0.0, 1.0,

        -1.0, -1.0, -1.0,   0.0, 0.0, 1.0, 1.0,
        -1.0,  1.0, -1.0,   0.0, 0.0, 1.0, 1.0,
        -1.0,  1.0,  1.0,   0.0, 0.0, 1.0, 1.0,
        -1.0, -1.0,  1.0,   0.0, 0.0, 1.0, 1.0,

        1.0, -1.0, -1.0,    1.0, 0.5, 0.0, 1.0,
        1.0,  1.0, -1.0,    1.0, 0.5, 0.0, 1.0,
        1.0,  1.0,  1.0,    1.0, 0.5, 0.0, 1.0,
        1.0, -1.0,  1.0,    1.0, 0.5, 0.0, 1.0,

        -1.0, -1.0, -1.0,   0.0, 0.5, 1.0, 1.0,
        -1.0, -1.0,  1.0,   0.0, 0.5, 1.0, 1.0,
         1.0, -1.0,  1.0,   0.0, 0.5, 1.0, 1.0,
         1.0, -1.0, -1.0,   0.0, 0.5, 1.0, 1.0,

        -1.0,  1.0, -1.0,   1.0, 0.0, 0.5, 1.0,
        -1.0,  1.0,  1.0,   1.0, 0.0, 0.5, 1.0,
         1.0,  1.0,  1.0,   1.0, 0.0, 0.5, 1.0,
         1.0,  1.0, -1.0,   1.0, 0.0, 0.5, 1.0
    };
    sg_buffer vbuf = sg_make_buffer(&(sg_buffer_desc){
        .data = SG_RANGE(vertices),
        .label = "cube-vertices"
    });

    /* create an index buffer for the cube */
    uint16_t indices[] = {
        0, 1, 2,  0, 2, 3,
        6, 5, 4,  7, 6, 4,
        8, 9, 10,  8, 10, 11,
        14, 13, 12,  15, 14, 12,
        16, 17, 18,  16, 18, 19,
        22, 21, 20,  23, 22, 20
    };
    sg_buffer ibuf = sg_make_buffer(&(sg_buffer_desc){
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .data = SG_RANGE(indices),
        .label = "cube-indices"
    });

    /* create shader */
    sg_shader shd = sg_make_shader(cube_shader_desc(sg_query_backend()));

    // /* create pipeline object */
    state.pip = sg_make_pipeline(&(sg_pipeline_desc) {
        .layout = {
            /* test to provide buffer stride, but no attr offsets */
            .buffers[0].stride = 28,
            .attrs = {
                [ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3,
                [ATTR_vs_color0].format   = SG_VERTEXFORMAT_FLOAT4
            }
        },
        .shader = shd,
        .index_type = SG_INDEXTYPE_UINT16,
        .cull_mode = SG_CULLMODE_BACK,
        .depth = {
            .write_enabled = true,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
        },
        .label = "cube-pipeline"
    });

    // set first volume to random
    ecs.volume_valid[0] = true;
    for (int i = 0; i < 100 * 100 * 100; i++) {
        ecs.volumes[0]._volume[i] = 128;
    }

    // sg_shader phong_shd = sg_make_shader(phong_shader_desc(sg_query_backend()));
    // state.mesh_pip = sg_make_pipeline(&(sg_pipeline_desc){
    //     .shader = phong_shd,
    //     /* if the vertex layout doesn't have gaps, don't need to provide strides and offsets */
    //     .layout = {
    //         .attrs = {
    //             [ATTR_vs_a_pos].format = SG_VERTEXFORMAT_FLOAT3,
    //             [ATTR_vs_a_tex_coords] = {
    //                 .format = SG_VERTEXFORMAT_FLOAT2,
    //                 .offset = 24
    //             }
    //         },
    //         .buffers[0].stride = 32
    //     },
    //     .depth = {
    //         .write_enabled = true,
    //         .compare = SG_COMPAREFUNC_LESS_EQUAL,
    //     },
    //     .label = "object-pipeline"
    // });

    // sg_buffer mesh_buffer = sg_make_buffer(&(sg_buffer_desc){
    //     .size = face_count * 3 * 8 * sizeof(float),
    //     .content = ecs.meshes[0],
    //     .label = "mesh-vertices"
    // });
    // state.mesh_bind.vertex_buffers[0] = mesh_buffer;

    /* setup resource bindings */
    state.bind = (sg_bindings) {
        .vertex_buffers[0] = vbuf,
        .index_buffer = ibuf
    };

    ecs.meshes[0].binding = (sg_bindings) {
        .vertex_buffers[0] = ecs.meshes[0].vbuf,
        .index_buffer = ecs.meshes[0].ibuf,
    };
}

void camera_move(float dt) {
    // if W key is down, move camera position forward
    const float camera_speed = 0.1f * dt;
    
    // Calculate forward direction based on camera's yaw and pitch
    hmm_vec3 forward = HMM_Vec3(
        cosf(HMM_ToRadians(state.cam_rx)) * sinf(HMM_ToRadians(state.cam_ry)),
        -sinf(HMM_ToRadians(state.cam_rx)),
        -cosf(HMM_ToRadians(state.cam_rx)) * cosf(HMM_ToRadians(state.cam_ry))
    );

    // Calculate right direction based on camera's yaw
    hmm_vec3 right = HMM_Vec3(
        cosf(HMM_ToRadians(state.cam_ry)),
        0.0f,
        sinf(HMM_ToRadians(state.cam_ry))
    );

    printf("forward %.2f, %.2f, %.2f | right %.2f, %.2f, %.2f\n", forward.X, forward.Y, forward.Z, right.X, right.Y, right.Z);

    // Move forward
    if (state.key_down[SAPP_KEYCODE_W]) {
        state.cam_pos = HMM_AddVec3(state.cam_pos, HMM_MultiplyVec3f(forward, camera_speed));
    }
    // Move backward
    if (state.key_down[SAPP_KEYCODE_S]) {
        state.cam_pos = HMM_SubtractVec3(state.cam_pos, HMM_MultiplyVec3f(forward, camera_speed));
    }
    // Move left
    if (state.key_down[SAPP_KEYCODE_A]) {
        state.cam_pos = HMM_SubtractVec3(state.cam_pos, HMM_MultiplyVec3f(right, camera_speed));
    }
    // Move right
    if (state.key_down[SAPP_KEYCODE_D]) {
        state.cam_pos = HMM_AddVec3(state.cam_pos, HMM_MultiplyVec3f(right, camera_speed));
    }

    // const float camera_speed = 0.1f; // Adjust speed as needed
    // if (state.key_down[SAPP_KEYCODE_W]) {
    //     hmm_vec3 forward = HMM_Vec3(0.0f, 0.0f, -1.0f); // Forward direction (negative Z)
    //     state.cam_pos = HMM_AddVec3(state.cam_pos, HMM_MultiplyVec3f(forward, camera_speed));
    // }
    // if (state.key_down[SAPP_KEYCODE_S]) {
    //     hmm_vec3 backward = HMM_Vec3(0.0f, 0.0f, 1.0f); // Backward direction (positive Z)
    //     state.cam_pos = HMM_AddVec3(state.cam_pos, HMM_MultiplyVec3f(backward, camera_speed));
    // }
    // if (state.key_down[SAPP_KEYCODE_A]) {
    //     hmm_vec3 left = HMM_Vec3(-1.0f, 0.0f, 0.0f); // Left direction (negative X)
    //     state.cam_pos = HMM_AddVec3(state.cam_pos, HMM_MultiplyVec3f(left, camera_speed));
    // }
    // if (state.key_down[SAPP_KEYCODE_D]) {
    //     hmm_vec3 right = HMM_Vec3(1.0f, 0.0f, 0.0f); // Right direction (positive X)
    //     state.cam_pos = HMM_AddVec3(state.cam_pos, HMM_MultiplyVec3f(right, camera_speed));
    // }
}

void frame(void) {
    // GUI Rendering
    simgui_new_frame(&(simgui_frame_desc_t){
        .width = sapp_width(),
        .height = sapp_height(),
        .delta_time = sapp_frame_duration(),
        .dpi_scale = sapp_dpi_scale(),
    });

    // UI Code
    // igPushFont(gui.main_font);
    igSetNextWindowPos((ImVec2){10,10}, ImGuiCond_Once, (ImVec2){0,0});
    igSetNextWindowSize((ImVec2){600, 300}, ImGuiCond_Once);
    igBegin("Tippelations!", 0, ImGuiWindowFlags_None);
    igText("Wagon Engine");
    igText("FPS %.1f\n", 1. / sapp_frame_duration());
    igText("Camera x, y, z (%.2f, %.2f, %.2f)", state.cam_pos.X, state.cam_pos.Y, state.cam_pos.Z);
    igText("Camera Rx, Ry (%.2f, %.2f)", state.cam_rx, state.cam_ry);
    igText("Camera FOV %.1f", state.cam_fov);
    igDragFloat("Camera X", &state.cam_pos.X, 0.1f, -35.0f, 35.0f, "%.1f", 0);
    igDragFloat("Camera Y", &state.cam_pos.Y, 0.1f, -35.0f, 35.0f, "%.1f", 0);
    igDragFloat("Camera Z", &state.cam_pos.Z, 0.1f, -35.0f, 35.0f, "%.1f", 0);
    igSliderFloat("Camera Rx", &state.cam_rx, -90.0f, 90.0f, "%.1f", 0);
    igSliderFloat("Camera Ry", &state.cam_ry, -180.0f, 180.0f, "%.1f", 0);
    igSliderFloat("Camera FOV", &state.cam_fov, 10.0f, 100.0f, "%.1f", 0);
    igCheckbox("Camera Rotation Drift", &state.cam_drift);
    igCheckbox("Show DearImgui demo window", &gui.show_imgui_demo);
    if (gui.show_imgui_demo) igShowDemoWindow(0);
    igEnd();


    igSetNextWindowPos((ImVec2){300,20}, ImGuiCond_Once, (ImVec2){0,0});
    igSetNextWindowSize((ImVec2){600, 300}, ImGuiCond_Once);
    igBegin("2d scroll slice view", 0, ImGuiWindowFlags_None);
    igSliderInt("Slice", &state.cam_rx, 0, 10, "%d", 0);
    igText("h");
    igEnd();

    const float w = sapp_widthf();
    const float h = sapp_heightf();
    const float t = (float)(sapp_frame_duration() * 60.0);

    // Camera movement
    camera_move(t);

    if (state.cam_drift) {
        state.cam_rx += 1.0f * t;
        state.cam_ry += 0.5f * t;
        if (state.cam_rx >= 90.0f) {
            state.cam_rx = -90.0f;
        }
        if (state.cam_rx >= 180.0f) {
            state.cam_rx = -180.0f;
        }
    }
    hmm_mat4 rxm = HMM_Rotate(state.cam_rx, HMM_Vec3(1.0f, 0.0f, 0.0f));
    hmm_mat4 rym = HMM_Rotate(state.cam_ry, HMM_Vec3(0.0f, 1.0f, 0.0f));

    hmm_mat4 proj = HMM_Perspective(state.cam_fov, w / h, 0.01f, 1000.0f);
    proj = HMM_MultiplyMat4(proj, rxm);
    proj = HMM_MultiplyMat4(proj, rym);
    // hmm_mat4 view = HMM_LookAt(HMM_Vec3(0.0f, 1.5f, 20.0f), HMM_Vec3(0.0f, 0.0f, 0.0f), HMM_Vec3(0.0f, 1.0f, 0.0f));
    // hmm_mat4 view = HMM_LookAt(state.cam_pos, HMM_Vec3(0.0f, 0.0f, 0.0f), HMM_Vec3(0.0f, 1.0f, 0.0f));
    // view = HMM_AddMat4(HMM_Translate(state.cam_pos), view);

    // Camera position is multiplied by -1 because this is camera position and we offset everything else by this position
    hmm_mat4 view = HMM_Translate(HMM_MultiplyVec3f(state.cam_pos, -1.0f));
    hmm_mat4 view_proj = HMM_MultiplyMat4(proj, view);

    sg_begin_default_pass(&state.pass_action, (int)w, (int)h);
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);

    // Calculate the model view projection matrix for each transform
    for (int i = 0; i < NUM_COMPONENTS; i++) {
        transform_c_t *transform = &ecs.transforms[i];
        if (ecs.transforms_valid[i]) {
            ecs.transforms[i].rotation.X += 1.0f * t;

            hmm_mat4 model = HMM_Translate(transform->position);
            // model = HMM_MultiplyMat4(model, rxm);
            // model = HMM_MultiplyMat4(model, rym);

            // Apply individual rotations
            hmm_mat4 pitch = HMM_Rotate(transform->rotation.X, HMM_Vec3(1.0f, 0.0f, 0.0f));
            hmm_mat4 yaw = HMM_Rotate(transform->rotation.Y, HMM_Vec3(0.0f, 1.0f, 0.0f));
            hmm_mat4 roll = HMM_Rotate(transform->rotation.Z, HMM_Vec3(0.0f, 0.0f, 1.0f));
            model = HMM_MultiplyMat4(model, pitch);
            model = HMM_MultiplyMat4(model, yaw);
            model = HMM_MultiplyMat4(model, roll);

            // model is now a 4x4 transformation matrix of the model's location
            // Save this transform for rendering in the next for loop
            transform->_transform = model;
        }
    }
    
    // Optional per-entity rendering of a cube mesh at the transform for debugging
    for (int i = 0; i < NUM_COMPONENTS; i++) {
        if (ecs.transforms_valid[i]) {
            vs_params_t vs_params;
            vs_params.mvp = HMM_MultiplyMat4(view_proj, ecs.transforms[i]._transform);
            sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));
            sg_draw(0, 36, 1);
        }
    }

    // Render each mesh if it exists
    for (int i = 0; i < NUM_COMPONENTS; i++) {
        if (ecs.transforms_valid[i] && ecs.mesh_valid[i]) {
            sg_apply_bindings(&ecs.meshes[i].binding);
            transform_c_t *transform = &ecs.transforms[i];
            vs_params_t vs_params;
            vs_params.mvp = HMM_MultiplyMat4(view_proj, ecs.transforms[i]._transform);
            sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));
            sg_draw(0, ecs.meshes[i].face_count * 3, 1);
        }
    }

    simgui_render();
    sg_end_pass();
    sg_commit();
}

void cleanup(void) {
    fast_obj_destroy(mesh);

    simgui_shutdown();
    sg_shutdown();
}

// Handle camera orbit
void camera_event(const sapp_event* ev) {
    // https://github.com/floooh/sokol-samples/blob/7b85538bb974eecb12027db9b5e66f274797461a/libs/util/camera.h#L112
    switch (ev->type) {
        case SAPP_EVENTTYPE_MOUSE_DOWN:
            if (ev->mouse_button == SAPP_MOUSEBUTTON_LEFT) {
                sapp_lock_mouse(true);
            }
            break;
        case SAPP_EVENTTYPE_MOUSE_UP:
            if (ev->mouse_button == SAPP_MOUSEBUTTON_LEFT) {
                sapp_lock_mouse(false);
            }
            break;
        // case SAPP_EVENTTYPE_MOUSE_SCROLL: // TODO support scrolling
        //     cam_zoom(cam, ev->scroll_y * 0.5f);
        //     break;
        case SAPP_EVENTTYPE_MOUSE_MOVE:
            if (sapp_mouse_locked()) {
                state.cam_ry += ev->mouse_dx * 0.1f; // left right motion
                state.cam_rx += ev->mouse_dy * 0.1f; // up down motion
                if (state.cam_rx > 90.0f) state.cam_rx = 90.0f;
                if (state.cam_rx < -90.0f) state.cam_rx = -90.0f;
            }
            break;
        default:
            break;
    }
}

static void event(const sapp_event* ev) {
    if(!simgui_handle_event(ev)) {
        camera_event(ev);
    }
    
    // Log keypresses
    if (ev->type == SAPP_EVENTTYPE_KEY_DOWN) state.key_down[ev->key_code] = true;
    if (ev->type == SAPP_EVENTTYPE_KEY_UP) state.key_down[ev->key_code] = false;
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event,
        .width = 1080,
        .height = 720,
        .sample_count = 4,
        .window_title = "Wagon Engine",
        .icon.sokol_default = true,
        .logger.func = slog_func,
    };
}
