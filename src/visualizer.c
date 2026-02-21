#include "visualizer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const Color COL_NORMAL  = {  72, 108, 196, 255 };
static const Color COL_COMPARE = { 238, 210,  60, 255 };
static const Color COL_SWAP    = { 228,  58,  58, 255 };
static const Color COL_SORTED  = {  56, 200,  96, 255 };
static const Color COL_PIVOT   = { 196,  72, 220, 255 };

static Color lerp_color(Color a, Color b, float t) {
    return (Color){
        (unsigned char)(a.r + (float)(b.r - a.r) * t),
        (unsigned char)(a.g + (float)(b.g - a.g) * t),
        (unsigned char)(a.b + (float)(b.b - a.b) * t),
        255
    };
}

static Color role_to_color(int role) {
    switch (role) {
        case ROLE_COMPARING: return COL_COMPARE;
        case ROLE_SWAPPING:  return COL_SWAP;
        case ROLE_SORTED:    return COL_SORTED;
        case ROLE_PIVOT:     return COL_PIVOT;
        default:             return COL_NORMAL;
    }
}

VisState *vis_create(int size) {
    VisState *vs = malloc(sizeof(VisState));
    vs->size   = size;
    vs->roles  = calloc(size, sizeof(int));
    vs->anim_t = calloc(size, sizeof(float));
    return vs;
}

void vis_free(VisState *vs) {
    free(vs->roles);
    free(vs->anim_t);
    free(vs);
}

void vis_resize(VisState *vs, int new_size) {
    free(vs->roles);
    free(vs->anim_t);
    vs->size   = new_size;
    vs->roles  = calloc(new_size, sizeof(int));
    vs->anim_t = calloc(new_size, sizeof(float));
}

void vis_apply_step(VisState *vs, const SortStep *step, int *data) {
    for (int i = 0; i < vs->size; i++)
        if (vs->roles[i] == ROLE_COMPARING || vs->roles[i] == ROLE_SWAPPING)
            vs->roles[i] = ROLE_NORMAL;

    switch (step->type) {
        case STEP_COMPARE:
            vs->roles[step->a] = ROLE_COMPARING;
            vs->roles[step->b] = ROLE_COMPARING;
            break;
        case STEP_SWAP: {
            vs->roles[step->a] = ROLE_SWAPPING;
            vs->roles[step->b] = ROLE_SWAPPING;
            int tmp = data[step->a]; data[step->a] = data[step->b]; data[step->b] = tmp;
            vs->anim_t[step->a] = 0.0f;
            vs->anim_t[step->b] = 0.0f;
            break;
        }
        case STEP_SET_PIVOT:   vs->roles[step->a] = ROLE_PIVOT;  break;
        case STEP_CLEAR_PIVOT:
            if (vs->roles[step->a] == ROLE_PIVOT) vs->roles[step->a] = ROLE_NORMAL;
            break;
        case STEP_MARK_SORTED:
            vs->roles[step->a]  = ROLE_SORTED;
            vs->anim_t[step->a] = 0.0f;
            break;
        case STEP_MARK_ALL_SORTED:
            for (int i = step->a; i <= step->b; i++) {
                vs->roles[i]  = ROLE_SORTED;
                vs->anim_t[i] = 0.0f;
            }
            break;
    }
}

void vis_update_anims(VisState *vs, float dt) {
    const float ANIM_SPEED = 8.3f;
    for (int i = 0; i < vs->size; i++) {
        if (vs->anim_t[i] < 1.0f) {
            vs->anim_t[i] += dt * ANIM_SPEED;
            if (vs->anim_t[i] > 1.0f) vs->anim_t[i] = 1.0f;
        }
    }
}

void vis_draw_bars(const Array *arr, const VisState *vs,
                   int ox, int oy, int w, int h) {
    float bar_w = (float)w / arr->size;
    float gap   = (bar_w > 3.0f) ? 1.0f : 0.0f;
    for (int i = 0; i < arr->size; i++) {
        float ratio = (float)arr->data[i] / (float)arr->max_val;
        int   bar_h = (int)(ratio * h);
        int   bx    = ox + (int)((float)i * bar_w);
        int   by    = oy + h - bar_h;
        int   bw    = (int)(bar_w - gap);
        if (bw < 1) bw = 1;
        Color target = role_to_color(vs->roles[i]);
        Color col    = lerp_color(COL_NORMAL, target, vs->anim_t[i]);
        DrawRectangle(bx, by, bw, bar_h, col);
        if (bar_h > 4)
            DrawRectangle(bx, by, bw, 2, (Color){ 255, 255, 255, 30 });
    }
}

void vis_draw_stats(int comparisons, int swaps, float elapsed_sec,
                    const char *algo_name, int x, int y) {
    char buf[160];
    snprintf(buf, sizeof(buf),
             "%s   |   comparisons: %d   swaps: %d   time: %.2fs",
             algo_name, comparisons, swaps, elapsed_sec);
    DrawText(buf, x, y, 16, (Color){ 160, 160, 185, 255 });
}
