#include "array.h"
#include <stdlib.h>
#include <time.h>

Array *array_create(int size) {
    Array *arr = malloc(sizeof(Array));
    arr->size    = size;
    arr->max_val = size;
    arr->data    = malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++)
        arr->data[i] = i + 1;
    srand((unsigned int)time(NULL));
    return arr;
}

void array_free(Array *arr) {
    free(arr->data);
    free(arr);
}
