#include <raylib.h>
#include <string.h>

#include "array.h"
#include "sort.h"
#include "visualizer.h"

#define SCREEN_W   1280
#define SCREEN_H    720
#define ALGO_COUNT    5

typedef void (*GenFn)(StepQueue *, const int *, int);

static const char *ALGO_NAMES[ALGO_COUNT] = {
    "Bubble Sort", "Selection Sort", "Insertion Sort",
    "Merge Sort",  "Quick Sort"
};
static const GenFn ALGO_GEN[ALGO_COUNT] = {
    gen_bubble_sort, gen_selection_sort, gen_insertion_sort,
    gen_merge_sort,  gen_quick_sort
};

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "c-sort-visual");
    SetTargetFPS(60);

    Array     *arr   = array_create(80);
    VisState  *vis   = vis_create(arr->size);
    StepQueue *queue = NULL;
    int running = 0;
    int algo    = 0;

    while (!WindowShouldClose()) {
        /* algorithm selection: keys 1-5 */
        for (int i = 0; i < ALGO_COUNT; i++) {
            if (IsKeyPressed(KEY_ONE + i)) {
                algo = i;
                array_shuffle(arr);
                memset(vis->roles,  0, vis->size * sizeof(int));
                memset(vis->anim_t, 0, vis->size * sizeof(float));
                if (queue) { queue_free(queue); queue = NULL; }
                running = 0;
            }
        }

        if (IsKeyPressed(KEY_SPACE)) {
            array_shuffle(arr);
            memset(vis->roles,  0, vis->size * sizeof(int));
            memset(vis->anim_t, 0, vis->size * sizeof(float));
            if (queue) { queue_free(queue); queue = NULL; }
            running = 0;
        }
        if (IsKeyPressed(KEY_ENTER) && !running) {
            if (queue) queue_free(queue);
            queue = queue_create();
            ALGO_GEN[algo](queue, arr->data, arr->size);
            running = 1;
        }

        if (running && !queue_done(queue)) {
            SortStep step;
            if (queue_pop(queue, &step))
                vis_apply_step(vis, &step, arr->data);
        }

        vis_update_anims(vis, GetFrameTime());

        BeginDrawing();
        ClearBackground((Color){ 10, 10, 16, 255 });
        vis_draw_bars(arr, vis, 0, 0, SCREEN_W, SCREEN_H);
        DrawText(ALGO_NAMES[algo], 10, 10, 20, (Color){ 200, 200, 200, 255 });
        EndDrawing();
    }

    if (queue) queue_free(queue);
    vis_free(vis);
    array_free(arr);
    CloseWindow();
    return 0;
}
