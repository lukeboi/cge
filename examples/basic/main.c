#include "wagon_engine.h"

void example_init() {
    printf("Example init\n");
    ecs.transforms[0].position.X = 0;
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