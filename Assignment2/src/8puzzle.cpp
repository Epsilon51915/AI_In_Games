#include "AudioDevice.hpp"
#include "Camera3D.hpp"
#include "Keyboard.hpp"
#include "Matrix.hpp"
#include "Model.hpp"
#include "RadiansDegrees.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "raylib-cpp.hpp"
#include "raylib.h"
#include <concepts>
#include <execution>
#include <iostream>
#include <limits>
#include <optional>
#include <memory>
#include <vector>
#include <iostream>
#include <map>
#include <utility>

/*
    ToDo:
        - Draw Pacman
        - Design ghost AI
        - Design pacman AI
        - Playable mode w/ difficulties?
*/
int main()
{
    srand(time(nullptr));
    raylib::Window window(800, 700, "PacPlusPlus");
    window.SetTargetFPS(60);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    while(!window.ShouldClose())
    {
        window.ClearBackground();
        window.BeginDrawing();
        window.EndDrawing();
    }

    return 0;
}