#include "wagon_engine.h"

void example_init() {
    printf("Example init\n");
    ecs.transforms[0].position.X = 0;
    
    // // Used for relative paths
    // char dir_path[1024];
    // get_dir_from_file(__FILE__, dir_path, sizeof(dir_path));
    // char mesh_path[1024];
    // snprintf(mesh_path, sizeof(mesh_path), "./ball.obj");
    // printf("Loading mesh: %s\n", mesh_path);
    // // exit(1);
    // load_mesh(1, mesh_path);
}

void example_frame_callback() {
    ecs.transforms[0].position.Y = sin(state.wall_time_ms / 1000.0);
}

int main() {
    user_init_callback = example_init;
    user_frame_callback = example_frame_callback;
    wagon_run();
    return 0;
}