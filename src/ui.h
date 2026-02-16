#ifndef UI_H
#define UI_H

#include <raylib.h>

typedef struct {
    Rectangle   rect;
    const char *label;
    int         active;
} Button;

typedef struct {
    Rectangle   rect;
    const char *label;
    float       min;
    float       max;
    float       value;
    int         dragging;
} Slider;

int  button_draw(Button *btn);
void slider_draw(Slider *sl);

#endif
