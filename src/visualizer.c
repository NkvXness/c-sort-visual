#include "visualizer.h"
#include <stdlib.h>

static const Color COL_NORMAL = { 72, 108, 196, 255 };

VisState *vis_create(int size) {
    VisState *vs = malloc(sizeof(VisState));
    vs->size = size;
    return vs;
}

void vis_free(VisState *vs) {
    free(vs);
}

void vis_draw_bars(const Array *arr, const VisState *vs,
                   int ox, int oy, int w, int h) {
    (void)vs;
    float bar_w = (float)w / arr->size;
    float gap   = (bar_w > 3.0f) ? 1.0f : 0.0f;
    for (int i = 0; i < arr->size; i++) {
        float ratio = (float)arr->data[i] / (float)arr->max_val;
        int   bar_h = (int)(ratio * h);
        int   bx    = ox + (int)((float)i * bar_w);
        int   by    = oy + h - bar_h;
        int   bw    = (int)(bar_w - gap);
        if (bw < 1) bw = 1;
        DrawRectangle(bx, by, bw, bar_h, COL_NORMAL);
    }
}
