#include "game.h"
#include <cmath>

int main()
{
    // ----- Load -----
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);
    InitWindow(SCREEN_W, SCREEN_H, "Brick Breaker");
    SetTargetFPS(60);

    Rectangle paddle = {(SCREEN_W - PADDLE_W) / 2.0f, PADDLE_Y, PADDLE_W, PADDLE_H};
    Ball ball = MakeBall(GetPaddleCenter(paddle));
    std::vector<Brick> bricks = MakeBricks();
    std::vector<Particle> particles;

    int lives = 3;
    int score = 0;
    int destroyedBricks = 0;
    int totalBricks = bricks.size();
    int highScore = LoadHighScore();
    bool newHighScore = false;

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
        {
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

            BallUpdate result = UpdateBall(ball, paddle, bricks, particles, dt);

            score += result.bricksHit * BRICK_POINTS;
            if (score > highScore)
            {
                highScore = score;
                newHighScore = true;
            }
            destroyedBricks += result.bricksHit;
            if (destroyedBricks >= totalBricks)
            {
                state = GameState::Win;
            }

            if (result.fellOff)
            {
                SpawnParticles(particles, {ball.pos.x, SCREEN_H - 2.0f}, MAROON, 200, 340);
                ball = MakeBall(GetPaddleCenter(paddle));
                lives--;
                if (lives <= 0 && state == GameState::Playing)
                {
                    state = GameState::GameOver;
                }
            }

            if (state != GameState::Playing)
            {
                SaveHighScore(highScore);
            }

            UpdateParticles(particles, dt);
            break;
        }

        case GameState::GameOver:
        case GameState::Win:
            if (IsKeyPressed(KEY_SPACE))
            {
                paddle.x = (SCREEN_W - PADDLE_W) / 2.0f;
                ball = MakeBall(GetPaddleCenter(paddle));
                bricks = MakeBricks();
                particles.clear();
                lives = 3;
                score = 0;
                newHighScore = false;
                destroyedBricks = 0;
                totalBricks = bricks.size();
                state = GameState::Playing;
            }

            UpdateParticles(particles, dt);
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
            DrawCenteredText(TextFormat("Best: %d", highScore), 380, 24, DARKGRAY);
            break;

        case GameState::Playing:
            for (const Brick &brick : bricks)
            {
                if (brick.active)
                {
                    DrawRectangleRec(brick.rect, brick.color);
                }
            }
            DrawParticles(particles);
            DrawRectangleRec(paddle, DARKBLUE);
            DrawCircleV(ball.pos, ball.radius, MAROON);
            DrawText(TextFormat("Lives: %d", lives), 10, 10, 24, DARKGRAY);
            DrawCenteredText(TextFormat("Score: %d", score), 10, 24, DARKGRAY);
            DrawRightText(TextFormat("Best: %d", highScore), 10, 24, DARKGRAY);
            DrawFPS(10, SCREEN_H - 24);
            break;

        case GameState::GameOver:
            DrawParticles(particles);
            DrawCenteredText("Game Over", 250, 40, RED);
            DrawCenteredText("Press SPACE to restart", 320, 24, DARKGRAY);
            DrawCenteredText(TextFormat("Score: %d", score), 380, 24, DARKGRAY);
            if (newHighScore)
            {
                DrawCenteredText("New high score!", 420, 24, GOLD);
            }
            else
            {
                DrawCenteredText(TextFormat("Best: %d", highScore), 420, 24, DARKGRAY);
            }
            break;

        case GameState::Win:
            DrawParticles(particles);
            DrawCenteredText("You Win!", 250, 40, GREEN);
            DrawCenteredText("Press SPACE to restart", 320, 24, DARKGRAY);
            DrawCenteredText(TextFormat("Score: %d", score), 380, 24, DARKGRAY);
            if (newHighScore)
            {
                DrawCenteredText("New high score!", 420, 24, GOLD);
            }
            else
            {
                DrawCenteredText(TextFormat("Best: %d", highScore), 420, 24, DARKGRAY);
            }
            break;
        }
        EndDrawing();
    }

    SaveHighScore(highScore);
    CloseWindow();
    return 0;
}
