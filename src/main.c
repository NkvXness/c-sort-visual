#include <raylib.h>

#include "array.h"
#include "visualizer.h"

#define SCREEN_W 1280
#define SCREEN_H  720

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "c-sort-visual");
    SetTargetFPS(60);

    Array    *arr = array_create(80);
    VisState *vis = vis_create(arr->size);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground((Color){ 10, 10, 16, 255 });
        vis_draw_bars(arr, vis, 0, 0, SCREEN_W, SCREEN_H);
        EndDrawing();
    }

    vis_free(vis);
    array_free(arr);
    CloseWindow();
    return 0;
}
