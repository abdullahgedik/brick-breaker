#include "raylib.h"
#include <cmath> // sinf, cosf

constexpr int SCREEN_W = 800;
constexpr int SCREEN_H = 600;

constexpr float PADDLE_W = 100.0f;
constexpr float PADDLE_H = 16.0f;
constexpr float PADDLE_Y = SCREEN_H - 40.0f;
constexpr float PADDLE_SPEED = 500.0f; // pixels per second

constexpr float BALL_RADIUS = 8.0f;
constexpr float BALL_SPEED = 360.0f;     // total speed after a paddle hit, pixels per second
constexpr float MAX_BOUNCE_ANGLE = 60.0f; // degrees from straight up, at the paddle's edge

struct Ball
{
    Vector2 pos; // center
    Vector2 vel; // pixels per second
    float radius;
};

// A fresh ball in the middle of the screen, heading up and to the right
Ball MakeBall()
{
    Ball ball = {{SCREEN_W / 2.0f, SCREEN_H / 2.0f}, {200.0f, -300.0f}, BALL_RADIUS};
    return ball;
}

int main()
{
    // --- load (Love2D: love.load) ---
    InitWindow(SCREEN_W, SCREEN_H, "Brick Breaker");
    SetTargetFPS(60);

    // x, y, width, height (x/y is the top-left corner)
    Rectangle paddle = {(SCREEN_W - PADDLE_W) / 2.0f, PADDLE_Y, PADDLE_W, PADDLE_H};
    Ball ball = MakeBall();

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

        // move the ball
        ball.pos.x += ball.vel.x * dt;
        ball.pos.y += ball.vel.y * dt;

        // bounce off left, right and top walls
        if (ball.pos.x < ball.radius)
        {
            ball.pos.x = ball.radius;
            ball.vel.x = -ball.vel.x;
        }
        if (ball.pos.x > SCREEN_W - ball.radius)
        {
            ball.pos.x = SCREEN_W - ball.radius;
            ball.vel.x = -ball.vel.x;
        }
        if (ball.pos.y < ball.radius)
        {
            ball.pos.y = ball.radius;
            ball.vel.y = -ball.vel.y;
        }

        // bounce off the paddle (only while moving down, so it can't get stuck inside)
        if (ball.vel.y > 0.0f && CheckCollisionCircleRec(ball.pos, ball.radius, paddle))
        {
            // where on the paddle: -1 = left edge, 0 = center, +1 = right edge
            float paddleCenter = paddle.x + paddle.width / 2.0f;
            float hit = (ball.pos.x - paddleCenter) / (paddle.width / 2.0f);
            if (hit < -1.0f)
            {
                hit = -1.0f;
            }
            if (hit > 1.0f)
            {
                hit = 1.0f;
            }

            float angle = hit * MAX_BOUNCE_ANGLE * DEG2RAD; // 0 = straight up
            ball.vel.x = sinf(angle) * BALL_SPEED;
            ball.vel.y = -cosf(angle) * BALL_SPEED;

            ball.pos.y = paddle.y - ball.radius; // sit on top of the paddle
        }

        // fell off the bottom: start over
        if (ball.pos.y > SCREEN_H + ball.radius)
        {
            ball = MakeBall();
        }

        // --- draw (Love2D: love.draw) ---
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangleRec(paddle, DARKBLUE);
        DrawCircleV(ball.pos, ball.radius, MAROON);
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
