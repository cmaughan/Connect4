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

const std::string password("Foobar2");
const std::string teamName("Morninator");

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

bool PlayMove(Api::Player* player, Game& game)
{
    auto moves = game.GetValidMoves();

    if (moves.empty())
        return false;
    
    auto playColor = game.CurrentState == CurrentGameState::YellowToPlay ? CellContent::Yellow : CellContent::Red;

    for (unsigned int index = 0; index < moves.size(); index++)
    {
        auto& move = moves[index];
        auto previousValue = game.Cells[move.column][move.row];
        game.Cells[move.column][move.row] = playColor;

        std::cout << "Move on column: " << move.column << std::endl;
        move.score = game.EvaluatePosition();

        game.Cells[move.column][move.row] = previousValue;
    }

    std::sort(moves.begin(), moves.end());

    std::vector<Location> pickMoves;
    int count = -1;
    auto itrSet = moves.rbegin();
    auto itrSetEnd = moves.rend();
    while (itrSet != itrSetEnd)
    {
        if (count == -1)
        {
            count = itrSet->score;
            pickMoves.push_back(*itrSet);
        }
        else
        {
            if (itrSet->score < count)
            {
                break;
            }
            pickMoves.push_back(*itrSet);
        }
        itrSet++;
    }

    auto rnd = rand() % pickMoves.size();

    // Add the play, so we can 'see' it before the server returns
    if (player->AddMove(game, pickMoves[rnd].column))
    {
        game.Cells[pickMoves[rnd].column][pickMoves[rnd].row] = game.CurrentState == CurrentGameState::YellowToPlay ? CellContent::Yellow : CellContent::Red;
        game.CurrentState = game.CurrentState == CurrentGameState::YellowToPlay ? CurrentGameState::RedToPlay : CurrentGameState::YellowToPlay;
        return true;
    }
    return false;
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

        PrintBoard(game);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        bool finished;
        auto statusText = game.GetStatusString(finished);
        if (!finished)
        {
            if (game.IsPlayerTurn(spBotPlayer->GetOpponentID()))
            {
                PlayMove(spBotPlayer.get(), game);
            }
            PrintBoard(game);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        else
        {
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