#include <raylib.h>
#include <string.h>

#include "array.h"
#include "sort.h"
#include "visualizer.h"
#include "ui.h"

#define SCREEN_W   1280
#define SCREEN_H    720
#define TOOLBAR_H    58
#define SLIDERS_H    56
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

static void reset(Array *arr, VisState *vis, StepQueue **queue, int *running) {
    array_shuffle(arr);
    memset(vis->roles,  0, vis->size * sizeof(int));
    memset(vis->anim_t, 0, vis->size * sizeof(float));
    if (*queue) { queue_free(*queue); *queue = NULL; }
    *running = 0;
}

static void start_sort(Array *arr, VisState *vis, StepQueue **queue,
                        int *running, int algo) {
    if (*queue) queue_free(*queue);
    *queue = queue_create();
    ALGO_GEN[algo](*queue, arr->data, arr->size);
    memset(vis->roles,  0, vis->size * sizeof(int));
    memset(vis->anim_t, 0, vis->size * sizeof(float));
    *running = 1;
}

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "c-sort-visual");
    SetTargetFPS(60);

    Array     *arr   = array_create(80);
    VisState  *vis   = vis_create(arr->size);
    StepQueue *queue = NULL;
    int   running = 0;
    int   paused  = 0;
    int   algo    = 0;
    float speed   = 40.0f;
    float accum   = 0.0f;

    Button algo_btns[ALGO_COUNT];
    for (int i = 0; i < ALGO_COUNT; i++)
        algo_btns[i] = (Button){
            { 10.0f + (float)i * 166.0f, 11.0f, 158.0f, 36.0f },
            ALGO_NAMES[i], (i == 0)
        };

    Button btn_shuffle = {{ 890.0f, 11.0f,  90.0f, 36.0f }, "Shuffle", 0 };
    Button btn_play    = {{ 986.0f, 11.0f,  76.0f, 36.0f }, "Play",    0 };
    Button btn_pause   = {{1068.0f, 11.0f,  76.0f, 36.0f }, "Pause",   0 };
    Button btn_step    = {{1150.0f, 11.0f, 120.0f, 36.0f }, "Step →",  0 };

    Slider sl_size  = {{  30.0f, TOOLBAR_H + 30.0f, 260.0f, 10.0f },
                        "Array size", 10.0f, 256.0f, 80.0f, 0 };
    Slider sl_speed = {{ 380.0f, TOOLBAR_H + 30.0f, 260.0f, 10.0f },
                        "Speed", 1.0f, 300.0f, 40.0f, 0 };

    while (!WindowShouldClose()) {
        /* keyboard algo selection */
        for (int i = 0; i < ALGO_COUNT; i++) {
            if (IsKeyPressed(KEY_ONE + i)) {
                algo = i;
                for (int j = 0; j < ALGO_COUNT; j++) algo_btns[j].active = (j == i);
                reset(arr, vis, &queue, &running);
            }
        }
        if (IsKeyPressed(KEY_SPACE)) reset(arr, vis, &queue, &running);
        if (IsKeyPressed(KEY_ENTER) && !running)
            start_sort(arr, vis, &queue, &running, algo);
        if (IsKeyPressed(KEY_P)) paused = !paused;

        speed = sl_speed.value;
        int new_size = (int)sl_size.value;
        if (new_size != arr->size && !running) {
            array_resize(arr, new_size);
            vis_resize(vis, new_size);
        }
        if (running && !paused && queue && !queue_done(queue)) {
            float dt = GetFrameTime();
            accum += dt;
            float interval = 1.0f / speed;
            while (accum >= interval && queue && !queue_done(queue)) {
                accum -= interval;
                SortStep step;
                if (queue_pop(queue, &step))
                    vis_apply_step(vis, &step, arr->data);
            }
        }

        vis_update_anims(vis, GetFrameTime());

        BeginDrawing();
        ClearBackground((Color){ 10, 10, 16, 255 });

        DrawRectangle(0, 0, GetScreenWidth(), TOOLBAR_H, (Color){ 18, 18, 28, 255 });
        DrawRectangle(0, TOOLBAR_H, GetScreenWidth(), SLIDERS_H, (Color){ 14, 14, 22, 255 });
        slider_draw(&sl_size);
        slider_draw(&sl_speed);

        for (int i = 0; i < ALGO_COUNT; i++) {
            if (button_draw(&algo_btns[i])) {
                algo = i;
                for (int j = 0; j < ALGO_COUNT; j++) algo_btns[j].active = (j == i);
                reset(arr, vis, &queue, &running);
            }
        }

        if (button_draw(&btn_shuffle)) reset(arr, vis, &queue, &running);
        if (button_draw(&btn_play)) {
            if (!running) start_sort(arr, vis, &queue, &running, algo);
            else paused = 0;
        }
        if (button_draw(&btn_pause)) paused = 1;
        if (button_draw(&btn_step)) {
            if (!running) start_sort(arr, vis, &queue, &running, algo);
            paused = 1;
            if (queue && !queue_done(queue)) {
                SortStep step;
                if (queue_pop(queue, &step))
                    vis_apply_step(vis, &step, arr->data);
            }
        }

        int bx = 10, by = TOOLBAR_H + SLIDERS_H + 10;
        int bw = GetScreenWidth() - 20;
        int bh = GetScreenHeight() - by - 10;
        vis_draw_bars(arr, vis, bx, by, bw, bh);

        EndDrawing();
    }

    if (queue) queue_free(queue);
    vis_free(vis);
    array_free(arr);
    CloseWindow();
    return 0;
}
