#ifndef UI_H
#define UI_H

#include <raylib.h>

typedef struct {
    Rectangle   rect;
    const char *label;
    int         active;
} Button;

int button_draw(Button *btn);

#endif
