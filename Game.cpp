#include "Game.h"

std::vector<Location> Game::GetValidMoves()
{
    std::vector<Location> moves;
    for (int column = 0; column < Game::NUMBER_OF_COLUMNS; column++)
    {
        for (int row = 0; row < Game::NUMBER_OF_ROWS; row++)
        {
            if (Cells[column][row] == CellContent::Empty)
            {
                moves.push_back(Location{ column, row });
                break;
            }
        }
    }
    return moves;
}

std::string Game::GetStatusString(bool& finished)
{
    switch (CurrentState)
    {
    case CurrentGameState::YellowToPlay:
        finished = false;
        return "Yellow to Play";
        break;
    case CurrentGameState::RedToPlay:
        finished = false;
        return "Red to Play";
        break;
    case CurrentGameState::Draw:
        finished = true;
        return "Draw";
        break;
    case CurrentGameState::GameNotStarted:
        finished = false;
        return "Not Started";
        break;
    case CurrentGameState::RedWon:
        finished = true;
        return "Red Won";
        break;
    case CurrentGameState::YellowWon:
        finished = true;
        return "Yellow Won";
        break;
    default:
        break;
    }
    finished = true;
    return "";
}

bool Game::IsPlayerTurn(const std::string& playerID)
{
    if (CurrentState == CurrentGameState::YellowToPlay)
    {
        return playerID == YellowPlayerID;
    }
    else if (CurrentState == CurrentGameState::RedToPlay)
    {
        return playerID == RedPlayerID;
    }
    return false;
}
