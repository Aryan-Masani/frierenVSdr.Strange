#include "raylib.h"

int main() {
    // Window setup
    InitWindow(800, 600, "Pixel Art Mage");
    SetTargetFPS(60);

    // Load sprite (make a small pixel-art PNG like 32x32 or 64x64)
    Texture2D mage = LoadTexture("mage.png");

    // Important: keep pixel-art sharp
    SetTextureFilter(mage, TEXTURE_FILTER_POINT);

    // Mage position
    Vector2 pos = {400, 300};

    while (!WindowShouldClose()) {
        // Movement
        if (IsKeyDown(KEY_RIGHT)) pos.x += 2;
        if (IsKeyDown(KEY_LEFT))  pos.x -= 2;
        if (IsKeyDown(KEY_UP))    pos.y -= 2;
        if (IsKeyDown(KEY_DOWN))  pos.y += 2;

        // Drawing
        BeginDrawing();
            ClearBackground(BLACK);

            // Draw the mage sprite at position, scaled up 4x
            DrawTextureEx(mage, pos, 0.0f, 4.0f, WHITE);

            DrawText("Move with arrow keys", 10, 10, 20, RAYWHITE);
        EndDrawing();
    }

    // Unload resources
    UnloadTexture(mage);
    CloseWindow();

    return 0;
}



