#include "ui.h"

static const Color UI_BG     = {  22,  22,  34, 255 };
static const Color UI_HOVER  = {  42,  45,  70, 255 };
static const Color UI_ACTIVE = {  70, 130, 220, 255 };
static const Color UI_BORDER = {  55,  58,  88, 255 };
static const Color UI_TEXT   = { 210, 212, 230, 255 };

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
