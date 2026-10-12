#include "raylib.h"
#include <cmath> // sinf, cosf
#include <vector>

constexpr int SCREEN_W = 800;
constexpr int SCREEN_H = 600;

constexpr float PADDLE_W = 100.0f;
constexpr float PADDLE_H = 16.0f;
constexpr float PADDLE_Y = SCREEN_H - 40.0f;
constexpr float PADDLE_SPEED = 500.0f; // pixels per second

constexpr float BALL_RADIUS = 8.0f;
constexpr float BALL_SPEED = 360.0f;      // total speed after a paddle hit, pixels per second
constexpr float MAX_BOUNCE_ANGLE = 60.0f; // degrees from straight up, at the paddle's edge

constexpr int BRICK_COLS = 10;
constexpr int BRICK_ROWS = 5;
constexpr float BRICK_W = 70.0f;
constexpr float BRICK_H = 22.0f;
constexpr float BRICK_GAP = 5.0f;
constexpr float BRICK_TOP = 60.0f; // space above the first row, for the HUD
constexpr int BRICK_POINTS = 10;

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

struct Brick
{
    Rectangle rect;
    Color color;
    bool active;
};

// Builds the whole grid, centered horizontally, one color per row
std::vector<Brick> MakeBricks()
{
    const Color rowColors[BRICK_ROWS] = {RED, ORANGE, GOLD, GREEN, SKYBLUE};

    float gridW = BRICK_COLS * BRICK_W + (BRICK_COLS - 1) * BRICK_GAP;
    float startX = (SCREEN_W - gridW) / 2.0f;

    std::vector<Brick> bricks;
    for (int row = 0; row < BRICK_ROWS; row++)
    {
        for (int col = 0; col < BRICK_COLS; col++)
        {
            Rectangle rect = {
                startX + col * (BRICK_W + BRICK_GAP),
                BRICK_TOP + row * (BRICK_H + BRICK_GAP),
                BRICK_W,
                BRICK_H};
            bricks.push_back({rect, rowColors[row], true});
        }
    }
    return bricks;
}

int main()
{
    // ----- Load -----
    InitWindow(SCREEN_W, SCREEN_H, "Brick Breaker");
    SetTargetFPS(60);

    // x, y, width, height (x/y is the top-left corner)
    Rectangle paddle = {(SCREEN_W - PADDLE_W) / 2.0f, PADDLE_Y, PADDLE_W, PADDLE_H};
    Ball ball = MakeBall();
    std::vector<Brick> bricks = MakeBricks();
    int score = 0;

    while (!WindowShouldClose())
    { // true when X or ESC is pressed
        // ----- Update -----
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

        // bounce off bricks (at most one brick per frame)
        for (Brick &brick : bricks)
        {
            if (!brick.active)
            {
                continue;
            }
            if (CheckCollisionCircleRec(ball.pos, ball.radius, brick.rect))
            {
                brick.active = false;
                score += BRICK_POINTS;

                // ball center above/below the brick -> hit top or bottom, otherwise a side
                if (ball.pos.x >= brick.rect.x && ball.pos.x <= brick.rect.x + brick.rect.width)
                {
                    ball.vel.y = -ball.vel.y;
                }
                else
                {
                    ball.vel.x = -ball.vel.x;
                }
                break;
            }
        }

        // fell off the bottom: start over
        if (ball.pos.y > SCREEN_H + ball.radius)
        {
            ball = MakeBall();
        }

        // ----- Draw -----
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (const Brick &brick : bricks)
        {
            if (brick.active)
            {
                DrawRectangleRec(brick.rect, brick.color);
            }
        }
        DrawRectangleRec(paddle, DARKBLUE);
        DrawCircleV(ball.pos, ball.radius, MAROON);
        DrawText(TextFormat("Score: %d", score), 10, 10, 24, DARKGRAY);
        DrawFPS(10, SCREEN_H - 24);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
