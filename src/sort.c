#include "sort.h"
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 8192

/* ── Queue internals ──────────────────────────────────────────────────────── */

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

    /* track stats at generation time so they reflect theoretical counts */
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

/* ── Helper: swap two ints in a local array ───────────────────────────────── */
static void swap(int *arr, int i, int j) {
    int tmp = arr[i];
    arr[i]  = arr[j];
    arr[j]  = tmp;
}

/* ── Bubble Sort ──────────────────────────────────────────────────────────────
   O(n²) comparisons, O(n²) swaps worst case.
   Each pass "bubbles" the largest unsorted element to the end.
   ─────────────────────────────────────────────────────────────────────────── */
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

/* ── Selection Sort ───────────────────────────────────────────────────────────
   O(n²) comparisons, O(n) swaps — good when writes are expensive.
   Finds the minimum of the unsorted portion and places it at the front.
   ─────────────────────────────────────────────────────────────────────────── */
void gen_selection_sort(StepQueue *q, const int *data, int size) {
    int *arr = malloc(sizeof(int) * size);
    memcpy(arr, data, sizeof(int) * size);

    for (int i = 0; i < size - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < size; j++) {
            queue_push(q, STEP_COMPARE, min_idx, j);
            if (arr[j] < arr[min_idx])
                min_idx = j;
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

/* ── Insertion Sort ───────────────────────────────────────────────────────────
   O(n²) worst case, O(n) best case (already sorted).
   Builds a sorted prefix by inserting each element into its correct position.
   ─────────────────────────────────────────────────────────────────────────── */
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
            } else {
                break;
            }
        }
    }
    queue_push(q, STEP_MARK_ALL_SORTED, 0, size - 1);

    free(arr);
}

/* ── Merge Sort (bottom-up iterative) ────────────────────────────────────────
   O(n log n) always.  Bottom-up avoids recursion and maps cleanly to steps.
   Merges increasingly large runs: width=1, 2, 4, 8 …
   ─────────────────────────────────────────────────────────────────────────── */
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

            /* merge arr[lo..mid) with arr[mid..hi) into tmp */
            int l = lo, r = mid, k = lo;
            while (l < mid && r < hi) {
                queue_push(q, STEP_COMPARE, l, r);
                if (arr[l] <= arr[r])
                    tmp[k++] = arr[l++];
                else
                    tmp[k++] = arr[r++];
            }
            while (l < mid) tmp[k++] = arr[l++];
            while (r < hi)  tmp[k++] = arr[r++];

            /* write merged result back; emit a SWAP for each moved element */
            for (int i = lo; i < hi; i++) {
                if (arr[i] != tmp[i]) {
                    queue_push(q, STEP_SWAP, i, i); /* in-place visual update */
                    arr[i] = tmp[i];
                }
            }
        }
    }
    queue_push(q, STEP_MARK_ALL_SORTED, 0, size - 1);

    free(tmp);
    free(arr);
}

/* ── Quick Sort (Lomuto partition, iterative via explicit stack) ──────────────
   O(n log n) average, O(n²) worst case.
   Lomuto: pivot = arr[hi]; partition into ≤pivot | pivot | >pivot.
   We avoid system recursion to keep the step generator straightforward.
   ─────────────────────────────────────────────────────────────────────────── */
void gen_quick_sort(StepQueue *q, const int *data, int size) {
    int *arr   = malloc(sizeof(int) * size);
    int *stack = malloc(sizeof(int) * size * 2); /* [lo, hi] pairs */
    memcpy(arr, data, sizeof(int) * size);

    int top = -1;
    /* push initial range */
    stack[++top] = 0;
    stack[++top] = size - 1;

    while (top >= 0) {
        int hi = stack[top--];
        int lo = stack[top--];
        if (lo >= hi) continue;

        /* partition: pivot is the last element */
        queue_push(q, STEP_SET_PIVOT, hi, -1);
        int pivot = arr[hi];
        int i     = lo - 1;

        for (int j = lo; j < hi; j++) {
            queue_push(q, STEP_COMPARE, j, hi);
            if (arr[j] <= pivot) {
                i++;
                if (i != j) {
                    queue_push(q, STEP_SWAP, i, j);
                    swap(arr, i, j);
                }
            }
        }
        int p = i + 1;
        if (p != hi) {
            queue_push(q, STEP_SWAP, p, hi);
            swap(arr, p, hi);
        }
        queue_push(q, STEP_CLEAR_PIVOT, hi, -1);
        queue_push(q, STEP_MARK_SORTED, p, -1);

        /* push sub-ranges; smaller range first to keep stack usage O(log n) */
        stack[++top] = lo;
        stack[++top] = p - 1;
        stack[++top] = p + 1;
        stack[++top] = hi;
    }

    free(stack);
    free(arr);
}