#ifndef WAGON_ENGINE_H
#define WAGON_ENGINE_H

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <libgen.h>
#include <time.h>

#define HANDMADE_MATH_IMPLEMENTATION
#define HANDMADE_MATH_NO_SSE
#include "lib/hmm/HandmadeMath.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"

#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"

#define SOKOL_IMPL
#define SOKOL_METAL
#define SOKOL_NO_ENTRY
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_log.h"
#include "sokol_glue.h"
#include "sokol_imgui.h"
#include "sokol_time.h"
#include "lib/stb/stb_image.h"

#include "shader/cube.glsl.h"
#include "shader/volume.glsl.h"
// #include "shader/phong.glsl.h"

static void wagon_run();  // Add this line to declare the function

// Macro to get the directory of the current file
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define HEADER_FILE TOSTRING(__FILE__)

// ECS
// TODO this is all ugly
#define NUM_COMPONENTS 32
#define VOLUME_DIMENSIONS 50

// Define component structs
typedef struct {
    hmm_vec3 position;
    hmm_vec3 rotation; // Euler angles
    hmm_mat4 _transform; // Transform of the object based on position and rotation.
    // When they disagree, the pos/rot always takes precedence and this tranform is just used for per-frame calculations
} transform_c_t;

typedef struct {
    float* _vertices;
    uint16_t* _indices;
    unsigned _vertices_size;
    unsigned _indices_size;
    unsigned face_count;
    sg_buffer vbuf;
    sg_buffer ibuf;
    sg_bindings binding;
} mesh_c_t;

typedef struct {
    // uint8_t _volume[50 * 50 * 50]; // All volumes come in increments of 500x500x500 // TODO malloc
    uint8_t* _volume;  // Pointer to dynamically allocated volume data
    hmm_vec3 position; // Position of the volume
    sg_image img;
    sg_buffer vbuf;
} volume_c_t;

// ECS Global struct
static struct {
    // Transforms of each object
    bool transforms_valid[NUM_COMPONENTS];
    transform_c_t transforms[NUM_COMPONENTS];
    
    // Object mesh buffer
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

// Update or create a volume. Will set it to valid and malloc as needed
void update_volume(int index, uint8_t* volume_data) {
    ecs.volume_valid[index] = true;
    if (ecs.volumes[index]._volume == NULL) {
        ecs.volumes[index]._volume = (uint8_t*)calloc(VOLUME_DIMENSIONS * VOLUME_DIMENSIONS * VOLUME_DIMENSIONS, sizeof(uint8_t));
    }
    
    memcpy(ecs.volumes[index]._volume, volume_data, VOLUME_DIMENSIONS * VOLUME_DIMENSIONS * VOLUME_DIMENSIONS * sizeof(uint8_t));

    ecs.volumes[index].img = sg_make_image(&(sg_image_desc){
        .type = SG_IMAGETYPE_3D,
        .width = VOLUME_DIMENSIONS, // Your volume dimensions
        .height = VOLUME_DIMENSIONS,
        .num_slices = VOLUME_DIMENSIONS,
        .pixel_format = SG_PIXELFORMAT_R8, // Assuming 8-bit grayscale volume
        .data.subimage[0][0] = {
            .ptr = ecs.volumes[index]._volume,
            .size = VOLUME_DIMENSIONS * VOLUME_DIMENSIONS * VOLUME_DIMENSIONS * sizeof(uint8_t)
        },
        .label = "volume-texture"
    });
}

// Free a volume
void free_volume(int index) {
    ecs.volume_valid[index] = false;
    free(ecs.volumes[index]._volume);
}

// Set a volume to random values
void randomize_volume(int index) {
    // Prepare volume data
    uint8_t volume_data[VOLUME_DIMENSIONS * VOLUME_DIMENSIONS * VOLUME_DIMENSIONS] = {0};
    for (int i = 0; i < VOLUME_DIMENSIONS * VOLUME_DIMENSIONS * VOLUME_DIMENSIONS; i++) {
        volume_data[i] = rand() % 256;
    }

    update_volume(index, volume_data);
}

void sphere_volume(int index) {
    // Prepare volume data
    uint8_t volume_data[VOLUME_DIMENSIONS * VOLUME_DIMENSIONS * VOLUME_DIMENSIONS] = {0};

    // Sphere parameters
    int center = VOLUME_DIMENSIONS / 2; // Center of the sphere
    int radius = VOLUME_DIMENSIONS / 2; // Radius of the sphere

    for (int z = 0; z < VOLUME_DIMENSIONS; z++) {
        for (int y = 0; y < VOLUME_DIMENSIONS; y++) {
            for (int x = 0; x < VOLUME_DIMENSIONS; x++) {
                int dx = x - center;
                int dy = y - center;
                int dz = z - center;
                if (dx * dx + dy * dy + dz * dz <= radius * radius) {
                    int index = z * VOLUME_DIMENSIONS * VOLUME_DIMENSIONS + y * VOLUME_DIMENSIONS + x;
                    volume_data[index] = rand() % 256;
                }
            }
        }
    }

    update_volume(index, volume_data);
}

void cube_volume(int index) {
    // Prepare volume data
    uint8_t volume_data[VOLUME_DIMENSIONS * VOLUME_DIMENSIONS * VOLUME_DIMENSIONS] = {0};

    for (int z = 0; z < VOLUME_DIMENSIONS; z++) {
        for (int y = 0; y < VOLUME_DIMENSIONS; y++) {
            for (int x = 0; x < VOLUME_DIMENSIONS; x++) {
                // Check if the point is on an edge of the cube
                if ((x == 0 || x == VOLUME_DIMENSIONS - 1) || (y == 0 || y == VOLUME_DIMENSIONS - 1) || (z == 0 || z == VOLUME_DIMENSIONS - 1)) {
                    // Exclude the points that are not on the wireframe (i.e., the ones that are in the faces but not on the edges)
                    if ((x == 0 || x == VOLUME_DIMENSIONS - 1) + (y == 0 || y == VOLUME_DIMENSIONS - 1) + (z == 0 || z == VOLUME_DIMENSIONS - 1) > 1) {
                        int index = z * VOLUME_DIMENSIONS * VOLUME_DIMENSIONS + y * VOLUME_DIMENSIONS + x;
                        volume_data[index] = 255;
                    }
                }
            }
        }
    }

    update_volume(index, volume_data);
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
    sg_pipeline volume_pip;
    // sg_image volume_img;
    sg_bindings volume_bind; // Temp var which is used to process the volume currently getting rendered
    sg_pass_action pass_action;
    bool key_down[256]; // keeps track of keypresses
    bool show_debug_cubes; // Checkbox to show debug cube for each entity
    double start_time_ticks;
    double wall_time_ms;
} state = {
    .cam_fov = 60.0f,
    .cam_drift = false,
    .show_debug_cubes = true,
};

fastObjMesh* mesh;

// User code pointers
void (*user_init_callback)();
void (*user_frame_callback)();

// Temp code to generate a colormap
#define COLORMAP_WIDTH 260
#define COLORMAP_HEIGHT 1

uint8_t colormap_data[COLORMAP_WIDTH * 4]; // 4 channels (RGBA) per pixel
sg_image colormap_img;

void create_colormap(void) {
    for (int i = 0; i < COLORMAP_WIDTH; ++i) {
        float t = (float)i / (COLORMAP_WIDTH - 1);
        colormap_data[i * 4 + 0] = (uint8_t)((1.0f - t) * 255); // Blue channel
        colormap_data[i * 4 + 1] = 0;                           // Green channel
        colormap_data[i * 4 + 2] = (uint8_t)(t * 255);          // Red channel
        colormap_data[i * 4 + 3] = (uint8_t)((1.0f - t) * 255);                         // Alpha channel
        // printf("COLORMAP %d %d %d %d\n", colormap_data[i * 4 + 0], colormap_data[i * 4 + 1], colormap_data[i * 4 + 2], colormap_data[i * 4 + 3]);
    }
}
// End temp code

// Function to get the directory from __FILE__
void get_dir_from_file(const char *file_path, char *dir_path, size_t size) {
    const char *last_slash = strrchr(file_path, '/');
    if (last_slash) {
        size_t dir_length = last_slash - file_path;
        if (dir_length < size) {
            strncpy(dir_path, file_path, dir_length);
            dir_path[dir_length] = '\0';
        }
    } else {
        // No slash found, set dir_path to empty string
        strncpy(dir_path, "", size);
    }
}

void cleanup_ecs(void);
void update_mesh(int index, float* vertices, unsigned vertices_size, uint16_t* indices, unsigned indices_size, unsigned face_count);


void cleanup_ecs(void) {
    for (int i = 0; i < NUM_COMPONENTS; i++) {
        if (ecs.mesh_valid[i]) {
            free(ecs.meshes[i]._vertices);
            free(ecs.meshes[i]._indices);
        }
    }
    free(ecs.mesh_valid);
    free(ecs.meshes);
}

void update_mesh(int index, float* positions, unsigned position_count, uint16_t* indices, unsigned indices_size, unsigned face_count) {
    // ecs.mesh_valid[index] = true;
    mesh_c_t* mesh = &ecs.meshes[index];

    // Free existing data if any
    if (mesh->_vertices) free(mesh->_vertices);
    if (mesh->_indices) free(mesh->_indices);

    // Calculate new sizes
    unsigned vertices_size = position_count * 7; // 3 for position, 4 for color
    mesh->_vertices_size = vertices_size;
    mesh->_indices_size = indices_size;
    mesh->face_count = face_count;

    // Allocate new memory
    mesh->_vertices = (float*)malloc(vertices_size * sizeof(float));
    mesh->_indices = (uint16_t*)malloc(indices_size * sizeof(uint16_t));

    if (!mesh->_vertices || !mesh->_indices) {
        // Handle memory allocation failure
        if (mesh->_vertices) free(mesh->_vertices);
        if (mesh->_indices) free(mesh->_indices);
        mesh->_vertices = NULL;
        mesh->_indices = NULL;
        ecs.mesh_valid[index] = false;
        return;
    }

    // Copy and format vertex data
    float red_rgba[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    for (unsigned i = 0; i < position_count; i++) {
        unsigned dest_pos = i * 7;
        unsigned src_pos = i * 3;
        
        // Copy position (XYZ)
        mesh->_vertices[dest_pos] = positions[src_pos];
        mesh->_vertices[dest_pos + 1] = positions[src_pos + 1];
        mesh->_vertices[dest_pos + 2] = positions[src_pos + 2];
        
        // Copy color (RGBA)
        memcpy(&mesh->_vertices[dest_pos + 3], red_rgba, 4 * sizeof(float));
    }

    // Copy index data
    memcpy(mesh->_indices, indices, indices_size * sizeof(uint16_t));

    // Update Sokol buffers
    mesh->vbuf = sg_make_buffer(&(sg_buffer_desc){
        .data = (sg_range){ mesh->_vertices, vertices_size * sizeof(float) },
        .label = "mesh-vertices"
    });
    mesh->ibuf = sg_make_buffer(&(sg_buffer_desc){
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .data = (sg_range){ mesh->_indices, indices_size * sizeof(uint16_t) },
        .label = "mesh-indices"
    });

    // Update bindings
    mesh->binding = (sg_bindings) {
        .vertex_buffers[0] = mesh->vbuf,
        .index_buffer = mesh->ibuf,
    };
}

void load_mesh(int index,const char* mesh_path, float r, float g, float b) {
    printf("hi\n");
    mesh = fast_obj_read(mesh_path);
    int face_count = mesh->face_count;
    printf("hih\n");

    printf("MESHY LOAD LOAD");
    
    ecs.meshes[index]._indices_size = mesh->index_count;
    ecs.meshes[index]._indices = (uint16_t*)malloc(ecs.meshes[index]._indices_size * sizeof(uint16_t));
    if (!ecs.meshes[index]._indices) {
        // Handle allocation failure
        printf("Failed to allocate memory for mesh indices!");
        return;
    }
    for (int i = 0; i < mesh->index_count; i++) {
        printf("= %d\n", mesh->indices[i].p);
        ecs.meshes[index]._indices[i] = mesh->indices[i].p;
    }
    ecs.meshes[index].face_count = mesh->face_count;

    unsigned int i = 0; // Keep i def outside loop to use it to get size

    ecs.meshes[index]._vertices_size = mesh->position_count * 7; // 3 for position, 4 for color
    ecs.meshes[index]._vertices = (float*)malloc(ecs.meshes[index]._vertices_size * sizeof(float));
    if (!ecs.meshes[index]._vertices) {
        // Handle allocation failure
        printf("Failed to allocate memory for mesh vertices!");
        return;
    }

    for (i = 0; i < mesh->position_count; i++) {
        printf("i is %d\n", i);
        unsigned int pos = i * 7;
        float red_rgba[] = { r, g, b, 1.0f};
        ecs.meshes[index]._vertices[pos] = mesh->positions[i * 3];
        ecs.meshes[index]._vertices[pos + 1] = mesh->positions[i * 3 + 1];
        ecs.meshes[index]._vertices[pos + 2] = mesh->positions[i * 3 + 2];
        memcpy(ecs.meshes[index]._vertices + pos + 3, red_rgba, 4 * sizeof(float)); // Color, just make it all red
    }

    ecs.mesh_valid[index] = true;

    for (int k = 0; k < i * 7; k++) {
        if (k % 7 == 0) {
            printf("\n");
        }
        if (ecs.meshes[index]._vertices[k] >= 0.0f)
            printf(" ");
        printf("%.2f, ", ecs.meshes[index]._vertices[k]);
    }

    ecs.meshes[index].vbuf = sg_make_buffer(&(sg_buffer_desc){
        .data = (sg_range){ ecs.meshes[index]._vertices, i * 7 * sizeof(float) },
        .label = "mesh-vertices"
    });

    ecs.meshes[index].ibuf = sg_make_buffer(&(sg_buffer_desc){
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .data = (sg_range){ ecs.meshes[index]._indices, ecs.meshes[index]._indices_size * sizeof(uint16_t) },
        .label = "mesh-indices"
    });

    ecs.meshes[index].binding = (sg_bindings) {
        .vertex_buffers[0] = ecs.meshes[index].vbuf,
        .index_buffer = ecs.meshes[index].ibuf,
    };
}

// Modify the init function
void init(void) {

    // Used for relative paths on default font and mesh
    char dir_path[1024];
    get_dir_from_file(__FILE__, dir_path, sizeof(dir_path));

    memset(state.key_down, 0, sizeof(state.key_down)); // set key down to zero

    for (int i = 0; i < 5; i++) {
        update_transform(i, &(transform_c_t){
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
    
    stm_setup();
    state.start_time_ticks = stm_now();
    state.wall_time_ms = 0.0;

    // sg_setup(&(sg_desc){
    //     .context = sapp_sgcontext(),
    //     .logger.func = slog_func,
    // });
    sg_setup(&(sg_desc){
        .environment = sglue_environment(),
        .logger.func = slog_func
    });

    // simgui_setup(&(simgui_desc_t){ 0 });
    simgui_setup(&(simgui_desc_t) {
        // .dpi_scale = sapp_dpi_scale(),
        .no_default_font = true,
    });

    // Load a custom font in imgui
    // TODO: Make this it's own function
    // TODO: Fix antialiasing/appears blurry on text
    // ImGuiIO *io = igGetIO();
    // gui.main_font = ImFontAtlas_AddFontFromFileTTF(io->Fonts, "assets/NotoSans-Regular.ttf", 16, NULL, NULL);

    
    char font_path[1024];
    snprintf(font_path, sizeof(font_path), "%s/assets/NotoSans-Regular.ttf", dir_path);
    float font_size = 16.0f;

    printf("Font path: %s\n", font_path);


    // Load the custom font inline using variables
    ImGuiIO* io = igGetIO();
    ImFont* main_font = ImFontAtlas_AddFontFromFileTTF(io->Fonts, font_path, font_size, NULL, NULL);
    if (main_font == NULL) {
        // Handle font loading failure
        printf("Failed to load font: %s\n", font_path);
    }
    printf("FONTY LOADY LOAD\n");
    // io->Fonts->Flags |= ImGuiFreeTypeBuilderFlags_NoHinting; // Example flag for better antialiasing (optional)

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

    printf("h\n");
    // Set atlas to right texture
    simgui_image_desc_t img_desc;
    _simgui_clear(&img_desc, sizeof(img_desc));
    img_desc.image = _simgui.font_img;
    img_desc.sampler = _simgui.font_smp;
    _simgui.default_font = simgui_make_image(&img_desc);
    io->Fonts->TexID = simgui_imtextureid(_simgui.default_font);


    // char mesh_path[1024];
    // snprintf(mesh_path, sizeof(mesh_path), "%s/assets/monkey.obj", dir_path);
    // printf("hi\n");
    // mesh = fast_obj_read(mesh_path);
    // int face_count = mesh->face_count;
    // printf("hih\n");

    printf("MESHY LOAD LOAD");
    // update_mesh(2, mesh->positions, mesh->position_count, (uint16_t*)mesh->indices, mesh->index_count, mesh->face_count);
    // Function to load and process mesh data

    // Call the function with the mesh path
    char mesh_path[1024];
    snprintf(mesh_path, sizeof(mesh_path), "%s/assets/monkey.obj", dir_path);
    printf("Loading mesh: %s\n", mesh_path);
    load_mesh(0, mesh_path, 1.0f, 0.0f, 0.0f);
    
    char mesh_path_2[1024];
    snprintf(mesh_path_2, sizeof(mesh_path_2), "%s/examples/basic/ball.obj", dir_path);
    printf("Loading mesh: %s\n", mesh_path_2);
    load_mesh(1, mesh_path_2, 0.0f, 1.0f, 0.0f);

    user_init_callback();
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
    0, 2, 1,  0, 3, 2,
    6, 4, 5,  7, 4, 6,
    8, 10, 9,  8, 11, 10,
    14, 12, 13,  15, 12, 14,
    16, 18, 17,  16, 19, 18,
    22, 20, 21,  23, 20, 22
};
    sg_buffer ibuf = sg_make_buffer(&(sg_buffer_desc){
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .data = SG_RANGE(indices),
        .label = "cube-indices"
    });

    /* create shader */
    sg_shader shd = sg_make_shader(cube_shader_desc(sg_query_backend()));
    sg_shader volume_shader = sg_make_shader(volume_shader_desc(sg_query_backend()));

    sg_blend_state blend_state = {
        .enabled = true,
        .src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA,
        .dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        .op_rgb = SG_BLENDOP_ADD,
        .src_factor_alpha = SG_BLENDFACTOR_ONE,
        .dst_factor_alpha = SG_BLENDFACTOR_ZERO,
        .op_alpha = SG_BLENDOP_ADD
    };

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
        .cull_mode = SG_CULLMODE_FRONT,
        .depth = {
            .write_enabled = true,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
        },
        .colors[0].blend = blend_state, // Add blend state here
        .label = "cube-pipeline"
    });

    state.volume_pip = sg_make_pipeline(&(sg_pipeline_desc){
        .layout = {
            .buffers[0].stride = 12,
            .attrs = {
                [ATTR_vs_volume_pos] = { .format = SG_VERTEXFORMAT_FLOAT3 }, // position
            }
        },
        .shader = volume_shader,
        .index_type = SG_INDEXTYPE_UINT16,
        .cull_mode = SG_CULLMODE_BACK,
        .depth = {
            .write_enabled = true,
            .compare = SG_COMPAREFUNC_LESS_EQUAL,
        },
        .colors[0].blend = blend_state, // Add blend state here
        .label = "volume-pipeline",
    });



    // Update volume for testing
    randomize_volume(1);

    // THIS WORKS
    // // set first volume to random
    // ecs.volume_valid[0] = true;
    // // for (int i = 0; i < 100 * 100 * 100; i++) {
    // //     ecs.volumes[0]._volume[i] = i % 256;
    // //     // printf("%d\n", ecs.volumes[0]._volume[i]);
    // // }

    // for (int z = 0; z < 100; z++) {
    //     for (int y = 0; y < 100; y++) {
    //         for (int x = 0; x < 100; x++) {
    //             // Calculate the linear gradient value along the X axis
    //             if (y > 1 || y < 1) {
    //                 ecs.volumes[0]._volume[x + y * 50 + z * 50 * 50] = (uint8_t)0;
    //             }
    //             else{
    //                 ecs.volumes[0]._volume[x + y * 50 + z * 50 * 50] = (uint8_t)((float)x / (50 - 1) * 255);
    //             }
    //         }
    //     }
    // }

    // int size = 100;
    // int radius = size / 2;
    // int centerX = radius;
    // int centerY = radius;
    // int centerZ = radius;

    // for (int z = 0; z < size; z++) {
    //     for (int y = 0; y < size; y++) {
    //         for (int x = 0; x < size; x++) {
    //             // Calculate the distance from the center
    //             int dx = x - centerX;
    //             int dy = y - centerY;
    //             int dz = z - centerZ;
    //             float distance = sqrt(dx * dx + dy * dy + dz * dz);

    //             if (distance > radius) {
    //                 ecs.volumes[0]._volume[x + y * size + z * size * size] = (uint8_t)0;
    //             } else {
    //                 ecs.volumes[0]._volume[x + y * size + z * size * size] = (uint8_t)((float)x / (size - 1) * 255);
    //             }
    //         }
    //     }
    // }
    // state.volume_img = sg_make_image(&(sg_image_desc){
    //     .type = SG_IMAGETYPE_3D,
    //     .width = 50, // Your volume dimensions
    //     .height = 50,
    //     .num_slices = 50,
    //     .pixel_format = SG_PIXELFORMAT_R8, // Assuming 8-bit grayscale volume
    //     .data.subimage[0][0] = {
    //         .ptr = ecs.volumes[0]._volume,
    //         .size = sizeof(ecs.volumes[0]._volume)
    //     },
    //     .label = "volume-texture"
    // });

    // Create the colormap
    create_colormap();

    // Create the Sokol image for the colormap
    colormap_img = sg_make_image(&(sg_image_desc){
        .width = COLORMAP_WIDTH,
        .height = COLORMAP_HEIGHT,
        .pixel_format = SG_PIXELFORMAT_RGBA8,
        .data.subimage[0][0] = {
            .ptr = colormap_data,
            .size = sizeof(colormap_data)
        },
        .label = "colormap"
    });


    // Volume vertices
    // float volume_vertices[] = {
    //     -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f,
    //      1.0f, -1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
    //      1.0f,  1.0f, 0.0f,  1.0f, 1.0f, 0.0f,
    //     -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f
    // };
    float volume_vertices[] = {
        // Positions
        -1.0, -1.0, -1.0,
         1.0, -1.0, -1.0,
         1.0,  1.0, -1.0,
        -1.0,  1.0, -1.0,

        -1.0, -1.0,  1.0,
         1.0, -1.0,  1.0,
         1.0,  1.0,  1.0,
        -1.0,  1.0,  1.0,

        -1.0, -1.0, -1.0,
        -1.0,  1.0, -1.0,
        -1.0,  1.0,  1.0,
        -1.0, -1.0,  1.0,

        1.0, -1.0, -1.0,
        1.0,  1.0, -1.0,
        1.0,  1.0,  1.0,
        1.0, -1.0,  1.0,

        -1.0, -1.0, -1.0,
        -1.0, -1.0,  1.0,
         1.0, -1.0,  1.0,
         1.0, -1.0, -1.0,

        -1.0,  1.0, -1.0,
        -1.0,  1.0,  1.0,
         1.0,  1.0,  1.0,
         1.0,  1.0, -1.0,
    };

    /* create an index buffer for the cube */
    uint16_t volume_indices[] = {
        0, 2, 1,  0, 3, 2,
        6, 4, 5,  7, 4, 6,
        8, 10, 9,  8, 11, 10,
        14, 12, 13,  15, 12, 14,
        16, 18, 17,  16, 19, 18,
        22, 20, 21,  23, 20, 22
    };

    sg_buffer volume_ibuf = sg_make_buffer(&(sg_buffer_desc){
        .type = SG_BUFFERTYPE_INDEXBUFFER,
        .data = SG_RANGE(volume_indices),
        .label = "cube-indices"
    });

    sg_buffer volume_vbuf = sg_make_buffer(&(sg_buffer_desc){
        .data = SG_RANGE(volume_vertices),
        .label = "volume-vertices"
    });
    
    state.volume_bind = (sg_bindings){
        .vertex_buffers[0] = volume_vbuf, // Assuming you have vertices for the volume
        .index_buffer = volume_ibuf, // Assuming you have indices for the volume
        // .fs_images[SLOT_tex] = state.volume_img
    };
    // //THIS WORKS
    // state.volume_bind.fs.images[SLOT_volume] = state.volume_img;
    state.volume_bind.fs.samplers[SLOT_volume_smp] = sg_make_sampler(&(sg_sampler_desc){
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
    });
    // Add the colormap image to the bindings
    // state.volume_bind.fs.images[SLOT_colormap] = colormap_img;
    state.volume_bind.fs.samplers[SLOT_colormap_smp] = sg_make_sampler(&(sg_sampler_desc){
        .min_filter = SG_FILTER_NEAREST,
        .mag_filter = SG_FILTER_NEAREST,
    });

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

    // ecs.meshes[1].binding = (sg_bindings) {
    //     .vertex_buffers[0] = ecs.meshes[1].vbuf,
    //     .index_buffer = ecs.meshes[1].ibuf,
    // };
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

    // printf("forward %.2f, %.2f, %.2f | right %.2f, %.2f, %.2f\n", forward.X, forward.Y, forward.Z, right.X, right.Y, right.Z);

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
    state.wall_time_ms = stm_ms(stm_since(state.start_time_ticks));

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
    igText("Wall Time: %.2f ms", state.wall_time_ms);
    igCheckbox("Show Debug Cubes", &state.show_debug_cubes);
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

    // Entity Settings UI Code
    igSetNextWindowPos((ImVec2){10, 320}, ImGuiCond_Once, (ImVec2){0,0});
    igSetNextWindowSize((ImVec2){600, 300}, ImGuiCond_Once);
    igBegin("Entity Settings", 0, ImGuiWindowFlags_None);
    for (int i = 0; i < NUM_COMPONENTS; i++) {
        char entity_label[64];
        if (ecs.transforms_valid[i]) {
            snprintf(entity_label, sizeof(entity_label), "Entity %d", i);
        }
        else {
            snprintf(entity_label, sizeof(entity_label), "Entity %d (invalid)", i);
        }
        if (igTreeNodeEx_Str(entity_label, ImGuiTreeNodeFlags_NoAutoOpenOnLog)) {
            igCheckbox("Transform Valid", &ecs.transforms_valid[i]);
            igCheckbox("Mesh Valid", &ecs.mesh_valid[i]);
            igCheckbox("Volume Valid", &ecs.volume_valid[i]);
            
            // Add XYZ input for each entity's position
            if (ecs.transforms_valid[i]) {
                igDragFloat3("Position", &ecs.transforms[i].position.X, 0.1f, -10.0f, 10.0f, "%.1f", 0);
                igDragFloat3("Rotation", &ecs.transforms[i].rotation.X, 0.1f, -360.0f, 360.0f, "%.1f", 0);
            }

            // Add a button to randomize the volume, if volume is valid
            if (ecs.volume_valid[i]) {
                if (igButton("Randomize Volume", (ImVec2){0, 0})) {
                    randomize_volume(i);
                }
                if (igButton("Set to sphere", (ImVec2){0, 0})) {
                    sphere_volume(i);
                }
                if (igButton("Set to cube", (ImVec2){0, 0})) {
                    cube_volume(i);
                }
            }


            igTreePop();
        }
    }
    igEnd();

    // I don't remember what this was for
    // igSetNextWindowPos((ImVec2){300,20}, ImGuiCond_Once, (ImVec2){0,0});
    // igSetNextWindowSize((ImVec2){600, 300}, ImGuiCond_Once);
    // igBegin("2d scroll slice view", 0, ImGuiWindowFlags_None);
    // igSliderInt("Slice", &state.cam_rx, 0, 10, "%d", 0);
    // igText("h");
    // igEnd();

    const float w = sapp_widthf();
    const float h = sapp_heightf();
    const float t = (float)(sapp_frame_duration() * 60.0);

    // Camera movement
    camera_move(t);

    // User-defined per-frame logic
    user_frame_callback();

    if (state.cam_drift) {
        state.cam_rx += 1.0f * t;
        state.cam_ry += 0.5f * t;
        // Wrap around camera left/right
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

    // Camera position is multiplied by -1 because this is camera position and we offset everything else by this position
    hmm_mat4 view = HMM_Translate(HMM_MultiplyVec3f(state.cam_pos, -1.0f));
    hmm_mat4 view_proj = HMM_MultiplyMat4(proj, view);

    // sg_begin_default_pass(&state.pass_action, (int)w, (int)h);
    sg_begin_pass(&(sg_pass){ .action = state.pass_action, .swapchain = sglue_swapchain() });
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);

    // Calculate the model view projection matrix for each transform
    for (int i = 0; i < NUM_COMPONENTS; i++) {
        transform_c_t *transform = &ecs.transforms[i];
        if (ecs.transforms_valid[i]) {
            ecs.transforms[i].rotation.X += 1.0f * t;

            hmm_mat4 model = HMM_Translate(transform->position);

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
    if (state.show_debug_cubes) {
        for (int i = 0; i < NUM_COMPONENTS; i++) {
            if (ecs.transforms_valid[i]) {
                vs_params_t vs_params;
                vs_params.mvp = HMM_MultiplyMat4(view_proj, ecs.transforms[i]._transform);
                sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_params));
                sg_draw(0, 36, 1);
            }
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

    // Render each volume if it exists
    for (int i = 0; i < NUM_COMPONENTS; i++) {
        if (ecs.transforms_valid[i] && ecs.volume_valid[i]) {
            if (ecs.volume_valid[i] && ecs.volumes[i]._volume == NULL) { // TODO make this a better null check
                printf("VOLUME DATA FROM ENTITY NUMBER %d UNALLOCATED!\n", i);
                continue;
            }

            sg_apply_pipeline(state.volume_pip);
            sg_bindings volume_bind = {
                .vertex_buffers[0] = state.volume_bind.vertex_buffers[0],
                .index_buffer = state.volume_bind.index_buffer,
            };

            volume_bind.fs.images[SLOT_volume] = ecs.volumes[i].img;
            volume_bind.fs.samplers[SLOT_volume_smp] = state.volume_bind.fs.samplers[SLOT_volume_smp];
            // Add the colormap image to the bindings
            volume_bind.fs.images[SLOT_colormap] = colormap_img;
            volume_bind.fs.samplers[SLOT_colormap_smp] = state.volume_bind.fs.samplers[SLOT_colormap_smp];
            sg_apply_bindings(&volume_bind);

            fs_vol_params_t fs_vol_params = {
                .volume_dims = { 100, 100, 100 },
                .dt_scale = 0.005f,
                .near_clip = 0.01f,
                .far_clip = 1000.0f,
                .new_box_min = { 0.0f, 0.0f, 0.0f },
                .new_box_max = { 1.0f, 1.0f, 1.0f }
            };
            sg_apply_uniforms(SG_SHADERSTAGE_FS, SLOT_fs_vol_params, &SG_RANGE(fs_vol_params));

            vs_vol_params_t vs_vol_params = {
                .proj_view = view_proj,
                .eye_pos = { state.cam_pos.X, state.cam_pos.Y, state.cam_pos.Z },
                .volume_scale = { 1.0f, 1.0f, 1.0f },
                .volume_translation = {
                    ecs.transforms[i].position.X,
                    ecs.transforms[i].position.Y,
                    ecs.transforms[i].position.Z
                }
            };
            sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &SG_RANGE(vs_vol_params));

            sg_draw(0, 36, 1); // Assuming 4 vertices for the volume quad
        }
    }

    simgui_render();
    sg_end_pass();
    sg_commit();
}

void cleanup(void) {
    // fast_obj_destroy(mesh);

    cleanup_ecs();
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

// sapp_desc sokol_main(int argc, char* argv[]) {
//     (void)argc;
//     (void)argv;
//     return (sapp_desc){
//         .init_cb = init,
//         .frame_cb = frame,
//         .cleanup_cb = cleanup,
//         .event_cb = event,
//         .width = 1080,
//         .height = 720,
//         .sample_count = 4,
//         .window_title = "Wagon Engine",
//         .icon.sokol_default = true,
//         .logger.func = slog_func,
//     };
// }

static void wagon_run() {
    // app_state_t* state = calloc(1, sizeof(app_state_t));
    sapp_run(&(sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event,
        .width = 800,
        .height = 600,
        .sample_count = 4,
        .window_title = "Noentry (sokol-app)",
        .icon.sokol_default = true,
        .logger.func = slog_func,
    });
    // free(state);    // NOTE: on some platforms, this isn't reached on exit
    // return 0;
}

#endif // WAGON_ENGINE_H
