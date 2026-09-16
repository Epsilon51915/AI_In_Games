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
#include <fstream>

/*
    ToDo:
        - Draw Pacman ✓
        - Design ghost AI   CANNOT CHANGE 180 DEGREES UNLESS CHANGING FROM CHASE TO SCATTER
        - Design pacman AI  CAN CHANGE 180 DEGREES WHENEVER DESIRED
        - Playable mode w/ difficulties? -=STRETCH GOAL=-
*/


//***************************//
//                           //
//      Board Management     //
//          Functions        //
//                           //
//***************************//
void DrawBoard(char board[36][28])
{
    std::string charstring;
    for(int row = 0; row < 36; row++)
    {
        for(int col = 0; col < 28; col++)
        {
            charstring = board[row][col];
            if(board[row][col] == '.')
            {
                DrawText(charstring.c_str(), 50 + col * 15, 50 + row * 15, 15, WHITE);
            }
            else if (board[row][col] == 'o')
            {
                DrawText(charstring.c_str(), 50 + col * 15, 50 + row * 15, 15, YELLOW);
            }
            else
            {
                DrawText(charstring.c_str(), 50 + col * 15, 50 + row * 15, 15, BLUE);
            }
        }
    }
}

bool loadBoard(char board[36][28]) 
{
    std::ifstream file("../build/tilemaps/LevelOneTilemap.txt");
    if(!file.is_open())
    {
        std::cerr << "File unable to open" << std::endl;
        return false;
    }

    int col = 0;
    int row = 0;
    char waste;
    char ch;
    while(file.get(ch))
    {
        board[row][col] = ch;
        // if(row < 5)
        // {   
            // std::cout << "row: " << row << " col: " << col << " char: " << ch << std::endl;
        // }
        //std::cout << board[row][col];
        col++;
        if((col % 28) == 0)
        {
            //std::cout << "WASTE" << std::endl;
            file.get(waste); // '\'
            file.get(waste); // 'n'
            col = 0;
            row++;
            //std::cout << std::endl;
        }
        
    }

    return true;
}


int main()
{
    srand(time(nullptr));
    raylib::Window window(530, 650, "PacPlusPlus");
    window.SetTargetFPS(60);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    char board[36][28];
    int score = 100;
    std::string scorestr;

    if(!loadBoard(board))
    {
        return -1;
    }

    while(!window.ShouldClose())
    {
        window.ClearBackground();
        window.BeginDrawing();
        DrawBoard(board);
        scorestr = "Score: " + std::to_string(score);
        DrawText(scorestr.c_str(), 205, 20, 15, WHITE);
        window.EndDrawing();
    }

    return 0;
}