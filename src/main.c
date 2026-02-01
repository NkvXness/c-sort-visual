#include <raylib.h>

int main(void) {
    InitWindow(1280, 720, "c-sort-visual");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground((Color){ 10, 10, 16, 255 });
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
