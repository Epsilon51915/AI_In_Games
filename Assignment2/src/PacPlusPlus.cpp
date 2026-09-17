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

enum PacState
{
    REGULAR,
    POWERUP
};

enum GhostState
{
    CHASE,
    SCATTER,
    FRIGHTENED
};

enum Ghost
{
    INKY,
    PINKY,
    BLINKY,
    CLYDE
};


struct Pacman
{
    int x_pos;
    int y_pos;
    char draw_char;
    PacState state = REGULAR;
};

struct Enemy
{
    int x_pos;
    int y_pos;
    char draw_char;
    GhostState state = CHASE;
    Ghost type;
};
/*
    ToDo:
        - Draw Pacman ✓
        - Design ghost AI   CANNOT CHANGE 180 DEGREES UNLESS CHANGING FROM CHASE TO SCATTER, SLOWER THAN PLAYER WHEN FLEEING
        - Design pacman AI  CAN CHANGE 180 DEGREES WHENEVER DESIRED
        - Playable mode w/ difficulties? -=STRETCH GOAL=-
*/

//***************************//
//                           //
//         PacMan AI         //
//          Functions        //
//                           //
//***************************//
// Implement A*, returning a direction for the pacman to move.

// Refactor once AI is functional to change position based on an input provided to function by pathing algorithms

void getInput(Pacman &pac)
{
    if(raylib::Keyboard::IsKeyPressed(KEY_A))
    {
        pac.x_pos--;
    }
    else if(raylib::Keyboard::IsKeyPressed(KEY_D))
    {
        pac.x_pos++;
    }
    else if(raylib::Keyboard::IsKeyPressed(KEY_W))
    {
        pac.y_pos--;
    }
    else if(raylib::Keyboard::IsKeyPressed(KEY_S))
    {
        pac.y_pos++;
    }
}

//***************************//
//                           //
//          Ghost AI         //
//          Functions        //
//                           //
//***************************//

void generalGhostAI(Enemy ghost, Pacman pac)
{
    // Chase pacman based on certain "personality" traits
    if(ghost.state == CHASE)
    {
        if(ghost.type == INKY)
        {

        }
        else if(ghost.type == PINKY)
        {

        }
        else if(ghost.type == BLINKY)
        {

        }
        else
        {

        }
    }
    // Move to assigned corner
    else if(ghost.state == SCATTER)
    {
        if(ghost.type == INKY)
        {

        }
        else if(ghost.type == PINKY)
        {

        }
        else if(ghost.type == BLINKY)
        {

        }
        else
        {

        }
    }
    // Frightened, run away from pacman
    else
    {
        if(ghost.type == INKY)
        {

        }
        else if(ghost.type == PINKY)
        {

        }
        else if(ghost.type == BLINKY)
        {

        }
        else
        {

        }
    }
    
}

//***************************//
//                           //
//      Board Management     //
//          Functions        //
//                           //
//***************************//
void DrawBoard(char board[36][28], Pacman &pac, int &score)
{
    std::string charstring;
    for(int row = 0; row < 36; row++)
    {
        for(int col = 0; col < 28; col++)
        {
            charstring = board[row][col];
            if(board[row][col] == '.')
            {
                if(pac.x_pos == col && pac.y_pos == row)
                {
                    board[row][col] = ' ';
                    score += 10;
                }
                DrawText(charstring.c_str(), 50 + col * 26, 50 + row * 26, 26, WHITE);
            }
            else if (board[row][col] == 'o')
            {
                if(pac.x_pos == col && pac.y_pos == row)
                {
                    board[row][col] = ' ';
                    score += 50;
                    pac.state = POWERUP;
                }
                DrawText(charstring.c_str(), 50 + col * 26, 50 + row * 26, 26, YELLOW);
            }
            else if(board[row][col] == 'X'){}
            else
            {
                DrawText(charstring.c_str(), 50 + col * 26, 50 + row * 26, 26, DARKBLUE);
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

void drawPac(Pacman &pac, int frame_counter)
{
    if(frame_counter % 15 == 0)
    {
        if(pac.draw_char == 'o')
        {
            pac.draw_char = 'c';
        }
        else
        {
            pac.draw_char ='o';
        }
    }
    std::string draw(1, pac.draw_char);
    DrawText(draw.c_str(), 50 + pac.x_pos * 26, 50 + pac.y_pos * 26, 26, YELLOW);
}


int main()
{
    srand(time(nullptr));
    //x, y
    raylib::Window window(800, 1200, "PacPlusPlus");
    window.SetTargetFPS(60);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    char board[36][28];
    int score = 0;
    std::string scorestr;

    Pacman pac;
    pac.x_pos = 14;
    pac.y_pos = 26;
    pac.draw_char = 'o';
    int frame_counter = 0;

    if(!loadBoard(board))
    {
        return -1;
    }

    while(!window.ShouldClose())
    {
        window.ClearBackground();
        window.BeginDrawing();
        frame_counter++;
        DrawBoard(board, pac, score);
        drawPac(pac, frame_counter);
        if(frame_counter == 60)
        {
            frame_counter = 0;
        }
        getInput(pac);
        scorestr = "Score: " + std::to_string(score);
        DrawText(scorestr.c_str(), 205, 20, 15, WHITE);
        window.EndDrawing();
        
    }
    //█ ■

    return 0;
}