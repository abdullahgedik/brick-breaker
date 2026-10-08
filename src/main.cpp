#include "raylib.h"

constexpr int SCREEN_W = 800;
constexpr int SCREEN_H = 600;

constexpr float PADDLE_W = 100.0f;
constexpr float PADDLE_H = 16.0f;
constexpr float PADDLE_Y = SCREEN_H - 40.0f;
constexpr float PADDLE_SPEED = 500.0f; // pixels per second

int main()
{
    // --- load (Love2D: love.load) ---
    InitWindow(SCREEN_W, SCREEN_H, "Brick Breaker");
    SetTargetFPS(60);

    // x, y, width, height (x/y is the top-left corner)
    Rectangle paddle = {(SCREEN_W - PADDLE_W) / 2.0f, PADDLE_Y, PADDLE_W, PADDLE_H};

    while (!WindowShouldClose())
    { // true when X or ESC is pressed
        // --- update (Love2D: love.update) ---
        float dt = GetFrameTime(); // seconds since last frame

        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        {
            paddle.x -= PADDLE_SPEED * dt;
        }
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        {
            paddle.x += PADDLE_SPEED * dt;
        }

        // clamp to the window
        if (paddle.x < 0.0f)
        {
            paddle.x = 0.0f;
        }
        if (paddle.x > SCREEN_W - paddle.width)
        {
            paddle.x = SCREEN_W - paddle.width;
        }

        // --- draw (Love2D: love.draw) ---
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangleRec(paddle, DARKBLUE);
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
