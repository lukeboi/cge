#include <assert.h>
#include <stdbool.h>

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

#include "shader/cube.glsl.h"
#include "shader/phong.glsl.h"

// ECS
// TODO this is all ugly
#define NUM_COMPONENTS 128

// Define component structs
typedef struct {
    hmm_vec3 position;
    hmm_vec3 rotation; // Euler angles
} transform_c_t;

// ECS Global struct
static struct {
    // Transforms of each object
    bool transforms_valid[NUM_COMPONENTS];
    transform_c_t transforms[NUM_COMPONENTS];
    
    // Object mesh buffer
    // TODO this should be malloced
    bool mesh_valid[NUM_COMPONENTS];
    float meshes[NUM_COMPONENTS][10000 * 3 * 8]; // Random big number, *should* be 10k verts
} ecs = {
    .transforms_valid = {
        [0 ... NUM_COMPONENTS-1] = false
    },
    .mesh_valid = {
        [0 ... NUM_COMPONENTS-1] = false
    }
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
} gui;

static struct {
    float rx, ry;
    sg_pipeline pip;
    sg_bindings bind;
    sg_pipeline mesh_pip;
    sg_bindings mesh_bind;
    sg_pass_action pass_action;
} state;

fastObjMesh* mesh;

// Sokol init
void init(void) {
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
    // io->FontGlobalScale = 0.1f;

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


    mesh = fast_obj_read("assets/cube.obj");

    // int idx = 0;

    // for (int i = 0; i < mesh->group_count; i++) {
    //     fastObjGroup group = mesh->groups[i];
    //     for (int j = 0; j < group.face_count; j++) {
    //         int num_verticies = mesh->face_vertices[group.face_offset + j];
    //         for (int k = 0; k < num_verticies; k++) {
    //             fastObjIndex mi = mesh->indices[group.index_offset + idx];
    //             printf("%d\n", mi.p);
    //             // m->positions[3 * mi.p + 0];
    //             // idx++;
    //         }
    //     }
    // }

    int face_count = mesh->face_count;

    for (unsigned int i = 0; i < mesh->face_count * 3; ++i) {
        fastObjIndex vertex = mesh->indices[i];

        unsigned int pos = i * 8;
        unsigned int v_pos = vertex.p * 3;
        unsigned int n_pos = vertex.n * 3;
        unsigned int t_pos = vertex.t * 2;

        memcpy(ecs.meshes[0] + pos, mesh->positions + v_pos, 3 * sizeof(float));
        memcpy(ecs.meshes[0] + pos + 3, mesh->normals + n_pos, 3 * sizeof(float));
        memcpy(ecs.meshes[0] + pos + 6, mesh->texcoords + t_pos, 2 * sizeof(float));
    }
    ecs.mesh_valid[0] = true;

    // | pos     | | norm    | | tex |
    // [f] [f] [f] [f] [f] [f] [f] [f]
    // |-----16b-----| |-----16b-----|
    // Thus stride is 32b

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

    /* create pipeline object */
    state.pip = sg_make_pipeline(&(sg_pipeline_desc){
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

    sg_shader phong_shd = sg_make_shader(phong_shader_desc(sg_query_backend()));
    state.mesh_pip = sg_make_pipeline(&(sg_pipeline_desc){
        .shader = phong_shd,
        /* if the vertex layout doesn't have gaps, don't need to provide strides and offsets */
        .layout = {
            .attrs = {
                [ATTR_vs_a_pos].format = SG_VERTEXFORMAT_FLOAT3,
                [ATTR_vs_a_tex_coords] = {
                    .format = SG_VERTEXFORMAT_FLOAT2,
                    .offset = 24
                }
            },
            .buffers[0].stride = 32
        },
        .depth = {
            .write_enabled = true,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
        },
        .label = "object-pipeline"
    });

    sg_buffer mesh_buffer = sg_make_buffer(&(sg_buffer_desc){
        .size = face_count * 3 * 8 * sizeof(float),
        .content = ecs.meshes[0],
        .label = "mesh-vertices"
    });
    state.mesh_bind.vertex_buffers[0] = mesh_buffer;

    /* setup resource bindings */
    state.bind = (sg_bindings) {
        .vertex_buffers[0] = vbuf,
        .index_buffer = ibuf
    };
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
    igSetNextWindowSize((ImVec2){400, 100}, ImGuiCond_Once);
    igBegin("Hello Dear ImGui!", 0, ImGuiWindowFlags_None);
    igColorEdit3("Background", &state.pass_action.colors[0].clear_value.r, ImGuiColorEditFlags_None);
    igShowDemoWindow(true);
    igEnd();

    const float w = sapp_widthf();
    const float h = sapp_heightf();
    const float t = (float)(sapp_frame_duration() * 60.0);

    hmm_mat4 proj = HMM_Perspective(60.0f, w/h, 0.01f, 1000.0f);
    hmm_mat4 view = HMM_LookAt(HMM_Vec3(0.0f, 1.5f, 20.0f), HMM_Vec3(0.0f, 0.0f, 0.0f), HMM_Vec3(0.0f, 1.0f, 0.0f));
    hmm_mat4 view_proj = HMM_MultiplyMat4(proj, view);

    state.rx += 1.0f * t;
    state.ry += 2.0f * t;
    hmm_mat4 rxm = HMM_Rotate(state.rx, HMM_Vec3(1.0f, 0.0f, 0.0f));
    hmm_mat4 rym = HMM_Rotate(state.ry, HMM_Vec3(0.0f, 1.0f, 0.0f));

    sg_begin_default_pass(&state.pass_action, (int)w, (int)h);
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);

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

            vs_params_t vs_params;
            vs_params.mvp = HMM_MultiplyMat4(view_proj, model);
            sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));
            sg_draw(0, 36, 1);
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

static void event(const sapp_event* ev) {
    simgui_handle_event(ev);
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event,
        .width = 800,
        .height = 600,
        .sample_count = 4,
        .window_title = "Cube (sokol-app)",
        .icon.sokol_default = true,
        .logger.func = slog_func,
    };
}