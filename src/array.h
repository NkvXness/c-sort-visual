#ifndef ARRAY_H
#define ARRAY_H

typedef struct {
    int *data;
    int  size;
    int  max_val;
} Array;

Array *array_create(int size);
void   array_free(Array *arr);
void   array_shuffle(Array *arr);
void   array_resize(Array *arr, int new_size);

#endif
