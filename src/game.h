#pragma once
#include "raylib.h"
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
    Vector2 pos; // center
    Vector2 vel; // pixels per second
    float radius;
    bool isStuck;
};

struct Brick
{
    Rectangle rect;
    Color color;
    bool active;
};

// A fresh ball is stuck to the paddle, ready to launch
Ball MakeBall(float paddleCenter);

void LaunchBall(Ball &ball);

// Builds the whole grid, centered horizontally, one color per row
std::vector<Brick> MakeBricks();

void DrawCenteredText(const char *text, int y, int fontSize, Color color);

float GetPaddleCenter(const Rectangle &paddle);

// Which side of the rectangle did the ball hit? Call this only when they overlap.
// The side the ball sticks into the least is the side it came from.
//
// useVelocity = true is for rectangles that don't move (bricks). It divides each overlap
// by the ball's speed on that axis, giving "how long ago did the ball cross this edge".
// A ball moving almost straight up then always counts as a top/bottom hit, even when
// it also touches a neighbor's side.
Side GetHitSide(const Ball &ball, const Rectangle &rect, bool useVelocity);
