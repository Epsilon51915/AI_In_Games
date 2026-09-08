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



int main()
{
    srand(time(nullptr));
    raylib::Window window(800, 700, "As8");
    window.SetTargetFPS(60);
    window.SetState(FLAG_WINDOW_RESIZABLE);
    
    int posx = 10;
    int posy = 200;



    int colX = 255; int colY = 0; int colZ = 0;

    bool Xdec = true;
    bool Ydec, Zdec = false;

    while(!window.ShouldClose())
    {
        window.BeginDrawing();
        for(int i = 0; i < 10; i++)
        {
            for(int j = 0; j < 38; j++)
            {
                DrawText("0", posx + 20 * j, posy + 30*i, 30, ColorFromNormalized(raylib::Vector4((float)colX/255, (float)colY/255, (float)colZ/255, 1.0f)));
            }
        }

        if(Xdec)
        {
            colX -= 1;
            colY += 1;
            std::cout << colX << "  " << colY << std::endl;
            if(colX == 0)
            {
                colX = 0;
                Xdec = false;
                Ydec = true;
            }
        }
        else if(Ydec)
        {
            colY -= 1;
            colZ += 1;
            std::cout << colY << "  " << colZ << std::endl;
            if(colY == 0)
            {
                colY = 0;
                Ydec = false;
                Zdec = true;
            }
        }
        else if(Zdec)
        {
            colZ -= 1;
            colX += 1;
            std::cout << colZ << "  " << colX << std::endl;
            if(colZ == 0)
            {
                colZ = 0;
                Zdec = false;
                Xdec = true;
            }
        }
        window.EndDrawing();

        
    }

    

    return 0;
}