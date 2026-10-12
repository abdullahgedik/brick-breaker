#include "game.h"
#include <cmath> // fabsf, fmaxf

Ball MakeBall(float paddleCenter)
{
    Ball ball = {{paddleCenter, SCREEN_H - 48.0f}, {0.0f, 0.0f}, BALL_RADIUS, true};
    return ball;
}

void LaunchBall(Ball &ball)
{
    ball.isStuck = false;
    ball.vel = {0.0f, -BALL_SPEED};
}

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

void DrawCenteredText(const char *text, int y, int fontSize, Color color)
{
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, (SCREEN_W - textWidth) / 2, y, fontSize, color);
}

float GetPaddleCenter(const Rectangle &paddle)
{
    return paddle.x + paddle.width / 2.0f;
}

Side GetHitSide(const Ball &ball, const Rectangle &rect, bool useVelocity)
{
    float overlapLeft = (ball.pos.x + ball.radius) - rect.x;
    float overlapRight = (rect.x + rect.width) - (ball.pos.x - ball.radius);
    float overlapTop = (ball.pos.y + ball.radius) - rect.y;
    float overlapBottom = (rect.y + rect.height) - (ball.pos.y - ball.radius);

    if (useVelocity)
    {
        // fmaxf keeps the speed at 1 or more, so we never divide by zero
        float speedX = fmaxf(fabsf(ball.vel.x), 1.0f);
        float speedY = fmaxf(fabsf(ball.vel.y), 1.0f);
        overlapLeft /= speedX;
        overlapRight /= speedX;
        overlapTop /= speedY;
        overlapBottom /= speedY;
    }

    Side side = Side::Left;
    float smallest = overlapLeft;

    if (overlapRight < smallest)
    {
        smallest = overlapRight;
        side = Side::Right;
    }
    if (overlapTop < smallest)
    {
        smallest = overlapTop;
        side = Side::Top;
    }
    if (overlapBottom < smallest)
    {
        smallest = overlapBottom;
        side = Side::Bottom;
    }
    return side;
}
