#include "array.h"
#include <stdlib.h>
#include <time.h>

static void fisher_yates(int *data, int size) {
    for (int i = size - 1; i > 0; i--) {
        int j   = rand() % (i + 1);
        int tmp = data[i];
        data[i] = data[j];
        data[j] = tmp;
    }
}

Array *array_create(int size) {
    Array *arr = malloc(sizeof(Array));
    arr->size    = size;
    arr->max_val = size;
    arr->data    = malloc(sizeof(int) * size);
    for (int i = 0; i < size; i++)
        arr->data[i] = i + 1;
    srand((unsigned int)time(NULL));
    fisher_yates(arr->data, size);
    return arr;
}

void array_free(Array *arr) {
    free(arr->data);
    free(arr);
}

void array_shuffle(Array *arr) {
    for (int i = 0; i < arr->size; i++)
        arr->data[i] = i + 1;
    fisher_yates(arr->data, arr->size);
}
