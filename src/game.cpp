#include "game.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>

Ball MakeBall(float paddleCenter)
{
    Ball ball = {{paddleCenter, SCREEN_H - 48.0f}, {0.0f, 0.0f}, BALL_RADIUS, true, BALL_SPEED};
    return ball;
}

void LaunchBall(Ball &ball)
{
    ball.isStuck = false;
    ball.vel = {0.0f, -ball.speed};
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

void SpawnParticles(std::vector<Particle> &particles, Vector2 center, Color color, int minAngle, int maxAngle)
{
    for (int i = 0; i < PARTICLE_COUNT; i++)
    {
        float angle = GetRandomValue(minAngle, maxAngle) * DEG2RAD;
        float speed = GetRandomValue(PARTICLE_MIN_SPEED, PARTICLE_MAX_SPEED);
        particles.push_back({center, {cosf(angle) * speed, sinf(angle) * speed}, color, PARTICLE_LIFE});
    }
}

void UpdateParticles(std::vector<Particle> &particles, float dt)
{
    for (Particle &p : particles)
    {
        p.vel.y += PARTICLE_GRAVITY * dt;
        p.pos.x += p.vel.x * dt;
        p.pos.y += p.vel.y * dt;
        p.life -= dt;
    }

    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
                       [](const Particle &p)
                       { return p.life <= 0.0f; }),
        particles.end());
}

void DrawParticles(const std::vector<Particle> &particles)
{
    for (const Particle &p : particles)
    {
        float t = p.life / PARTICLE_LIFE;
        float size = PARTICLE_SIZE * t;
        DrawRectangleV({p.pos.x - size / 2.0f, p.pos.y - size / 2.0f}, {size, size}, Fade(p.color, t));
    }
}

BallUpdate UpdateBall(Ball &ball, const Rectangle &paddle, std::vector<Brick> &bricks, std::vector<Particle> &particles, float dt)
{
    BallUpdate result = {0, false};

    if (ball.isStuck)
    {
        ball.pos.x = GetPaddleCenter(paddle);
        ball.pos.y = paddle.y - ball.radius;
    }

    ball.pos.x += ball.vel.x * dt;
    ball.pos.y += ball.vel.y * dt;

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

    if (!ball.isStuck && CheckCollisionCircleRec(ball.pos, ball.radius, paddle))
    {
        Side side = GetHitSide(ball, paddle, false);

        if (side == Side::Top && ball.vel.y > 0.0f)
        {
            float paddleCenter = GetPaddleCenter(paddle);
            float hit = (ball.pos.x - paddleCenter) / (paddle.width / 2.0f);
            if (hit < -1.0f)
            {
                hit = -1.0f;
            }
            if (hit > 1.0f)
            {
                hit = 1.0f;
            }

            float angle = hit * MAX_BOUNCE_ANGLE * DEG2RAD;
            ball.vel.x = sinf(angle) * ball.speed;
            ball.vel.y = -cosf(angle) * ball.speed;

            ball.pos.y = paddle.y - ball.radius;
        }
        else if (side == Side::Left)
        {
            ball.pos.x = paddle.x - ball.radius;
            ball.vel.x = -fabsf(ball.vel.x);
        }
        else if (side == Side::Right)
        {
            ball.pos.x = paddle.x + paddle.width + ball.radius;
            ball.vel.x = fabsf(ball.vel.x);
        }
    }

    for (Brick &brick : bricks)
    {
        if (!brick.active)
        {
            continue;
        }
        if (CheckCollisionCircleRec(ball.pos, ball.radius, brick.rect))
        {
            brick.active = false;
            result.bricksHit++;
            SpawnParticles(particles, {brick.rect.x + brick.rect.width / 2.0f, brick.rect.y + brick.rect.height / 2.0f}, brick.color, 0, 359);

            ball.speed = fminf(ball.speed + BALL_SPEED_STEP, BALL_SPEED_MAX);
            ball.vel = Vector2Scale(Vector2Normalize(ball.vel), ball.speed);

            switch (GetHitSide(ball, brick.rect, true))
            {
            case Side::Top:
                ball.vel.y = -fabsf(ball.vel.y);
                break;
            case Side::Bottom:
                ball.vel.y = fabsf(ball.vel.y);
                break;
            case Side::Left:
                ball.vel.x = -fabsf(ball.vel.x);
                break;
            case Side::Right:
                ball.vel.x = fabsf(ball.vel.x);
                break;
            }
            break;
        }
    }

    if (ball.pos.y > SCREEN_H + ball.radius)
    {
        result.fellOff = true;
    }

    return result;
}
