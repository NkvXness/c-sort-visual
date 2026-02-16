#include "ui.h"
#include <stdio.h>

static const Color UI_BG     = {  22,  22,  34, 255 };
static const Color UI_HOVER  = {  42,  45,  70, 255 };
static const Color UI_ACTIVE = {  70, 130, 220, 255 };
static const Color UI_BORDER = {  55,  58,  88, 255 };
static const Color UI_TEXT   = { 210, 212, 230, 255 };
static const Color UI_LABEL  = { 130, 135, 165, 255 };

int button_draw(Button *btn) {
    Vector2 mouse   = GetMousePosition();
    int     hovered = CheckCollisionPointRec(mouse, btn->rect);
    int     clicked = hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    Color bg = btn->active ? UI_ACTIVE : hovered ? UI_HOVER : UI_BG;
    DrawRectangleRec(btn->rect, bg);
    DrawRectangleLinesEx(btn->rect, 1.0f, btn->active ? UI_ACTIVE : UI_BORDER);
    int tw = MeasureText(btn->label, 15);
    int tx = (int)btn->rect.x + ((int)btn->rect.width  - tw) / 2;
    int ty = (int)btn->rect.y + ((int)btn->rect.height - 15) / 2;
    DrawText(btn->label, tx, ty, 15, UI_TEXT);
    return clicked;
}

void slider_draw(Slider *sl) {
    float t      = (sl->value - sl->min) / (sl->max - sl->min);
    float fill_w = t * sl->rect.width;
    DrawRectangleRec(sl->rect, UI_BG);
    DrawRectangleLinesEx(sl->rect, 1.0f, UI_BORDER);
    if (fill_w > 0)
        DrawRectangle((int)sl->rect.x, (int)sl->rect.y,
                      (int)fill_w, (int)sl->rect.height, UI_ACTIVE);
    int hx = (int)(sl->rect.x + fill_w);
    int hy = (int)(sl->rect.y + sl->rect.height / 2.0f);
    DrawCircle(hx, hy, 8.0f, UI_ACTIVE);
    DrawCircle(hx, hy, 4.0f, UI_TEXT);
    Rectangle hit = { sl->rect.x - 8, sl->rect.y - 10,
                      sl->rect.width + 16, sl->rect.height + 20 };
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(mouse, hit)) sl->dragging = 1;
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) sl->dragging = 0;
    if (sl->dragging) {
        float new_t = (mouse.x - sl->rect.x) / sl->rect.width;
        if (new_t < 0.0f) new_t = 0.0f;
        if (new_t > 1.0f) new_t = 1.0f;
        sl->value = sl->min + new_t * (sl->max - sl->min);
    }
    char buf[64];
    snprintf(buf, sizeof(buf), "%s: %.0f", sl->label, sl->value);
    DrawText(buf, (int)sl->rect.x, (int)(sl->rect.y - 18), 13, UI_LABEL);
}
