#include "sort.h"
#include <stdlib.h>

#define INITIAL_CAPACITY 8192

StepQueue *queue_create(void) {
    StepQueue *q   = malloc(sizeof(StepQueue));
    q->steps       = malloc(sizeof(SortStep) * INITIAL_CAPACITY);
    q->capacity    = INITIAL_CAPACITY;
    q->count       = 0;
    q->head        = 0;
    q->comparisons = 0;
    q->swaps       = 0;
    return q;
}

void queue_free(StepQueue *q) {
    free(q->steps);
    free(q);
}

void queue_push(StepQueue *q, StepType type, int a, int b) {
    if (q->count >= q->capacity) {
        q->capacity *= 2;
        q->steps = realloc(q->steps, sizeof(SortStep) * q->capacity);
    }
    q->steps[q->count++] = (SortStep){ type, a, b };
    if (type == STEP_COMPARE) q->comparisons++;
    if (type == STEP_SWAP)    q->swaps++;
}

int queue_pop(StepQueue *q, SortStep *out) {
    if (q->head >= q->count) return 0;
    *out = q->steps[q->head++];
    return 1;
}

int queue_done(const StepQueue *q) {
    return q->head >= q->count;
}
