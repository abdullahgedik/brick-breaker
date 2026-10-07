#include "raylib.h"

constexpr int SCREEN_W = 800;
constexpr int SCREEN_H = 600;

int main() {
    // --- load (Love2D: love.load) ---
    InitWindow(SCREEN_W, SCREEN_H, "Brick Breaker");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {      // true when X or ESC is pressed
        // --- update (Love2D: love.update) ---
        float dt = GetFrameTime();      // seconds since last frame, unused until milestone 2
        (void)dt;

        // --- draw (Love2D: love.draw) ---
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Brick Breaker", 290, 270, 30, DARKGRAY);
        DrawText("Press ESC to quit", 310, 310, 20, GRAY);
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
