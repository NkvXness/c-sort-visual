#include <raylib.h>
#include <string.h>

#include "array.h"
#include "sort.h"
#include "visualizer.h"

#define SCREEN_W 1280
#define SCREEN_H  720

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "c-sort-visual");
    SetTargetFPS(60);

    Array     *arr   = array_create(80);
    VisState  *vis   = vis_create(arr->size);
    StepQueue *queue = NULL;
    int running = 0;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_SPACE)) {
            array_shuffle(arr);
            memset(vis->roles, 0, vis->size * sizeof(int));
            if (queue) { queue_free(queue); queue = NULL; }
            running = 0;
        }
        if (IsKeyPressed(KEY_ENTER) && !running) {
            if (queue) queue_free(queue);
            queue = queue_create();
            gen_bubble_sort(queue, arr->data, arr->size);
            running = 1;
        }

        if (running && !queue_done(queue)) {
            SortStep step;
            if (queue_pop(queue, &step))
                vis_apply_step(vis, &step, arr->data);
        }

        BeginDrawing();
        ClearBackground((Color){ 10, 10, 16, 255 });
        vis_draw_bars(arr, vis, 0, 0, SCREEN_W, SCREEN_H);
        EndDrawing();
    }

    if (queue) queue_free(queue);
    vis_free(vis);
    array_free(arr);
    CloseWindow();
    return 0;
}
