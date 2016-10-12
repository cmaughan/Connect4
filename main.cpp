#include "Api.h"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>

#ifdef  WIN32
#include "DrawWindow.h"
#endif

const std::string password("Foobar");
const std::string teamName("The_Morninator");
std::shared_ptr<Api::Player> spPlayer;

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

bool PlayMove(Game& game)
{
    auto moves = game.GetValidMoves();

    if (moves.empty())
        return false;

    int moveNum = (rand() % moves.size());

    // Add the play, so we can 'see' it before the server returns
    if (spPlayer->AddMove(game, moves[moveNum].column))
    {
        game.Cells[moves[moveNum].column][moves[moveNum].row] = game.CurrentState == CurrentGameState::YellowToPlay ? CellContent::Yellow : CellContent::Red;
        game.CurrentState = game.CurrentState == CurrentGameState::YellowToPlay ? CurrentGameState::RedToPlay : CurrentGameState::YellowToPlay;
        return true;
    }
    return false;
}

int main(int num, void** ppArg)
{
    spPlayer = std::make_shared<Api::BotPlayer>(teamName, password);
    if (!spPlayer->Valid())
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
        if (!spPlayer->GetNextMove(game))
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
            if (game.IsPlayerTurn(spPlayer->GetOpponentID()))
            {
                PlayMove(game);
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
                spPlayer->NewGame(game);
            }
            else if (k == 'q')
            {
                break;
            }
        }
    } while (1);

}