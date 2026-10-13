#pragma once
#include "raylib.h"
#include <vector>

constexpr int SCREEN_W = 800;
constexpr int SCREEN_H = 600;

constexpr float PADDLE_W = 100.0f;
constexpr float PADDLE_H = 16.0f;
constexpr float PADDLE_Y = SCREEN_H - 40.0f;
constexpr float PADDLE_SPEED = 500.0f;

constexpr float BALL_RADIUS = 8.0f;
constexpr float BALL_SPEED = 360.0f;
constexpr float MAX_BOUNCE_ANGLE = 60.0f;
constexpr float BALL_SPEED_STEP = 8.0f;
constexpr float BALL_SPEED_MAX = 600.0f;

constexpr int BRICK_COLS = 10;
constexpr int BRICK_ROWS = 5;
constexpr float BRICK_W = 70.0f;
constexpr float BRICK_H = 22.0f;
constexpr float BRICK_GAP = 5.0f;
constexpr float BRICK_TOP = 60.0f;
constexpr int BRICK_POINTS = 10;

enum class GameState
{
    Title,
    Playing,
    GameOver,
    Win
};

enum class Side
{
    Left,
    Right,
    Top,
    Bottom
};

struct Ball
{
    Vector2 pos;
    Vector2 vel;
    float radius;
    bool isStuck;
    float speed;
};

struct Brick
{
    Rectangle rect;
    Color color;
    bool active;
};

Ball MakeBall(float paddleCenter);

void LaunchBall(Ball &ball);

std::vector<Brick> MakeBricks();

void DrawCenteredText(const char *text, int y, int fontSize, Color color);

float GetPaddleCenter(const Rectangle &paddle);

Side GetHitSide(const Ball &ball, const Rectangle &rect, bool useVelocity);
