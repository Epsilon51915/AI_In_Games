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
#include <queue>

#define CYAN ColorFromHSV(180, 1, 1);
int TIME_BETWEEN_MOVES = 6;

int fire = 0;

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
    int lives = 3;
    int pellets_eaten;
    int total_ghosts_eaten;
    int power_pellets_eaten;
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
    //Tile *adj_tile[4];
    std::unique_ptr<Tile*[]> adj_tile = std::make_unique<Tile*[]>(4);

    // Number of moves to reach this tile
    int num_moves;

    ~Tile(){}
};
/*
    ToDo:
        - Draw Pacman ✓
        - Design ghost AI   CANNOT CHANGE 180 DEGREES UNLESS CHANGING FROM CHASE TO SCATTER, SLOWER THAN PLAYER WHEN FLEEING ✓
        - Design pacman AI  CAN CHANGE 180 DEGREES WHENEVER DESIRED ✓
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

void findAdj(Tile* cur_tile, char board[36][28], bool ghost_finding)//, std::unordered_set<std::string> &visited)
{
    char board_tile;
    int x, y;
    for(int i = 0; i < 4; i++)
    {
        if(i % 2 == 0)
        {
            x = cur_tile->x_pos + (i-1);
            y= cur_tile->y_pos;
            board_tile = board[y][x];
        }
        else
        {
            y= cur_tile->y_pos + (i-2);
            x = cur_tile->x_pos;
            board_tile = board[y][x];
        }

        if(board_tile == 'X' || board_tile == '.' || board_tile == 'o' || board_tile == ' ' || (ghost_finding && board_tile == '-'))
        {
            Tile *new_tile = new Tile;
            new_tile->x_pos = x;
            new_tile->y_pos = y;
            new_tile->value = 0;
            cur_tile->adj_tile[i] = new_tile;
        }
        else
        {
            cur_tile->adj_tile[i] = nullptr;
        }
    }
}

int findNearestPellet(Tile* cur_tile, char board[36][28])
{
    // Perform BFS starting at cur_tile, ending when the first pellet is found.
    std::queue<Tile*> queue;
    std::unordered_set<std::string> visited;

    std::string temp;

    Tile* cur_search_tile = cur_tile;
    cur_search_tile->value = 0;
    queue.push(cur_search_tile);

    while(board[cur_search_tile->y_pos][cur_search_tile->x_pos] != '.')
    {
        findAdj(cur_search_tile, board, false);
        for(int i = 0; i < 4; i++)
        {
            if(cur_search_tile->adj_tile[i] != nullptr)
            {
                cur_search_tile->adj_tile[i]->value = cur_search_tile->value + 1;
                temp = std::to_string(cur_search_tile->adj_tile[i]->x_pos) + "," + std::to_string(cur_search_tile->adj_tile[i]->y_pos);
                if(!visited.contains(temp))
                {
                    visited.insert(temp);
                    queue.push(cur_search_tile->adj_tile[i]);
                }
            }
        }
        cur_search_tile = queue.front();
        queue.pop();
    }

    return cur_search_tile->value + 1;
}

int findNearestGhost(Tile* cur_tile, char board[36][28], const Enemy &g1, const Enemy &g2, const Enemy &g3, const Enemy &g4, int counter, bool test)
{
    // Perform BFS starting at cur_tile, ending when the first pellet is found.
    std::queue<Tile*> queue;
    std::unordered_set<std::string> visited;

    std::string temp;

    Tile* cur_search_tile = cur_tile;
    queue.push(cur_search_tile);

    int x = cur_search_tile->x_pos;
    int y = cur_search_tile->y_pos;
    temp = std::to_string(cur_search_tile->x_pos) + "," + std::to_string(cur_search_tile->y_pos);
    visited.insert(temp);
    std::cout << "TEST1" << std::endl;
    if(test)
    {
        if(cur_tile->prev_tile != nullptr)
        {
            std::cout << "cur_tile->prev_tile is not nullptr" << std::endl;
            temp = std::to_string(cur_tile->prev_tile->x_pos) + "," + std::to_string(cur_tile->prev_tile->y_pos);
            visited.insert(temp);
        }
    }
    
    //std::cout << "TEST" << std::endl;
    //int counter = 0;
    while(!(x == g1.x_pos && y == g1.y_pos && g1.state != EATEN) && !(x == g2.x_pos && y == g2.y_pos && g2.state != EATEN) && !(x == g3.x_pos && y == g3.y_pos && g3.state != EATEN) && !(x == g4.x_pos && y == g4.y_pos && g4.state != EATEN) && counter > 0)
    {
        findAdj(cur_search_tile, board, true);
        for(int i = 0; i < 4; i++)
        {
            if(cur_search_tile->adj_tile[i] != nullptr)
            {
                cur_search_tile->adj_tile[i]->value = cur_search_tile->value + 1;
                temp = std::to_string(cur_search_tile->adj_tile[i]->x_pos) + "," + std::to_string(cur_search_tile->adj_tile[i]->y_pos);
                if(!visited.contains(temp))
                {
                    visited.insert(temp);
                    queue.push(cur_search_tile->adj_tile[i]);
                }
            }
        }
        cur_search_tile = queue.front();
        queue.pop();
        x = cur_search_tile->x_pos;
        y = cur_search_tile->y_pos;
        counter--;
    }
    //outputS.close();
    //std::cout << "Found" << std::endl;

    return counter;
}

int findPowerPellet(Tile* cur_tile, char board[36][28])
{
    // Perform BFS starting at cur_tile, ending when the first pellet is found.
    std::queue<Tile*> queue;
    std::unordered_set<std::string> visited;

    std::string temp;
    int counter = 0;
    Tile* cur_search_tile = cur_tile;
    queue.push(cur_search_tile);

    while(board[cur_search_tile->y_pos][cur_search_tile->x_pos] != 'o' && counter < 6)
    {
        findAdj(cur_search_tile, board, false);
        for(int i = 0; i < 4; i++)
        {
            if(cur_search_tile->adj_tile[i] != nullptr)
            {
                cur_search_tile->adj_tile[i]->value = cur_search_tile->value + 1;
                temp = std::to_string(cur_search_tile->adj_tile[i]->x_pos) + "," + std::to_string(cur_search_tile->adj_tile[i]->y_pos);
                if(!visited.contains(temp))
                {
                    visited.insert(temp);
                    queue.push(cur_search_tile->adj_tile[i]);
                }
            }
        }
        cur_search_tile = queue.front();
        queue.pop();
        counter++;
    }

    return counter + 1;
}

bool trapped(Pacman pac, Pacman test, char board[36][28], const Enemy &g1, const Enemy &g2, const Enemy &g3, const Enemy &g4)
{
    /*
        Request this fcn when testing a possible move. This function will search away from pacman through tunnels:
            If a tunnel has a ghost in it, or a ghost would be able to get into the tunnel before pacman could leave, return TRUE
            If pacman can make it through the tunnel without being cornered by a ghost, return FALSE
    */
    int x;
    int y;
    int prev_x = pac.x_pos;
    int prev_y = pac.y_pos;
    int counter = 0;
    // LEFT, UP ,RIGHT, DOWN
    bool in_tunnel = true;
    char tile;
    int possible_moves;
    int hold_x;
    int hold_y;
    while(in_tunnel)
    {
        // Search for possible moves. If 2 or more non-backtracking moves are possible, pacman is out of the tunnel.
        possible_moves = 0;
        for(int i = 0; i < 4; i++)
        {
            //std::cout << i << std::endl;
            x = test.x_pos;
            y = test.y_pos;

            if (i % 2 == 0)
            {
                x += i - 1;
            }
            else
            {
                y += i - 2;
            }

            if(x == prev_x && y == prev_y)
            {
                continue;
            }
            else if ((x == g1.x_pos && y == g1.y_pos))
            {
                //std::cout << "Trapped by: " << g1.x_pos << "," << g1.y_pos << " at: " << x << "," << y << std::endl;
                return true;
            }
            else if ((x == g2.x_pos && y == g2.y_pos))
            {
                //std::cout << "Trapped by: " << g2.x_pos << "," << g2.y_pos << " at: " << x << "," << y << std::endl;
                return true;
            }
            else if ((x == g3.x_pos && y == g3.y_pos))
            {
                //std::cout << "Trapped by: " << g3.x_pos << "," << g3.y_pos << " at: " << x << "," << y << std::endl;
                return true;
            }
            else if ((x == g4.x_pos && y == g4.y_pos))
            {
                //std::cout << "Trapped by: " << g4.x_pos << "," << g4.y_pos << " at: " << x << "," << y << std::endl;
                return true;
            } 
            else
            {
                tile = board[y][x];
                if(tile == ' ' || tile == '.' || tile == 'o' || tile == 'X')
                {
                    //std::cout << tile << possible_moves << std::endl;
                    possible_moves++;
                    hold_x = x;
                    hold_y = y;
                }
            }
        }
        if(possible_moves > 1)
        {
            //std::cout << "Not in tunnel" << std::endl;
            in_tunnel = false;
        }
        else
        {
            counter++;
            prev_x = test.x_pos;
            prev_y = test.y_pos;
            test.x_pos = hold_x;
            test.y_pos = hold_y;
            //std::cout << "Counter: " << counter << std::endl;
        }
    }

    // Check adjacencies. If pacman runs into ANY ghost before counter is 0, ghost will be able to enter the tunnel before pacman can leave
    if(counter != 0)
    {
        Tile* cur_tile = new Tile;
        Tile* prev = new Tile;
        cur_tile->x_pos = test.x_pos;
        cur_tile->y_pos = test.y_pos;
        prev->x_pos = prev_x;
        prev->y_pos = prev_y;
        cur_tile->prev_tile = prev;
        //std::cout << "Out ofloop" << std::endl;
        if(findNearestGhost(cur_tile, board, g1, g2, g3, g4, counter, true) != 0)
        {
            //std::cout << test.x_pos << "," << test.y_pos << " is TRAPPED!!" << std::endl;
            return true;
        }
    }
    
    //std::cout << "rtn" << std::endl;
    return false;
}

// LEFT = 0, UP = 1, RIGHT = 2, DOWN = 3 (CW at 9)
int nextMove(Pacman &pac, char board[36][28], Enemy &blinky, Enemy &pinky, Enemy &inky, Enemy &clyde, Pacman prev)
{
    /*
        UTILITY FUNCTION:
            Choose tile with HIGHEST VALUE, calculated as:
                Score = 10/distance from nearest pellet + 50 * distance from nearest ghost + 50 if power pellet is within 4 moves
                + (pow(2, number of ghosts eaten + 1) * 200)/distance from closest ghost if pacman has powerup 
    */

    // 1. Find all adjacent tiles
    Tile *cur_tile = new Tile;
    cur_tile->x_pos = pac.x_pos;
    cur_tile->y_pos = pac.y_pos;
    cur_tile->value = 0;
    cur_tile->prev_tile = nullptr;
    findAdj(cur_tile, board, false);
    
    int pellet;
    float ghost;
    int power_pellet;
    float value = 0;
    std::string s;
    int values[4] = {-10000, -10000, -10000, -10000};
    int highest = -10000;
    int hI = -1;
    for(int i = 0; i < 4; i++)
    {
        if(cur_tile->adj_tile[i] != nullptr)
        {
            // 2. Find nearest pellet to all tiles
            pellet = findNearestPellet(cur_tile->adj_tile[i], board);

            // 3. Find nearest ghost to all tiles
            ghost = findNearestGhost(cur_tile->adj_tile[i], board, blinky, pinky, inky, clyde, 7, false);

            if(pac.state == POWERUP)
            {
                ghost = (pow(2, pac.ghosts_eaten + 1) * 200) - 30 * (7 - ghost);
            }
            else
            {
                ghost = 30 * (7-ghost);
            }

            // 4. Find if a power pellet is within 4 tiles
            if(pac.power_pellets_eaten == 4)
            {
                power_pellet = 50000;
            }
            else
            {
                power_pellet = findPowerPellet(cur_tile->adj_tile[i], board);
            }

            value = 30.0/(static_cast<float>(pellet) - static_cast<float>(pellet) * (pac.pellets_eaten) / 242.0) + ghost + 50.0/(float)power_pellet;

            Pacman test;
            test.x_pos = cur_tile->adj_tile[i]->x_pos;
            test.y_pos = cur_tile->adj_tile[i]->y_pos;

            if(pac.state != POWERUP)
            {
                if(trapped(pac, test, board, blinky, pinky, inky, clyde))
                {
                    value = 0;
                }
            }
            
            values[i] = value;
        }
    }
    std::cout << std::endl;

    // 5. Compare values for adjacent tiles, return highest value
    
    int x, y;
    for(int i = 0; i < 4; i++)
    {
        if(cur_tile->adj_tile[i] != nullptr)
        {
            if(i % 2 == 0)
            {
                x = cur_tile->x_pos + (i - 1);
                y = cur_tile->y_pos;
            }
            else
            {
                x = cur_tile->x_pos;
                y = cur_tile->y_pos + (i - 2);
            }
            // Doubling back
            std::cout << x << " vs " << prev.x_pos << " and " << y << " vs " << prev.y_pos << std::endl;
            if(x == prev.x_pos && y == prev.y_pos)
            {
                std::cout << "Doubled back." << std::endl;
                values[i] = values[i] - 40.0;
            }

            if(pac.state != POWERUP && ((x == blinky.x_pos && y == blinky.y_pos) || (x == pinky.x_pos && y == pinky.y_pos) || (x == inky.x_pos && y == inky.y_pos) || (x == clyde.x_pos && y == clyde.y_pos)))
            {
                values[i] = -1000.0;
            }

            for(int j = 0; j < 4; j++)
            {
                x = cur_tile->adj_tile[i]->x_pos;
                y = cur_tile->adj_tile[i]->y_pos;
                if(j % 2 == 0)
                {
                    x += j - 1;
                }
                else
                {
                    y += j - 2;
                }
                if(pac.state != POWERUP)
                {
                    if((x == blinky.x_pos && y == blinky.y_pos) || (x == pinky.x_pos && y == pinky.y_pos) || (x == inky.x_pos && y == inky.y_pos) || (x == clyde.x_pos && y == clyde.y_pos))
                    {
                        values[i] = -1000.0;
                        break;
                    }
                }
            }
        }
    }
    for(int i = 0; i < 4; i++)
    {
        if(values[i] > highest)
        {
            highest = values[i];
            hI = i;
        }
        switch(i)
            {
                case 0:
                    s = "left";
                    break;

                case 1:
                    s = "up";
                    break;

                case 2:
                    s = "right";
                    break;

                case 3:
                    s = "down";
                    break;
            }
        std::cout << "Final assigned value for " << s << ": " << values[i] <<  std::endl;
    }

    delete cur_tile;
    cur_tile = nullptr;

    switch(hI)
    {
        case 0:
            s = "left";
            break;

        case 1:
            s = "up";
            break;

        case 2:
            s = "right";
            break;

        case 3:
            s = "down";
            break;
    }
    std::cout << "Go " << s << std::endl;
    std::cout << "-----------------------------" << std::endl;
    return hI;
}

bool update(Pacman &pac, char board[36][28], Enemy blinky, Enemy pinky, Enemy inky, Enemy clyde, Pacman &prev)
{
    // Check if pacman has eaten 242 pellets
    if(pac.pellets_eaten == 242)
    {
        return true;
    }

    // Run algorithm to find highest utility adjacent move
    int move = nextMove(pac, board, blinky, pinky, inky, clyde, prev);

    // Based on return value, move to next positions
    prev.x_pos = pac.x_pos;
    prev.y_pos = pac.y_pos;
    if(move % 2 == 0)
    {
        pac.x_pos += move - 1;
    }
    else
    {
        pac.y_pos += move - 2;
    }
    return false;
}

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

    //std::cout << "START" << start_x << "," << start_y << std::endl;
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
        //std::cout << "PREV TILE FROM " << cur_search_tile->x_pos << "," << cur_search_tile->y_pos << ": " << 
        //cur_search_tile->prev_tile->x_pos << "," << cur_search_tile->prev_tile->y_pos << std::endl; 

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
        //std::cout << cur_search_tile->x_pos << " " << cur_search_tile->y_pos << std::endl;
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
        //std::cout << "Start find move" << std::endl;
        findMove(ghost, pac, cur_tile, board);
        //std::cout << ghost.x_pos << " " << ghost.y_pos << std::endl;
        //std::cout << "End find move" << std::endl;
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
            ghost.dead_counter = 6 + 4 * ghost.type;
            ghost.state = CHASE;
        }
    }
    delete cur_tile;
    cur_tile = nullptr;
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
void DrawBoard(char board[36][28], Pacman &pac, int &score, bool won_level)
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
                    pac.pellets_eaten++;
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
                    pac.power_pellets_eaten++;
                }
                DrawText(charstring.c_str(), 50 + col * 26, 50 + row * 26, 26, YELLOW);
            }
            else if(board[row][col] == 'X'){}
            else
            {
                if(won_level)
                {
                    DrawText(charstring.c_str(), 50 + col * 26, 50 + row * 26, 26, WHITE);
                }   
                else
                {
                    DrawText(charstring.c_str(), 50 + col * 26, 50 + row * 26, 26, DARKBLUE);
                }
                
            }
        }
    }
}

bool loadBoard(char board[36][28], int level) 
{
    std::ifstream file;
    if(level == 1)
    {
        file.open("../build/tilemaps/LevelOneTilemap.txt");
    }
    else
    {
        file.open("../build/tilemaps/LevelTwoTilemap.txt");
    }
    
    if(!file.is_open())
    {
        std::cerr << "File unable to open" << std::endl;
        return false;
    }

    int col = 0;
    int row = 0;
    char waste;
    char ch;
    for(int i = 0; i < 36; i++)
    {
        for(int j = 0; j < 28; j++)
        {
            board[i][j] = ' ';
        }
    }
    while(file.get(ch))
    {
        board[row][col] = ch;
        col++;
        if((col % 28) == 0)
        {
            file.get(waste); // '\'
            file.get(waste); // 'n'
            col = 0;
            row++;
        }
        
    }
    file.close();
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

    for(int i = 0; i < pac.lives; i++)
    {
        DrawText("c", 50 + (i+1) * 26, 26, 26, YELLOW);
    }
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

void drawPacDie(Pacman pac, int fc)
{
    if(fc < 45)
    {
        pac.draw_char = 'o';
    }
    else if(fc < 90)
    {
        pac.draw_char = 'u';
    }
    else if (fc < 135)
    {
        pac.draw_char = '_';
    }
    else
    {
        pac.draw_char = ' ';
    }
    std::string draw(1, pac.draw_char);
    DrawText(draw.c_str(), 50 + pac.x_pos * 26, 50 + pac.y_pos * 26, 26, YELLOW);

    for(int i = 0; i < pac.lives; i++)
    {
        DrawText("c", 50 + (i+1) * 26, 26, 26, YELLOW);
    }
}

void resetAfterDeath(Enemy &blinky, Enemy &pinky, Enemy &inky, Enemy &clyde, Pacman &pac)
{
    blinky.x_pos = 13;
    blinky.y_pos = 17;
    blinky.state = CHASE;
    blinky.draw_char = 'o';
    blinky.dead = true;
    blinky.dead_counter = 3 + (rand() % 3) - 1;

    inky.x_pos = 14;
    inky.y_pos = 17;
    inky.state = CHASE;
    inky.draw_char = 'o';
    inky.dead = true;
    inky.dead_counter = 9 + 2 * (rand() % 3) - 2;

    pinky.x_pos = 15;
    pinky.y_pos = 17;
    pinky.state = CHASE;
    pinky.draw_char = 'o';
    pinky.dead = true;
    pinky.dead_counter = 15 + 3 * (rand() % 3) - 3;

    clyde.x_pos = 16;
    clyde.y_pos = 17;
    clyde.state = CHASE;
    clyde.draw_char = 'o';
    clyde.dead = true;
    clyde.dead_counter = 21 + 4 * (rand() % 3) - 4;

    pac.x_pos = 14;
    pac.y_pos = 26;
    pac.draw_char = 'o';
    pac.state = REGULAR;
}


int main()
{
    srand(time(nullptr));
    //x, y
    raylib::Window window(800, 1200, "PacPlusPlus");
    window.SetTargetFPS(60);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    char board[36][28];
    char newboard[36][28];
    int score = 0;
    int scene = 0;
    std::string scorestr;

    Pacman pac;
    pac.x_pos = 14;
    pac.y_pos = 26;
    pac.draw_char = 'o';
    int frame_counter = 0;
    pac.lives = 3;
    bool pac_died = false;
    pac.pellets_eaten = 0;
    pac.power_pellets_eaten = 0;
    pac.total_ghosts_eaten = 0;
    Pacman prev;
    prev.x_pos = 14;
    prev.y_pos = 14;

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

    if(!loadBoard(board, 1))
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
            if(raylib::Keyboard::IsKeyPressed(KEY_LEFT_SHIFT))
            {
                TIME_BETWEEN_MOVES = 200;
            }
            else if (raylib::Keyboard::IsKeyPressed(KEY_LEFT_CONTROL))
            {
                TIME_BETWEEN_MOVES = 2;
            }
            if(pac_died)
            {
                frame_counter++;
                drawPacDie(pac, frame_counter);
                DrawBoard(board, pac, score, false);
                
                if(frame_counter == 180)
                {
                    pac.lives--;
                    if(pac.lives == 0)
                    {
                        scene = -1;
                    }
                    else
                    {
                        resetAfterDeath(blinky, pinky, inky, clyde, pac);
                        seconds = 0;
                        seconds_counter = 7;
                        frame_counter = 0;
                        pac_died = false;
                    }
                }
            }
            else
            {
                frame_counter++;
                DrawBoard(board, pac, score, false);
                drawPac(pac, frame_counter);
                drawGhost(inky, powerup_counter);
                drawGhost(blinky, powerup_counter);
                drawGhost(pinky, powerup_counter);
                drawGhost(clyde, powerup_counter);
                if(frame_counter == TIME_BETWEEN_MOVES * 3)
                {
                    frame_counter = 0;
                    seconds++;
                }
                if(frame_counter % (TIME_BETWEEN_MOVES/2) == 0)
                {
                    if(frame_counter % TIME_BETWEEN_MOVES == 0)
                    {
                        if(update(pac, board, blinky, pinky, inky, clyde, prev))
                        {
                            scene = 2;
                            frame_counter = 0;
                        }
                        else
                        {
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
                                    pac.total_ghosts_eaten++;
                                    score += (pow(2, pac.ghosts_eaten) * 200);
                                }
                                if(pac_x == inky.x_pos && pac_y == inky.y_pos && inky.state != EATEN)
                                {
                                    inky.state = EATEN;
                                    pac.ghosts_eaten++;
                                    pac.total_ghosts_eaten++;
                                    score += (pow(2, pac.ghosts_eaten) * 200);
                                }
                                if(pac_x == pinky.x_pos && pac_y == pinky.y_pos && pinky.state != EATEN)
                                {
                                    pinky.state = EATEN;
                                    pac.ghosts_eaten++;
                                    pac.total_ghosts_eaten++;
                                    score += (pow(2, pac.ghosts_eaten) * 200);
                                }
                                if(pac_x == clyde.x_pos && pac_y == clyde.y_pos && clyde.state != EATEN)
                                {
                                    clyde.state = EATEN;
                                    pac.ghosts_eaten++;
                                    pac.total_ghosts_eaten++;
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
                                pac.state = REGULAR;
                                runGhostAI(blinky, inky, pinky, clyde, pac, board);
                                if((pac.x_pos == inky.x_pos && pac.y_pos == inky.y_pos) || (pac.x_pos == blinky.x_pos && pac.y_pos == blinky.y_pos) || (pac.x_pos == pinky.x_pos && pac.y_pos == pinky.y_pos) || (pac.x_pos == clyde.x_pos && pac.y_pos == clyde.y_pos))
                                {
                                    pac_died = true;
                                    frame_counter = 0;
                                }
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
                            }
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
        }
        else if(scene == 2)
        {
            DrawBoard(board, pac, score, true);
            drawPac(pac, frame_counter);
            frame_counter++;
            if(frame_counter == 180)
            {
                scene = 3;
            }
        }
        else if(scene == 3)
        {
            if(!loadBoard(newboard, 2))
            {
                return -1;
            }
            else
            {
                resetAfterDeath(blinky, pinky, inky, clyde, pac);
                pac.pellets_eaten = 0;
                scene = 4;
            }
        }
        else if(scene == 4)
        {
            if(raylib::Keyboard::IsKeyPressed(KEY_LEFT_SHIFT))
            {
                TIME_BETWEEN_MOVES = 200;
            }
            else if (raylib::Keyboard::IsKeyPressed(KEY_LEFT_CONTROL))
            {
                TIME_BETWEEN_MOVES = 2;
            }
            if(pac_died)
            {
                frame_counter++;
                drawPacDie(pac, frame_counter);
                DrawBoard(newboard, pac, score, false);
                
                if(frame_counter == 180)
                {
                    pac.lives--;
                    if(pac.lives == 0)
                    {
                        scene = -1;
                    }
                    else
                    {
                        resetAfterDeath(blinky, pinky, inky, clyde, pac);
                        seconds = 0;
                        seconds_counter = 7;
                        frame_counter = 0;
                        pac_died = false;
                    }
                }
            }
            else
            {
                frame_counter++;
                DrawBoard(newboard, pac, score, false);
                drawPac(pac, frame_counter);
                drawGhost(inky, powerup_counter);
                drawGhost(blinky, powerup_counter);
                drawGhost(pinky, powerup_counter);
                drawGhost(clyde, powerup_counter);
                if(frame_counter == TIME_BETWEEN_MOVES * 3)
                {
                    frame_counter = 0;
                    seconds++;
                }
                if(frame_counter % (TIME_BETWEEN_MOVES/2) == 0)
                {
                    if(frame_counter % TIME_BETWEEN_MOVES == 0)
                    {
                        if(update(pac, newboard, blinky, pinky, inky, clyde, prev))
                        {
                            scene = 2;
                            frame_counter = 0;
                        }
                        else
                        {
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
                                    runGhostAI(blinky, inky, pinky, clyde, pac, newboard);
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
                                    pac.total_ghosts_eaten++;
                                    score += (pow(2, pac.ghosts_eaten) * 200);
                                }
                                if(pac_x == inky.x_pos && pac_y == inky.y_pos && inky.state != EATEN)
                                {
                                    inky.state = EATEN;
                                    pac.ghosts_eaten++;
                                    pac.total_ghosts_eaten++;
                                    score += (pow(2, pac.ghosts_eaten) * 200);
                                }
                                if(pac_x == pinky.x_pos && pac_y == pinky.y_pos && pinky.state != EATEN)
                                {
                                    pinky.state = EATEN;
                                    pac.ghosts_eaten++;
                                    pac.total_ghosts_eaten++;
                                    score += (pow(2, pac.ghosts_eaten) * 200);
                                }
                                if(pac_x == clyde.x_pos && pac_y == clyde.y_pos && clyde.state != EATEN)
                                {
                                    clyde.state = EATEN;
                                    pac.ghosts_eaten++;
                                    pac.total_ghosts_eaten++;
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
                                pac.state = REGULAR;
                                runGhostAI(blinky, inky, pinky, clyde, pac, newboard);
                                if((pac.x_pos == inky.x_pos && pac.y_pos == inky.y_pos) || (pac.x_pos == blinky.x_pos && pac.y_pos == blinky.y_pos) || (pac.x_pos == pinky.x_pos && pac.y_pos == pinky.y_pos) || (pac.x_pos == clyde.x_pos && pac.y_pos == clyde.y_pos))
                                {
                                    pac_died = true;
                                    frame_counter = 0;
                                }
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
                            }
                        }
                        
                        last_state = pac.state;
                    }
                    else
                    {
                        if(blinky.state == EATEN)
                        {
                            generalGhostAI(blinky, pac, newboard);
                        }
                        if(pinky.state == EATEN)
                        {
                            generalGhostAI(pinky, pac, newboard);
                        }
                        if(inky.state == EATEN)
                        {
                            generalGhostAI(inky, pac, newboard);
                        }
                        if(clyde.state == EATEN)
                        {
                            generalGhostAI(clyde, pac, newboard);
                        }
                    }
                }
                scorestr = "Score: " + std::to_string(score);
                DrawText(scorestr.c_str(), 205, 20, 15, WHITE);
            }
        }   
        else if(scene == -1)
        {
            DrawText("Game Over", 100, 400, 50, RED);
            scorestr = "Final Score: " + std::to_string(score);
            DrawText(scorestr.c_str(), 100, 450, 30, WHITE);
            scorestr = "Pellets Eaten: " + std::to_string(pac.pellets_eaten);
            DrawText(scorestr.c_str(), 100, 480, 30, WHITE);
            scorestr = "Power Pellets Eaten: " + std::to_string(pac.power_pellets_eaten);
            DrawText(scorestr.c_str(), 100, 510, 30, WHITE);
            scorestr = "Ghosts Eaten: " + std::to_string(pac.total_ghosts_eaten);
            DrawText(scorestr.c_str(), 100, 540, 30, WHITE);

            DrawText("Press ENTER to restart", 100, 640, 50, RED);
            DrawText("Press ESC to quit", 100, 690, 50, RED);

            if(raylib::Keyboard::IsKeyPressed(KEY_ESCAPE))
            {
                window.Close();
            }
            else if(raylib::Keyboard::IsKeyPressed(KEY_ENTER))
            {
                scene = 0;
            }
        }
        window.EndDrawing();
    }
    return 0;
}
// fin :D