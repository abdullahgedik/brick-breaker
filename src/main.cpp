#include "game.h"
#include <cmath>
#include "raymath.h"

int main()
{
    // ----- Load -----
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(SCREEN_W, SCREEN_H, "Brick Breaker");
    SetTargetFPS(60);

    Rectangle paddle = {(SCREEN_W - PADDLE_W) / 2.0f, PADDLE_Y, PADDLE_W, PADDLE_H};
    Ball ball = MakeBall(GetPaddleCenter(paddle));
    std::vector<Brick> bricks = MakeBricks();

    int lives = 3;
    int score = 0;
    int destroyedBricks = 0;
    int totalBricks = bricks.size();

    GameState state = GameState::Title;

    while (!WindowShouldClose())
    {
        // ----- Update -----
        float dt = fminf(GetFrameTime(), 1.0f / 30.0f);

        switch (state)
        {
        case GameState::Title:
            if (IsKeyPressed(KEY_SPACE))
            {
                state = GameState::Playing;
            }
            break;

        case GameState::Playing:
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
            {
                paddle.x -= PADDLE_SPEED * dt;
            }
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
            {
                paddle.x += PADDLE_SPEED * dt;
            }
            if (IsKeyPressed(KEY_SPACE) && ball.isStuck)
            {
                LaunchBall(ball);
            }

            if (paddle.x < 0.0f)
            {
                paddle.x = 0.0f;
            }
            if (paddle.x > SCREEN_W - paddle.width)
            {
                paddle.x = SCREEN_W - paddle.width;
            }

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
                    score += BRICK_POINTS;
                    destroyedBricks++;
                    if (destroyedBricks >= totalBricks)
                    {
                        state = GameState::Win;
                    }

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
                ball = MakeBall(GetPaddleCenter(paddle));
                lives--;
                if (lives <= 0 && state == GameState::Playing)
                {
                    state = GameState::GameOver;
                }
            }
            break;

        case GameState::GameOver:
        case GameState::Win:
            if (IsKeyPressed(KEY_SPACE))
            {
                paddle.x = (SCREEN_W - PADDLE_W) / 2.0f;
                ball = MakeBall(GetPaddleCenter(paddle));
                bricks = MakeBricks();
                lives = 3;
                score = 0;
                destroyedBricks = 0;
                totalBricks = bricks.size();
                state = GameState::Playing;
            }
            break;
        }

        // ----- Draw -----
        BeginDrawing();
        ClearBackground(RAYWHITE);
        switch (state)
        {
        case GameState::Title:
            DrawCenteredText("Brick Breaker", 250, 40, DARKGRAY);
            DrawCenteredText("Press SPACE to start", 320, 24, BLUE);
            break;

        case GameState::Playing:
            for (const Brick &brick : bricks)
            {
                if (brick.active)
                {
                    DrawRectangleRec(brick.rect, brick.color);
                }
            }
            DrawRectangleRec(paddle, DARKBLUE);
            DrawCircleV(ball.pos, ball.radius, MAROON);
            DrawText(TextFormat("Lives: %d", lives), 10, 10, 24, DARKGRAY);
            DrawCenteredText(TextFormat("Score: %d", score), 10, 24, DARKGRAY);
            DrawFPS(10, SCREEN_H - 24);
            break;

        case GameState::GameOver:
            DrawCenteredText("Game Over", 250, 40, RED);
            DrawCenteredText("Press SPACE to restart", 320, 24, DARKGRAY);
            break;

        case GameState::Win:
            DrawCenteredText("You Win!", 250, 40, GREEN);
            DrawCenteredText("Press SPACE to restart", 320, 24, DARKGRAY);
            DrawCenteredText(TextFormat("Score: %d", score), 380, 24, DARKGRAY);
            break;
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
