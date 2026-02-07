#include "sort.h"
#include <stdlib.h>
#include <string.h>

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

static void swap(int *arr, int i, int j) {
    int tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
}

void gen_bubble_sort(StepQueue *q, const int *data, int size) {
    int *arr = malloc(sizeof(int) * size);
    memcpy(arr, data, sizeof(int) * size);
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i; j++) {
            queue_push(q, STEP_COMPARE, j, j + 1);
            if (arr[j] > arr[j + 1]) {
                queue_push(q, STEP_SWAP, j, j + 1);
                swap(arr, j, j + 1);
            }
        }
    }
    free(arr);
}
