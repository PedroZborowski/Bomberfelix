#include "graficos.h"
#include "raylib.h"

int main(void) {
    InitWindow(800, 600, "Exemplo de Interação");
    SetTargetFPS(60);

    int posX = 400;
    int posY = 300;

    while (!WindowShouldClose()) {
        // Movendo com setas do teclado
        if (IsKeyDown(KEY_RIGHT)) posX += 2;
        if (IsKeyDown(KEY_LEFT)) posX -= 2;
        if (IsKeyDown(KEY_DOWN)) posY += 2;
        if (IsKeyDown(KEY_UP)) posY -= 2;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawCircle(posX, posY, 50, BLUE);
        DrawText("Use as setas para mover o círculo", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}