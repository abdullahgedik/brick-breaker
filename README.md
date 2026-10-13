# Brick Breaker

A classic Brick Breaker clone written in C++17 with [raylib](https://www.raylib.com/). Everything is drawn with simple shapes, so there are no external assets.

## About this project

This is a tutorial project I built for myself to get into raylib and to practise C++. I come from other languages, so the goal was to learn by building something small but complete: a window and game loop, input, collision, game states, and a bit of polish. The code is deliberately kept simple and readable.

## Features

- Paddle controlled with the keyboard, clamped to the window
- Ball that bounces off the walls, the paddle and the bricks
- Paddle bounce angle depends on where the ball lands (centre = straight up, edges = steep angle)
- Side-aware collisions, so the ball reflects correctly off the sides and corners of the paddle and bricks
- 10 x 5 grid of coloured bricks, with score and lives
- Game states: title, playing, game over and win, with restart
- Ball that speeds up as bricks break
- Particle effects when a brick breaks and when the ball is lost
- High score that is saved to a small text file (`highscore.txt`, next to the executable) and loaded the next time you start the game

## Controls

| Key | Action |
|---|---|
| Left / Right or A / D | Move the paddle |
| Space | Start the game, launch the ball, restart after game over or win |
| Esc | Quit |

## Building

You need:

- A C++17 compiler (developed with MinGW-w64 GCC on Windows)
- [CMake](https://cmake.org/) 3.15 or newer
- Git and an internet connection for the first configure, because CMake downloads raylib 5.5 automatically (`FetchContent`) and there is nothing to install by hand

```bash
git clone https://github.com/abdullahgedik/brick-breaker.git
cd brick-breaker
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

Then run `build\game.exe` (or `./build/game` on other platforms). The first configure compiles raylib as well, so it takes a minute or two. Drop the `-G "MinGW Makefiles"` part if you use a different generator, for example Visual Studio or Ninja. I have only tested it on Windows.

## Project structure

```
brick-breaker/
  CMakeLists.txt
  src/
    main.cpp     window, main loop and game state handling
    game.h       constants, types and function declarations
    game.cpp     ball, bricks, collision and particle code
```

## What I practised

- The raylib game loop: input, update and draw kept separate
- Frame-rate independent movement with delta time
- Structs, `std::vector`, references and splitting code into a header and a source file
- Circle-versus-rectangle collision and choosing which side was hit
- A simple state machine with an `enum class`
- Reading and writing a file with raylib's `LoadFileText` / `UnloadFileText` / `SaveFileText`

## Ideas not done yet

- Sound effects
- Splitting each frame into smaller steps so a very fast ball can never skip through a brick
