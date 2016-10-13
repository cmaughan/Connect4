#include "Api.h"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include "Game.h"
#include <algorithm>
#include <set>

#ifdef  WIN32
#include "DrawWindow.h"
#endif

// Put your team and password here
const std::string password("Foobar2");
const std::string teamName("Morninator");

// Bigger search depth takes longer
const int MaxDepth = 7;

// ASCII Art version of the board
// On Windows you get the fancy one....
void PrintBoard(Game& game)
{
    SetGame(game);
    std::ostringstream str;
    str << "---------" << std::endl;
    for (int row = Game::NUMBER_OF_ROWS - 1; row >= 0; row--)
    {
        for (int column = 0; column < Game::NUMBER_OF_COLUMNS; column++)
        {
            if (column == 0)
            {
                str << "|";
            }

            switch (game.Cells[column][row])
            {
            case CellContent::Empty:
                str << "O";
                break;
            case CellContent::Red:
                str << "R";
                break;
            case CellContent::Yellow:
                str << "Y";
                break;
            }

            if (column == Game::NUMBER_OF_COLUMNS - 1)
            {
                str << "|";
            }
        }
        str << std::endl;
    }
    str << "---------" << std::endl;
    bool finished;
    auto statusText = game.GetStatusString(finished);
    str << statusText << std::endl << std::endl;
    std::cout << str.str();
}

// Build a move tree
bool GenerateMove(Api::Player* player, Game& game, Node& node, int depth)
{
    auto moves = game.GetValidMoves();

    if (moves.empty())
        return false;

    // Score + or - depending on player or opponent's turn
    auto scoreMult = (depth & 1) ? -1 : 1;
    auto playColor = game.GetCurrentPlayer();
    for (unsigned int index = 0; index < moves.size(); index++)
    {
        auto& move = moves[index];

        // Modify the game
        auto previousValue = game.Cells[move.column][move.row];
        game.Cells[move.column][move.row] = playColor;

        // ... evaluate it.  More weight to less deep moves
        move.score = int((game.EvaluatePosition() * scoreMult) / float(depth + 1));
        Node childNode;
        childNode.location = move;
        node.children.push_back(childNode);

        // Next level down
        if (depth < MaxDepth)
        {
            // Other player
            game.SwapPlayer();
            GenerateMove(player, game, node.children[node.children.size() -1], depth + 1);
            game.SwapPlayer();
        }

        // Add the child score to our own
        node.location.score += childNode.location.score;

        // Restore board state
        game.Cells[move.column][move.row] = previousValue;
    }

    // Decided on a move, so play it if this is the root
    if (depth == 0)
    {
        for (auto& node : node.children)
        {
            std::cout << "Val: " << node.location.score << std::endl;
        }

        // Sort low->high score
        std::sort(node.children.begin(), node.children.end());

        // Pick the best moves
        std::vector<Location> pickMoves;
        int count = -1;
        auto itrSet = node.children.rbegin();
        auto itrSetEnd = node.children.rend();
        while (itrSet != itrSetEnd)
        {
            if (count == -1)
            {
                count = itrSet->location.score;
                pickMoves.push_back(itrSet->location);
            }
            else
            {
                if (itrSet->location.score < count)
                {
                    break;
                }
                pickMoves.push_back(itrSet->location);
            }
            itrSet++;
        }

        // Random choice of the remaining best moves
        auto rnd = rand() % pickMoves.size();

        // Add the play, so we can 'see' it before the server returns
        if (player->AddMove(game, pickMoves[rnd].column))
        {
            game.Cells[pickMoves[rnd].column][pickMoves[rnd].row] = game.GetCurrentPlayer();
            // Swap, for UI.
            game.SwapPlayer();
        }
    }
    return true;
}

int main(int num, void** ppArg)
{
    auto spBotPlayer = std::make_shared<Api::BotPlayer>(teamName, password);
    if (!spBotPlayer->Valid())
    {
        std::cout << "Failed init!" << std::endl;
        return 1;
    }

    Game game;
#ifdef WIN32
    CreateGameWindow();
#endif

    do
    {
        if (!spBotPlayer->GetNextMove(game))
        {
            std::cout << "Game state failed!" << std::endl;
            return 1;
        }

        // Show the board
        PrintBoard(game);

        // Get the status to see if we are finished
        bool finished;
        auto statusText = game.GetStatusString(finished);
        if (!finished)
        {
            // If our turn, play a move
            if (game.IsPlayerTurn(spBotPlayer->GetOpponentID()))
            {
                Node root;
                root.location.score = 0;
                
                std::cout << "Thinking...." << std::endl;
                GenerateMove(spBotPlayer.get(), game, root, 0);

            }
            PrintBoard(game);
        }
        else
        {
            // Finished, so wait for quit or new
            std::cout << "n for new game, q to quit:";
            auto k = getchar();
            if (k == 'n')
            {
                spBotPlayer->NewGame(game);
            }
            else if (k == 'q')
            {
                break;
            }
        }
    } while (1);
}