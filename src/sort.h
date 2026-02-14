#ifndef SORT_H
#define SORT_H

typedef enum {
    STEP_COMPARE,
    STEP_SWAP,
    STEP_SET_PIVOT,
    STEP_CLEAR_PIVOT,
    STEP_MARK_SORTED,
    STEP_MARK_ALL_SORTED,
} StepType;

typedef struct {
    StepType type;
    int      a;
    int      b;
} SortStep;

typedef struct {
    SortStep *steps;
    int       count;
    int       capacity;
    int       head;
    int       comparisons;
    int       swaps;
} StepQueue;

StepQueue *queue_create(void);
void       queue_free(StepQueue *q);
void       queue_push(StepQueue *q, StepType type, int a, int b);
int        queue_pop(StepQueue *q, SortStep *out);
int        queue_done(const StepQueue *q);

void gen_bubble_sort(StepQueue *q, const int *data, int size);
void gen_selection_sort(StepQueue *q, const int *data, int size);
void gen_insertion_sort(StepQueue *q, const int *data, int size);
void gen_merge_sort(StepQueue *q, const int *data, int size);
void gen_quick_sort(StepQueue *q, const int *data, int size);

#endif
