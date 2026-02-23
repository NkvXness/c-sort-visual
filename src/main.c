#include <raylib.h>
#include <stdlib.h>
#include <string.h>

#include "array.h"
#include "sort.h"
#include "visualizer.h"
#include "ui.h"

/* ── Layout constants ─────────────────────────────────────────────────────── */
#define SCREEN_W   1280
#define SCREEN_H    720
#define TOOLBAR_H    58
#define SLIDERS_H    56
#define PAD          10

/* ── Algorithm registry ───────────────────────────────────────────────────── */
#define ALGO_COUNT 5

typedef void (*GenFn)(StepQueue *, const int *, int);

static const char *ALGO_NAMES[ALGO_COUNT] = {
    "Bubble Sort", "Selection Sort", "Insertion Sort",
    "Merge Sort",  "Quick Sort"
};
static const GenFn ALGO_GEN[ALGO_COUNT] = {
    gen_bubble_sort, gen_selection_sort, gen_insertion_sort,
    gen_merge_sort,  gen_quick_sort
};

/* ── Application state ────────────────────────────────────────────────────── */
typedef struct {
    Array     *arr;
    VisState  *vis;
    StepQueue *queue;          /* NULL when no sort is active      */

    int   algo;                /* index into ALGO_NAMES / ALGO_GEN */
    int   running;
    int   paused;
    int   done;

    float speed;               /* steps consumed per second        */
    float accum;               /* leftover time for step pacing    */
    float elapsed;             /* wall-clock seconds since start   */

    /* stats shown in the footer — incremented as steps are consumed */
    int   cmps;
    int   swaps_count;
} App;

/* ── Helpers ──────────────────────────────────────────────────────────────── */

static void app_start_sort(App *a) {
    if (a->queue) queue_free(a->queue);
    a->queue   = queue_create();
    ALGO_GEN[a->algo](a->queue, a->arr->data, a->arr->size);
    vis_clear_roles(a->vis);
    a->running = 1;
    a->paused  = 0;
    a->done    = 0;
    a->accum   = 0.0f;
    a->elapsed = 0.0f;
    a->cmps    = 0;
    a->swaps_count = 0;
}

static void app_reset(App *a) {
    array_shuffle(a->arr);
    vis_clear_roles(a->vis);
    if (a->queue) { queue_free(a->queue); a->queue = NULL; }
    a->running = 0;
    a->paused  = 0;
    a->done    = 0;
    a->accum   = 0.0f;
    a->elapsed = 0.0f;
    a->cmps    = 0;
    a->swaps_count = 0;
}

/* Consume one step and mirror it into the display array + vis state. */
static void app_consume_step(App *a) {
    SortStep step;
    if (queue_pop(a->queue, &step)) {
        if (step.type == STEP_COMPARE) a->cmps++;
        if (step.type == STEP_SWAP)    a->swaps_count++;
        vis_apply_step(a->vis, &step, a->arr->data);
    } else {
        a->done = 1;
    }
}

/* ── Entry point ──────────────────────────────────────────────────────────── */

int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_W, SCREEN_H, "c-sort-visual");
    SetTargetFPS(60);

    /* ── Initial app state ────────────────────────────────────────────────── */
    App a = {0};
    a.arr   = array_create(80);
    a.vis   = vis_create(a.arr->size);
    a.speed = 40.0f;

    /* ── Algorithm selector buttons ──────────────────────────────────────── */
    Button algo_btns[ALGO_COUNT];
    for (int i = 0; i < ALGO_COUNT; i++) {
        algo_btns[i] = (Button){
            .rect   = { 10.0f + (float)i * 166.0f, 11.0f, 158.0f, 36.0f },
            .label  = ALGO_NAMES[i],
            .active = (i == 0)
        };
    }

    Button btn_shuffle = {{ 890.0f, 11.0f, 90.0f,  36.0f }, "Shuffle", 0 };
    Button btn_play    = {{ 986.0f, 11.0f, 76.0f,  36.0f }, "Play",    0 };
    Button btn_pause   = {{1068.0f, 11.0f, 76.0f,  36.0f }, "Pause",   0 };
    Button btn_step    = {{1150.0f, 11.0f, 120.0f, 36.0f }, "Step →",  0 };

    /* ── Sliders ──────────────────────────────────────────────────────────── */
    Slider sl_size  = {{ 30.0f,  TOOLBAR_H + 30.0f, 260.0f, 10.0f },
                        "Array size",  10.0f, 256.0f, 80.0f,  0 };
    Slider sl_speed = {{ 380.0f, TOOLBAR_H + 30.0f, 260.0f, 10.0f },
                        "Speed",       1.0f,  300.0f, 40.0f,  0 };

    /* ── Main loop ────────────────────────────────────────────────────────── */
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        /* ── Keyboard shortcuts ───────────────────────────────────────────── */
        for (int i = 0; i < ALGO_COUNT; i++) {
            if (IsKeyPressed(KEY_ONE + i)) {
                a.algo = i;
                for (int j = 0; j < ALGO_COUNT; j++)
                    algo_btns[j].active = (j == i);
                app_reset(&a);
            }
        }
        if (IsKeyPressed(KEY_SPACE)) app_reset(&a);
        if (IsKeyPressed(KEY_ENTER) && !a.running) app_start_sort(&a);
        if (IsKeyPressed(KEY_P)) a.paused = !a.paused;
        if (IsKeyPressed(KEY_RIGHT) && a.running && a.paused && !a.done)
            app_consume_step(&a);

        /* ── Sync slider values ───────────────────────────────────────────── */
        a.speed = sl_speed.value;

        int new_size = (int)sl_size.value;
        if (new_size != a.arr->size && !a.running) {
            array_resize(a.arr, new_size);
            vis_resize(a.vis, new_size);
        }

        /* ── Step the sort ────────────────────────────────────────────────── */
        if (a.running && !a.paused && !a.done) {
            a.accum   += dt;
            a.elapsed += dt;
            float interval = 1.0f / a.speed;
            while (a.accum >= interval && !a.done) {
                a.accum -= interval;
                app_consume_step(&a);
            }
        }

        vis_update_anims(a.vis, dt);

        /* ── Draw ─────────────────────────────────────────────────────────── */
        BeginDrawing();
        ClearBackground((Color){ 10, 10, 16, 255 });

        /* Toolbar background */
        DrawRectangle(0, 0, GetScreenWidth(), TOOLBAR_H,
                      (Color){ 18, 18, 28, 255 });

        /* Algorithm buttons — clicking them also resets the sort */
        for (int i = 0; i < ALGO_COUNT; i++) {
            if (button_draw(&algo_btns[i])) {
                a.algo = i;
                for (int j = 0; j < ALGO_COUNT; j++)
                    algo_btns[j].active = (j == i);
                app_reset(&a);
            }
        }

        if (button_draw(&btn_shuffle)) app_reset(&a);

        if (button_draw(&btn_play)) {
            if (!a.running) app_start_sort(&a);
            else            a.paused = 0;
        }
        if (button_draw(&btn_pause)) a.paused = 1;
        if (button_draw(&btn_step)) {
            if (!a.running) app_start_sort(&a);
            a.paused = 1;
            if (!a.done) app_consume_step(&a);
        }

        /* Sliders row */
        DrawRectangle(0, TOOLBAR_H, GetScreenWidth(), SLIDERS_H,
                      (Color){ 14, 14, 22, 255 });
        slider_draw(&sl_size);
        slider_draw(&sl_speed);

        /* Status hint */
        const char *hint = a.done    ? "Sorted!  Press Space to shuffle"
                         : a.paused  ? "Paused   P = resume   → = step"
                         : a.running ? "Running..."
                         :             "Press Play or Enter to start";
        DrawText(hint, 700, TOOLBAR_H + 20, 13, (Color){ 110, 115, 150, 255 });

        /* Bar area */
        int bx = PAD;
        int by = TOOLBAR_H + SLIDERS_H + PAD;
        int bw = GetScreenWidth()  - PAD * 2;
        int bh = GetScreenHeight() - by - 32;
        vis_draw_bars(a.arr, a.vis, bx, by, bw, bh);

        /* Footer stats */
        vis_draw_stats(a.cmps, a.swaps_count, a.elapsed,
                       ALGO_NAMES[a.algo], bx, GetScreenHeight() - 24);

        EndDrawing();
    }

    /* ── Cleanup — every malloc has a matching free ───────────────────────── */
    if (a.queue) queue_free(a.queue);
    vis_free(a.vis);
    array_free(a.arr);
    CloseWindow();
    return 0;
}