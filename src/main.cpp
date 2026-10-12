#include "game.h"
#include <cmath> // sinf, cosf, fabsf

int main()
{
    // ----- Load -----
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(SCREEN_W, SCREEN_H, "Brick Breaker");
    SetTargetFPS(60);

    // x, y, width, height (x/y is the top-left corner)
    Rectangle paddle = {(SCREEN_W - PADDLE_W) / 2.0f, PADDLE_Y, PADDLE_W, PADDLE_H};
    Ball ball = MakeBall(GetPaddleCenter(paddle));
    std::vector<Brick> bricks = MakeBricks();

    int lives = 3;
    int score = 0;
    int destroyedBricks = 0;
    int totalBricks = bricks.size();

    GameState state = GameState::Title;

    while (!WindowShouldClose())
    { // true when X or ESC is pressed
        // ----- Update -----
        float dt = GetFrameTime(); // seconds since last frame

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

            // clamp to the window
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

            // bounce off the paddle
            if (!ball.isStuck && CheckCollisionCircleRec(ball.pos, ball.radius, paddle))
            {
                Side side = GetHitSide(ball, paddle, false); // paddle moves too, so overlap only

                if (side == Side::Top && ball.vel.y > 0.0f) // only while moving down, so it can't get stuck inside
                {
                    // where on the paddle: -1 = left edge, 0 = center, +1 = right edge
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

                    float angle = hit * MAX_BOUNCE_ANGLE * DEG2RAD; // 0 = straight up
                    ball.vel.x = sinf(angle) * BALL_SPEED;
                    ball.vel.y = -cosf(angle) * BALL_SPEED;

                    ball.pos.y = paddle.y - ball.radius; // sit on top of the paddle
                }
                else if (side == Side::Left)
                {
                    ball.pos.x = paddle.x - ball.radius; // push out, don't teleport up
                    ball.vel.x = -fabsf(ball.vel.x);     // always away from the paddle
                }
                else if (side == Side::Right)
                {
                    ball.pos.x = paddle.x + paddle.width + ball.radius;
                    ball.vel.x = fabsf(ball.vel.x);
                }
                // Side::Bottom (ball under the paddle) is ignored
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
                    destroyedBricks++;
                    if (destroyedBricks >= totalBricks)
                    {
                        state = GameState::Win;
                    }

                    // send the ball away from the side it hit
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

            // fell off the bottom: start over
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
                // reset everything
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
