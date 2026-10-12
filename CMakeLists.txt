cmake_minimum_required(VERSION 3.15)
project(brick_breaker CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# raylib 5.5 declares an old minimum CMake version; silence that deprecation warning
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)
FetchContent_MakeAvailable(raylib)

add_executable(game src/main.cpp src/game.cpp src/game.h)
target_link_libraries(game raylib)
