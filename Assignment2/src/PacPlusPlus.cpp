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
#include <vector>
#include <list>
#include <unordered_set>
#include <cmath>

#define CYAN ColorFromHSV(180, 1, 1);

enum PacState
{
    REGULAR,
    POWERUP
};

enum GhostState
{
    CHASE,
    SCATTER,
    FRIGHTENED,
    EATEN
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
    int ghosts_eaten = 0;
};

struct Enemy
{
    int x_pos;
    int y_pos;
    char draw_char;
    GhostState state = CHASE;
    Ghost type;
    bool dead = true;
    int dead_counter = 3;
};

struct Tile
{
    // Position of each tile on the board
    int x_pos, y_pos;

    // Tile that we traveled from to get to this tile
    Tile* prev_tile;

    // Value of current tile for A* search
    int value;

    // List of all 4 cardinally adjacent tiles (UP, RIGHT, DOWN, LEFT)
    Tile *adj_tile[4];

    // Number of moves to reach this tile
    int num_moves;

    ~Tile()
    {
        for(int i = 0; i < 4; i++)
        {
            if(adj_tile[i] != nullptr)
            {
                delete adj_tile[i];
            }
        }
    }
};
/*
    ToDo:
        - Draw Pacman ✓
        - Design ghost AI   CANNOT CHANGE 180 DEGREES UNLESS CHANGING FROM CHASE TO SCATTER, SLOWER THAN PLAYER WHEN FLEEING ✓
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
    if(raylib::Keyboard::IsKeyDown(KEY_A))
    {
        pac.x_pos--;
    }
    else if(raylib::Keyboard::IsKeyDown(KEY_D))
    {
        pac.x_pos++;
    }
    else if(raylib::Keyboard::IsKeyDown(KEY_W))
    {
        pac.y_pos--;
    }
    else if(raylib::Keyboard::IsKeyDown(KEY_S))
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

// Can use similar logic for pacman, except instead of looking for pac position, look for pellet positions

// Need a parent function that holds current ghost tile and can constantly check for next moves. Do logic in helper fcn.
bool findMoveHelper(Pacman pac, Tile *cur_tile, char board[36][28])
{
    /*
        if board at ghost pos +- 1 is valid:
            next tile value = cur_tile value + 1 + distance from pacman
            update next tile pos values
        else:
            next tile value = INFINITY
        add next tile to cur tile adj list
        compare tile pos with pacman pos
        if equal:
            return TRUE
        else:
            return FALSE
    */
    //std::cout << "Start find move helper" << std::endl;
    if(cur_tile->x_pos == pac.x_pos && cur_tile->y_pos == pac.y_pos)
    {
        //std::cout << "FORWARDS" << std::endl;
        return true;
    }
    //std::cout << "Pac not found" << std::endl;
    for(int i = 0; i < 4; i++)
    {
        if(i % 2 == 0)
        {   
            int new_x_pos = cur_tile->x_pos + (i - 1);
            char tile = board[cur_tile->y_pos][new_x_pos];
            if(tile == 'X' || tile == '.' || tile == 'o' || tile == ' ')
            {
                Tile* new_tile = new Tile;
                new_tile->prev_tile = cur_tile;
                new_tile->x_pos = new_x_pos;
                new_tile->y_pos = cur_tile->y_pos;
                new_tile->value = cur_tile->num_moves + 1 + pow((abs(pac.x_pos - new_x_pos) + abs(pac.y_pos - new_tile->y_pos)), 2);
                new_tile->num_moves = cur_tile->num_moves + 1;
                cur_tile->adj_tile[i] = new_tile;
            }
            else
            {
                cur_tile->adj_tile[i] = nullptr;
            }
        }
        else
        {
            int new_y_pos = cur_tile->y_pos + (i - 2);
            char tile = board[new_y_pos][cur_tile->x_pos];
            if(tile == 'X' || tile == '.' || tile == 'o' || tile == ' ')
            {
                Tile* new_tile = new Tile;
                new_tile->prev_tile = cur_tile;
                new_tile->x_pos = cur_tile->x_pos;
                new_tile->y_pos = new_y_pos;
                new_tile->value = cur_tile->num_moves + 1 + pow((abs(pac.x_pos - new_tile->x_pos) + abs(pac.y_pos - new_tile->y_pos)), 2);
                new_tile->num_moves = cur_tile->num_moves + 1;
                cur_tile->adj_tile[i] = new_tile;
            }
            else
            {
                cur_tile->adj_tile[i] = nullptr;
            }
        }
    }

    return false;
}

void findMove(Enemy &ghost, Pacman pac, Tile *cur_tile, char board[36][28])
{
    if(ghost.x_pos == pac.x_pos && ghost.y_pos == pac.y_pos)
    {
        return;
    }
    /*
        while (call to helper function deos not return pacman pos):
            create queue of all adj tiles, possibly using a set of tile pos with values, to ensure no looping
            organize queue by lowest adj tile cost
            call helper function with argument of lowest tile cost
            if tie, pick first from queue
    */

    //std::cout << "Inside find move" << std::endl;
    int start_x = ghost.x_pos;
    int start_y = ghost.y_pos;

    std::cout << "START" << start_x << "," << start_y << std::endl;
    Tile* cur_search_tile = cur_tile;
    std::list<Tile*> all_tiles;
    all_tiles.push_back(cur_search_tile);
    bool inserted = false;
    std::string tile;
    std::unordered_set<std::string> visited;
    std::ofstream output("output.txt");
    //std::cout << "Start while loop" << std::endl;
    while(!all_tiles.empty() && !findMoveHelper(pac, cur_search_tile, board))
    {
        for(int i = 0; i < 4; i++)
        {
            //std::cout << i << std::endl;
            if(cur_search_tile->adj_tile[i] != nullptr)
            {
                tile = "";
                tile += std::to_string(cur_search_tile->adj_tile[i]->x_pos);
                tile += ",";
                tile += std::to_string(cur_search_tile->adj_tile[i]->y_pos);
                output << "Adjacent to " << cur_search_tile->x_pos << "," << cur_search_tile->y_pos << ":" << tile << std::endl;
                if(visited.contains(tile))
                {
                    continue;
                }
                visited.insert(tile);
                auto it = all_tiles.begin();
                for(auto tile : all_tiles)
                {
                    if(tile->value >= cur_search_tile->adj_tile[i]->value)
                    {
                        all_tiles.insert(it, cur_search_tile->adj_tile[i]);
                        inserted = true;
                        break;
                    }
                    std::advance(it, 1);
                }
                if(!inserted)
                {
                    all_tiles.push_back(cur_search_tile->adj_tile[i]);
                }
                inserted = false;
            }
        }
        cur_search_tile = all_tiles.front();
        auto it = all_tiles.begin();
        all_tiles.erase(it);
        //std::cout << "Looped" << std::endl;
        
        output << "New Search Tile X: " << cur_search_tile->x_pos << " New Search Tile Y: " << cur_search_tile->y_pos << std::endl;
    }

    // pac position found at cur_searech_tile
    
    while(cur_search_tile->prev_tile->x_pos != start_x || cur_search_tile->prev_tile->y_pos != start_y)
    {
        std::cout << "PREV TILE FROM " << cur_search_tile->x_pos << "," << cur_search_tile->y_pos << ": " << 
        cur_search_tile->prev_tile->x_pos << "," << cur_search_tile->prev_tile->y_pos << std::endl; 

        cur_search_tile = cur_search_tile->prev_tile;
    }

    if(ghost.state == FRIGHTENED)
    {
        int bad_x = cur_search_tile->x_pos;
        int bad_y = cur_search_tile->y_pos;
        int new_x;
        int new_y;

        for(int i = 0; i < 4; i++)
        {
            if(cur_tile->adj_tile[i] != nullptr)
            {
                new_x = cur_tile->adj_tile[i]->x_pos;
                new_y = cur_tile->adj_tile[i]->y_pos;

                if(new_x == bad_x && new_y == bad_y)
                {
                    continue;
                }
                else
                {
                    ghost.x_pos = new_x;
                    ghost.y_pos = new_y;
                    break;
                }
            }
            
        }
    }
    else
    {
        std::cout << cur_search_tile->x_pos << " " << cur_search_tile->y_pos << std::endl;
        ghost.x_pos = cur_search_tile->x_pos;
        ghost.y_pos = cur_search_tile->y_pos;
    }
    // cur_search_tile is the next tile we need to move to

    
}

void generalGhostAI(Enemy &ghost, Pacman pac, char board[36][28])
{
    if(ghost.dead)
    {
        ghost.dead_counter --;
        if(ghost.dead_counter == 0)
        {
            ghost.dead = false;
            ghost.x_pos = 14;
            ghost.y_pos = 14;
        }
        return;
    }
    Tile* cur_tile = new Tile;
    cur_tile->x_pos = ghost.x_pos;
    cur_tile->y_pos = ghost.y_pos;
    cur_tile->num_moves = 0;
    cur_tile->value = 0;
    // Chase pacman based on certain "personality" traits
    if(ghost.state == CHASE)
    {
        std::cout << "Start find move" << std::endl;
        findMove(ghost, pac, cur_tile, board);
        std::cout << ghost.x_pos << " " << ghost.y_pos << std::endl;
        std::cout << "End find move" << std::endl;
        // UNIMPLEMENTED
        /*if(ghost.type == INKY)
        {

        }
        else if(ghost.type == PINKY)
        {
            // Target pacman's position plus 4 tiles in front of pacman
        }
        else if(ghost.type == BLINKY)
        {
            // Target pacman's exact position
        }
        else
        {
            // Clyde, target pacman until within 8 tiles, then retreat to bottom right corner
        }*/
    }
    // Move to assigned corner
    else if(ghost.state == SCATTER)
    {
        if(ghost.type == INKY)
        {
            Pacman lr;
            lr.x_pos = 26;
            lr.y_pos = 32; 
            findMove(ghost, lr, cur_tile, board);
        }
        else if(ghost.type == PINKY)
        {
            // Upper Left
            Pacman ul;
            ul.x_pos = 1;
            ul.y_pos = 4; 
            findMove(ghost, ul, cur_tile, board);
        }
        else if(ghost.type == BLINKY)
        {
            // Upper Right
            Pacman ur;
            ur.x_pos = 26;
            ur.y_pos = 4; 
            findMove(ghost, ur, cur_tile, board);
        }
        else
        {
            // Lower Left
            Pacman ll;
            ll.x_pos = 1;
            ll.y_pos = 32; 
            findMove(ghost, ll, cur_tile, board);
        }
    }
    // Frightened, run away from pacman
    else if(ghost.state == FRIGHTENED)
    {
        // 1. Find PacMan
        // 2. Run away 
        findMove(ghost, pac, cur_tile, board);
    }
    // Eaten, return to base
    else
    {
        Pacman base;
        base.x_pos = 14;
        base.y_pos = 14;
        findMove(ghost, base, cur_tile, board);
        if(ghost.x_pos == base.x_pos && ghost.y_pos == base.y_pos)
        {
            ghost.y_pos = 17;
            ghost.dead = true;
            ghost.dead_counter = 6;
            ghost.state = CHASE;
        }
    }
    
}

void runGhostAI(Enemy &blinky, Enemy &inky, Enemy &pinky, Enemy &clyde, Pacman pac, char board[36][28])
{
    generalGhostAI(blinky, pac, board);
    generalGhostAI(inky, pac, board);
    generalGhostAI(pinky, pac, board);
    generalGhostAI(clyde, pac, board);
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

void drawGhost(Enemy &ghost, int powerup_counter)
{
    std::string draw(1, ghost.draw_char);

    raylib::Color color;
    if(ghost.state == FRIGHTENED)
    {
        if(powerup_counter < 7 && powerup_counter % 2 == 0)
        {
            color = WHITE;
        }
        else
        {
            color = BLUE;
        }  
    }
    else if(ghost.state == EATEN)
    {
        color = GRAY;
    }
    else
    {
        switch(ghost.type)
        {
            case INKY:
                color = CYAN;
                break;

            case PINKY:
                color = PINK;
                break;

            case BLINKY:
                color = RED;
                break;

            case CLYDE:
                color = ORANGE;
                break;
        }
    }
    raylib::DrawText(draw.c_str(), 50 + ghost.x_pos * 26, 50 + ghost.y_pos * 26, 26, color);
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
    int scene = 0;
    std::string scorestr;

    Pacman pac;
    pac.x_pos = 14;
    pac.y_pos = 26;
    pac.draw_char = 'o';
    int frame_counter = 0;

    Enemy blinky;
    blinky.x_pos = 13;
    blinky.y_pos = 17;
    blinky.type = BLINKY;
    blinky.state = CHASE;
    blinky.draw_char = 'o';
    blinky.dead = true;
    blinky.dead_counter = 3 + (rand() % 3) - 1;

    Enemy inky;
    inky.x_pos = 14;
    inky.y_pos = 17;
    inky.type = INKY;
    inky.state = CHASE;
    inky.draw_char = 'o';
    inky.dead = true;
    inky.dead_counter = 9 + 2 * (rand() % 3) - 2;

    Enemy pinky;
    pinky.x_pos = 15;
    pinky.y_pos = 17;
    pinky.type = PINKY;
    pinky.state = CHASE;
    pinky.draw_char = 'o';
    pinky.dead = true;
    pinky.dead_counter = 15 + 3 * (rand() % 3) - 3;

    Enemy clyde;
    clyde.x_pos = 16;
    clyde.y_pos = 17;
    clyde.type = CLYDE;
    clyde.state = CHASE;
    clyde.draw_char = 'o';
    clyde.dead = true;
    clyde.dead_counter = 21 + 4 * (rand() % 3) - 4;


    bool skip = false;
    PacState last_state = REGULAR;
    int powerup_counter = 0;
    int seconds = 0;
    int seconds_counter = 7;

    if(!loadBoard(board))
    {
        return -1;
    }

    while(!window.ShouldClose())
    {
        window.ClearBackground();
        window.BeginDrawing();
        if(scene == 0)
        {
            DrawText("PacPlusPlus", 100, 350, 100, YELLOW);
            if(raylib::Keyboard::IsKeyPressed(KEY_ENTER))
            {
                scene++;
            }
        }
        else if(scene == 1)
        {
            frame_counter++;
            DrawBoard(board, pac, score);
            drawPac(pac, frame_counter);
            drawGhost(inky, powerup_counter);
            drawGhost(blinky, powerup_counter);
            drawGhost(pinky, powerup_counter);
            drawGhost(clyde, powerup_counter);
            if(frame_counter == 60)
            {
                frame_counter = 0;
                seconds++;
            }
            if(frame_counter % 10 == 0)
            {
                if(frame_counter % 20 == 0)
                {
                    getInput(pac);
                    if(pac.state == POWERUP)
                    {   
                        if(last_state == REGULAR)
                        {
                            powerup_counter = 16;
                            inky.state = FRIGHTENED;
                            pinky.state = FRIGHTENED;
                            blinky.state = FRIGHTENED;
                            clyde.state = FRIGHTENED;
                        }
                        if(!skip)
                        {
                            runGhostAI(blinky, inky, pinky, clyde, pac, board);
                            skip = true;
                        }
                        else
                        {
                            skip = false;
                        }
                        int pac_x = pac.x_pos;
                        int pac_y = pac.y_pos;

                        if(pac_x == blinky.x_pos && pac_y == blinky.y_pos && blinky.state != EATEN)
                        {
                            blinky.state = EATEN;
                            pac.ghosts_eaten++;
                            score += (pow(2, pac.ghosts_eaten) * 200);
                        }
                        if(pac_x == inky.x_pos && pac_y == inky.y_pos && inky.state != EATEN)
                        {
                            inky.state = EATEN;
                            pac.ghosts_eaten++;
                            score += (pow(2, pac.ghosts_eaten) * 200);
                        }
                        if(pac_x == pinky.x_pos && pac_y == pinky.y_pos && pinky.state != EATEN)
                        {
                            pinky.state = EATEN;
                            pac.ghosts_eaten++;
                            score += (pow(2, pac.ghosts_eaten) * 200);
                        }
                        if(pac_x == clyde.x_pos && pac_y == clyde.y_pos && clyde.state != EATEN)
                        {
                            clyde.state = EATEN;
                            pac.ghosts_eaten++;
                            score += (pow(2, pac.ghosts_eaten) * 200);
                        }
                        powerup_counter--;
                        if(powerup_counter == 0)
                        {
                            pac.state = REGULAR;
                            if(inky.state != EATEN)
                            {
                                inky.state = CHASE;
                            }
                            if(pinky.state != EATEN)
                            {
                                pinky.state = CHASE;
                            }
                            if(blinky.state != EATEN)
                            {
                                blinky.state = CHASE;
                            }
                            if(clyde.state != EATEN)
                            {
                                clyde.state = CHASE;
                            }
                            pac.ghosts_eaten = 0;
                        }
                    }
                    else
                    {
                        skip = false;
                        if(seconds_counter != -1)
                        {
                            if(seconds == seconds_counter)
                            {
                                if(seconds_counter == 7 && inky.state == SCATTER)
                                {
                                    seconds_counter = 5;
                                }
                                else if(seconds_counter == 5 && inky.state == SCATTER)
                                {
                                    seconds_counter = -1;
                                }
                                seconds = 0;
                                if(inky.state == CHASE)
                                {
                                    inky.state = SCATTER;
                                    pinky.state = SCATTER;
                                    blinky.state = SCATTER;
                                    clyde.state = SCATTER;
                                }
                                else
                                {
                                    inky.state = CHASE;
                                    pinky.state = CHASE;
                                    blinky.state = CHASE;
                                    clyde.state = CHASE;
                                }
                            }
                        }
                        runGhostAI(blinky, inky, pinky, clyde, pac, board);
                    }
                    last_state = pac.state;
                }
                else
                {
                    if(blinky.state == EATEN)
                    {
                        generalGhostAI(blinky, pac, board);
                    }
                    if(pinky.state == EATEN)
                    {
                        generalGhostAI(pinky, pac, board);
                    }
                    if(inky.state == EATEN)
                    {
                        generalGhostAI(inky, pac, board);
                    }
                    if(clyde.state == EATEN)
                    {
                        generalGhostAI(clyde, pac, board);
                    }
                }
            }
            scorestr = "Score: " + std::to_string(score);
            DrawText(scorestr.c_str(), 205, 20, 15, WHITE);
        }
        else if(scene == 2)
        {

        }
        window.EndDrawing();
    }
    return 0;
}