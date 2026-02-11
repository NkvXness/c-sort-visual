#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "array.h"
#include "sort.h"
#include <raylib.h>

typedef enum {
    ROLE_NORMAL = 0,
    ROLE_COMPARING,
    ROLE_SWAPPING,
    ROLE_SORTED,
    ROLE_PIVOT,
} BarRole;

typedef struct {
    int   *roles;
    float *anim_t;
    int    size;
} VisState;

VisState *vis_create(int size);
void      vis_free(VisState *vs);
void      vis_apply_step(VisState *vs, const SortStep *step, int *data);
void      vis_update_anims(VisState *vs, float dt);
void      vis_draw_bars(const Array *arr, const VisState *vs,
                        int ox, int oy, int w, int h);

#endif
