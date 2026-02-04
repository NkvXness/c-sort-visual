#include "visualizer.h"
#include <stdlib.h>
#include <string.h>

static const Color COL_NORMAL  = {  72, 108, 196, 255 };
static const Color COL_COMPARE = { 238, 210,  60, 255 };
static const Color COL_SWAP    = { 228,  58,  58, 255 };
static const Color COL_SORTED  = {  56, 200,  96, 255 };
static const Color COL_PIVOT   = { 196,  72, 220, 255 };

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
    vs->size  = size;
    vs->roles = calloc(size, sizeof(int));
    return vs;
}

void vis_free(VisState *vs) {
    free(vs->roles);
    free(vs);
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
        Color col = role_to_color(vs->roles[i]);
        DrawRectangle(bx, by, bw, bar_h, col);
    }
}
