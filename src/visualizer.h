#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "array.h"
#include <raylib.h>

typedef struct {
    int  size;
} VisState;

VisState *vis_create(int size);
void      vis_free(VisState *vs);
void      vis_draw_bars(const Array *arr, const VisState *vs,
                        int ox, int oy, int w, int h);

#endif
