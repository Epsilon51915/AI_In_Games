//TODO: Fix CMAKELISTS.TXT file
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
#include <queue>
#include <unordered_set>
#include <stack>




// Stores indicies by which a given tile can move, i.e: tile 1 (the '0'th indexed tile, can move to tiles indexed at 3 (tile 4,) and at 1 (tile 4.))
int moves[9][4] = 
{
    /*     0    */  /*     1    */ /*     2    */
    {-1, 3, -1, 1}, {-1, 4, 0, 2}, {-1, 5, 1, -1},
    /*     3    */ /*     4    *//*     5    */
    {0, 6, -1, 4}, {1, 7, 3, 5}, {2, 8, 4, -1},
    /*     6    */  /*     7    */ /*     8    */
    {3, -1, -1, 7}, {4, -1, 6, 8}, {5, -1, 7, -1}
};

enum Move
{
    UP = 0,
    DOWN,
    LEFT,
    RIGHT,
    NONE
};

class Node
{
    public:
        std::string board ="";
        int selected = -1;
        Move prevMove = NONE;
        std::vector<Node*> next = {};
        Node* prevNode = nullptr;

    
    std::string swapBoard(Move move)
    {
        std::string newBoard = board;
        int moveIndex;
        if(move == UP)
        {
            moveIndex = selected - 3;
        }
        else if(move == DOWN)
        {
            moveIndex = selected + 3;
        }
        else if(move == LEFT)
        {
            moveIndex = selected - 1;
        }
        else if(move == RIGHT)
        {
            moveIndex = selected + 1;
        }
        std::swap(newBoard[selected], newBoard[moveIndex]);
        return newBoard;
    }

    // TODO: Finish
    void findNexts()
    {
        std::cout << "Current board is: " << board << std::endl << "With selected indexed at: " << selected << std::endl << std::endl;
        for(int i = 0; i < 4; i++)
        {
            if(moves[selected][i] != -1)
            {
                
                Node *n = new Node;
                //std::cout << "move found" << std::endl;
                n->prevMove = static_cast<Move>(i);
                //std::cout << "move found2" << std::endl;
                n->board = swapBoard(static_cast<Move>(i));
                n->selected = n->board.find('0');
                n->prevNode = this;
                next.emplace_back(n);
                std::cout << "Next board found at move: " << i << std::endl << "With board: " << n->board << std::endl << "With select indexed at: " << n->selected << std::endl << std::endl;
            }
        }
    }

    ~Node()
    {
        for(int i = 0; i < next.size(); i++)
        {
            delete next.at(i);
        }
    }
};
std::stack<Node*> stack;
void DrawBoard(int board[], bool solved)
{
    for(int i = 0; i < 3; i ++)
    {
        for(int j = 0; j < 3; j++)
        {
            if(board[3*i +j] == 0)
            {
                continue;
            }
            if(solved)
            {
            DrawText(std::to_string(board[3*i + j]).c_str(), 300 + 50 * j, 150 + 50 * i, 50, GREEN);

            }
            else
            {
                DrawText(std::to_string(board[3*i + j]).c_str(), 300 + 50 * j, 150 + 50 * i, 50, RED);
            }
            
        }
    }
}

// Selected is the index of the empty tile on the board
// Swap is the index of the tile that is to be swapped with the selected blank tile
// Return index of new blank tile
int Swap(int board[], int selected, int swap)
{
    int blank = 0;
    int swap_number = board[swap];

    board[swap] = blank;
    board[selected] = swap_number;

    return swap;
}

int MoveTile(int board[], Move move, int selected)
{
    if(moves[selected][move] == -1)
    {
        // Invalid move selected, ignore move.
        //std::cout << "Swap failed, tried to swap " << move << " at index " << selected << std::endl;
        return selected;
    }
    //std::cout << "Swap succeeded, tried to swap " << move << " at index " << selected << std::endl;
    return Swap(board, selected, moves[selected][move]);
}

void BuildPastMovesArr(Move pastMoves[], Move move)
{
    // Step 1: Shuffle all moves down by 1

    for(int i = 0; i< 3; i++)
    {
        pastMoves[i+1] = pastMoves[i];
    }

    // Step 2: Place last move at beginning of arr

    pastMoves[0] = move;
}

int AnalysePastMoves(Move pastMoves[])
{
    int chanceUp = 25;
    int chanceDown = 25;
    int chanceLeft = 25;
    int chanceRight = 25;
    int total = 100;

    for(int i = 0; i < 4; i++)
    {
        if(pastMoves[i] == UP)
        {
            chanceDown -= 5;
            total -= 5;
        }
        else if(pastMoves[i] == DOWN)
        {
            chanceUp -= 5;
            total -= 5;
        }
        else if(pastMoves[i] == LEFT)
        {
            chanceRight -= 5;
            total -= 5;
        }
        else if(pastMoves[i] == RIGHT)
        {
            chanceLeft -= 5;
            total -= 5;
        }
    }

    int test = rand() % total;

    if(test < chanceUp)
    {
        return 0;
    }
    else if(test < chanceUp + chanceDown)
    {
        return 1;
    }
    else if(test < chanceUp + chanceDown + chanceLeft)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

int RandomizeBoard(int board[], int selected)
{
    Move pastMoves[4] = {NONE, NONE, NONE, NONE};
    int index = 0;
    for(int i = 0; i < 10; i++)
    {
        int move = AnalysePastMoves(pastMoves);
        int test = MoveTile(board, static_cast<Move>(move), selected);
        //ensure 5 unique moves are made
        if(test == selected)
        {
            i--;
        }
        else
        {
            BuildPastMovesArr(pastMoves, static_cast<Move>(move));
        }
        selected = test;
    }
    return selected;
}

int resetBoard(int board[])
{
    for(int i = 0; i < 8; i++)
    {
        board[i] = i+1;
    }
    board[8] = 0;
    return RandomizeBoard(board, 8);
    
}


void Solve(int board[], int selected)
{
    std::cout << std::endl << std::endl << "BEGIN SOLVE" << std::endl << std::endl << std::endl;
    Node * curNode = new Node;

    curNode->selected = selected;

    std::string goal = "123456780";
    for(int i = 0; i < 9; i++)
    {
        //std::cout << i << std::endl;
        curNode->board.append(std::to_string(board[i]));
    }
    std::string curBoard;
    for(int i = 0; i < 9; i++)
    {
        curBoard.append(std::to_string(board[i]));
    }
    //std::cout << "curnode board made" << std::endl;
    std::queue<Node*> nodesToCheck;
    std::unordered_set<std::string> visited;
    

    while(curNode->board.compare(goal) != 0)
    {
        
        std::cout << std::endl << std::endl << "NEW NODE" << std::endl << std::endl << std::endl;
        curNode->findNexts();
        //std::cout << "FOUND NEXTS" << std::endl;
        for(int i = 0; i < curNode->next.size(); i++)
        {
            if(visited.contains(curNode->next.at(i)->board )) continue;
            nodesToCheck.push(curNode->next.at(i));
            visited.insert(curNode->next.at(i)->board);

            std::cout << "INSERTED " << curNode->next.at(i)->board << std::endl;
        }
        //std::cout << "PUSH DONE" << std::endl;
        Node* nextNode = nodesToCheck.front();
        //std::cout << "ASSIGNED next node" << std::endl;
        curNode = nextNode;
        
        nodesToCheck.pop();
    }
    
    if(curNode->board == goal)
    {
        std::cout << "Solution found!" << std::endl;
        while(curNode->board != curBoard)
        {
            stack.push(curNode);
            curNode = curNode->prevNode;
        }

        for(int i = stack.size() -1; i >= 0; i--)
        {
            Node* node = stack.top();
            stack.pop();
            std::cout << "Start at board " << curBoard << std::endl;

            std::string moveName;
            switch(node->prevMove)
            {
                case 0:
                    moveName = "UP";
                    break;

                case 1:
                    moveName = "DOWN";
                    break;

                case 2:
                    moveName = "LEFT";
                    break;

                case 3:
                    moveName = "RIGHT";
                    break;
            }
            std::cout << "Move " << moveName << " to get to board " << node->board << std::endl;
        }
        return;
    }
    
}


int main()
{
    srand(time(nullptr));
    raylib::Window window(800, 700, "8Puzzle");
    window.SetTargetFPS(60);
    window.SetState(FLAG_WINDOW_RESIZABLE);

    int board[9] = 
    {
        1, 2, 3,
        4, 5, 6,
        7, 8, 0
    };
    std::string solvedS ="123456780";

    int selected = 8;
    selected = RandomizeBoard(board, selected);
    // Node testNode;
    // for(int i = 0; i < 9; i++)
    // {
    //     testNode.board.append(std::to_string(board[i]));
    // }
    // testNode.selected = selected;
    // testNode.findNexts();
    while(!window.ShouldClose())
    {
        window.ClearBackground();
        window.BeginDrawing();

        DrawText("8-Puzzle", 325, 50, 40, BLUE);
        DrawText("Press 'h' for help", 325, 400, 30, BLUE);
        DrawText("Press 'r' for a new board", 325, 450, 30, BLUE);
        std::string boardS;
        
        for(int i = 0; i < 9; i++)
        {
            boardS.append(std::to_string(board[i]));
        }
        if(boardS == solvedS) DrawBoard(board, true);
        else DrawBoard(board, false);
        window.EndDrawing();

        if(raylib::Keyboard::IsKeyPressed(KEY_W) || raylib::Keyboard::IsKeyPressed(KEY_UP))
        {
            selected = MoveTile(board, UP, selected);
            
        }
        if(raylib::Keyboard::IsKeyPressed(KEY_A) || raylib::Keyboard::IsKeyPressed(KEY_LEFT))
        {
            selected = MoveTile(board, LEFT, selected);
        }
        if(raylib::Keyboard::IsKeyPressed(KEY_S) || raylib::Keyboard::IsKeyPressed(KEY_DOWN))
        {
            selected = MoveTile(board, DOWN, selected);
        }
        if(raylib::Keyboard::IsKeyPressed(KEY_D) || raylib::Keyboard::IsKeyPressed(KEY_RIGHT))
        {
            selected = MoveTile(board, RIGHT, selected);
        }
        if(raylib::Keyboard::IsKeyPressed(KEY_R))
        {
            selected = resetBoard(board);
        }
        if(raylib::Keyboard::IsKeyPressed(KEY_H))
        {
            Solve(board, selected);
        }
    }

    return 0;
}