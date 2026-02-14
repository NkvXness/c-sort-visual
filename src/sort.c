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

void gen_merge_sort(StepQueue *q, const int *data, int size) {
    int *arr = malloc(sizeof(int) * size);
    int *tmp = malloc(sizeof(int) * size);
    memcpy(arr, data, sizeof(int) * size);
    for (int width = 1; width < size; width *= 2) {
        for (int lo = 0; lo < size; lo += 2 * width) {
            int mid = lo + width;
            if (mid >= size) break;
            int hi = lo + 2 * width;
            if (hi > size) hi = size;
            int l = lo, r = mid, k = lo;
            while (l < mid && r < hi) {
                queue_push(q, STEP_COMPARE, l, r);
                if (arr[l] <= arr[r]) tmp[k++] = arr[l++];
                else                  tmp[k++] = arr[r++];
            }
            while (l < mid) tmp[k++] = arr[l++];
            while (r < hi)  tmp[k++] = arr[r++];
            for (int i = lo; i < hi; i++) {
                if (arr[i] != tmp[i]) {
                    queue_push(q, STEP_SWAP, i, i);
                    arr[i] = tmp[i];
                }
            }
        }
    }
    queue_push(q, STEP_MARK_ALL_SORTED, 0, size - 1);
    free(tmp);
    free(arr);
}

void gen_insertion_sort(StepQueue *q, const int *data, int size) {
    int *arr = malloc(sizeof(int) * size);
    memcpy(arr, data, sizeof(int) * size);
    for (int i = 1; i < size; i++) {
        int j = i;
        while (j > 0) {
            queue_push(q, STEP_COMPARE, j - 1, j);
            if (arr[j - 1] > arr[j]) {
                queue_push(q, STEP_SWAP, j - 1, j);
                swap(arr, j - 1, j);
                j--;
            } else break;
        }
    }
    queue_push(q, STEP_MARK_ALL_SORTED, 0, size - 1);
    free(arr);
}

void gen_selection_sort(StepQueue *q, const int *data, int size) {
    int *arr = malloc(sizeof(int) * size);
    memcpy(arr, data, sizeof(int) * size);
    for (int i = 0; i < size - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < size; j++) {
            queue_push(q, STEP_COMPARE, min_idx, j);
            if (arr[j] < arr[min_idx]) min_idx = j;
        }
        if (min_idx != i) {
            queue_push(q, STEP_SWAP, i, min_idx);
            swap(arr, i, min_idx);
        }
        queue_push(q, STEP_MARK_SORTED, i, -1);
    }
    queue_push(q, STEP_MARK_SORTED, size - 1, -1);
    free(arr);
}

void gen_bubble_sort(StepQueue *q, const int *data, int size) {
    int *arr = malloc(sizeof(int) * size);
    memcpy(arr, data, sizeof(int) * size);
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            queue_push(q, STEP_COMPARE, j, j + 1);
            if (arr[j] > arr[j + 1]) {
                queue_push(q, STEP_SWAP, j, j + 1);
                swap(arr, j, j + 1);
            }
        }
        queue_push(q, STEP_MARK_SORTED, size - i - 1, -1);
    }
    queue_push(q, STEP_MARK_SORTED, 0, -1);
    free(arr);
}
