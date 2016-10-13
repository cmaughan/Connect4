#include "Game.h"
#include <cstdio>
#include <iostream>

Game::Game()
{
    for (int column = 0; column < Game::NUMBER_OF_COLUMNS; column++)
    {
        for (int row = 0; row < Game::NUMBER_OF_ROWS; row++)
        {
            EvalNum[column][row] = 0;
        }
    }
}
std::vector<Location> Game::GetValidMoves()
{
    std::vector<Location> moves;
    for (int column = 0; column < Game::NUMBER_OF_COLUMNS; column++)
    {
        for (int row = 0; row < Game::NUMBER_OF_ROWS; row++)
        {
            if (Cells[column][row] == CellContent::Empty)
            {
                moves.push_back(Location{ column, row, 0});
                break;
            }
        }
    }
    return moves;
}


int Game::EvaluateWorm(Worm& worm)
{
    int score = 0;
    int numCounters = 0;
    int continuousString = 0;
    int numEmpty = 0;
    if (worm.length < 4)
    {
        return 0;
    }
    for (int i = 0; i < worm.length; i++)
    {
        if (worm.cells[i] == CellContent::Empty)
        {
            numEmpty++;
            continuousString = 0;
        }
        else
        {
            numCounters++;
            continuousString++;
            if (continuousString == 4)
            {
                return 1000000;
            }
        }
    }
    score += (numCounters * numCounters * numCounters * 10);
    score += (numEmpty *  1);
    return score;
}

void Game::GetVerticalWorm(Worm& worm, CellContent currentPlayer, int column)
{
    bool foundMe = false;
    worm.length = 0;
    for (int row = 0; row < Game::NUMBER_OF_ROWS; row++)
    {
        if (Cells[column][row] == CellContent::Empty)
        {
            if (!foundMe)
            {
                worm.length = 0;
                return;
            }
            worm.cells[worm.length++] = CellContent::Empty;
        }
        else if (Cells[column][row] == currentPlayer)
        {
            foundMe = true;
            worm.cells[worm.length++] = currentPlayer;
        }
        else if (Cells[column][row] != currentPlayer)
        {
            worm.length = 0;
            foundMe = false;
            continue;
        }

        if (worm.length >= 7)
        {
            return;
        }
    }
}

void Game::GetHorizontalWorm(Worm& worm, CellContent currentPlayer, int column)
{
    worm.length = 0;
    int row = Game::NUMBER_OF_ROWS - 1;
    for (; row >=0; row--)
    {
        if (EvalNum[column][row] == EvalCount)
        {
           return;
        }
        if (Cells[column][row] == CellContent::Empty)
        {
            continue;
        }
        else if (Cells[column][row] != currentPlayer)
        {
            return;
        }
        else
        {
            break;
        }
    }

    if (row < 0)
    {
        return;
    }

    if (column > 0)
    {
        for (int backColumn = column - 1; backColumn >= 0; backColumn--)
        {
            if (Cells[backColumn][row] == CellContent::Empty ||
                Cells[backColumn][row] == currentPlayer)
            {
                worm.cells[worm.length++] = Cells[backColumn][row];
                EvalNum[backColumn][row] = EvalCount;
            }
            else
            {
                break;
            }
        }
    }

    worm.cells[worm.length++] = currentPlayer;

    if (column < Game::NUMBER_OF_COLUMNS - 1)
    {
        bool stoppedMe = false;
        for (int forwardColumn = column + 1; forwardColumn < Game::NUMBER_OF_COLUMNS; forwardColumn++)
        {
            if (Cells[forwardColumn][row] == CellContent::Empty)
            {
                worm.cells[worm.length++] = CellContent::Empty;
                EvalNum[forwardColumn][row] = EvalCount;
            }
            else if (Cells[forwardColumn][row] == currentPlayer)
            {
                worm.cells[worm.length++] = currentPlayer;
                EvalNum[forwardColumn][row] = EvalCount;
            }
            else if (Cells[forwardColumn][row] != currentPlayer)
            {
                break;
            }
        }
    }
}

int Game::EvaluatePosition()
{
    int score = 0;
    auto currentPlayer = CurrentState == CurrentGameState::YellowToPlay ? CellContent::Yellow : CellContent::Red;

    Worm vWorm;
    Worm hWorm;

    EvalCount++;
    for (int column = 0; column < Game::NUMBER_OF_COLUMNS; column++)
    {
        GetVerticalWorm(vWorm, currentPlayer, column);

        int vWormScore = 0;
        if (vWorm.length > 0)
        {
            vWormScore = EvaluateWorm(vWorm);
        }

        std::cout << "C" << std::to_string(column) << " : VWorm: " << vWormScore << std::endl;
        score += vWormScore;
    }

    EvalCount++;
    for (int column = 0; column < Game::NUMBER_OF_COLUMNS; column++)
    {
        GetHorizontalWorm(hWorm, currentPlayer, column);

        int hWormScore = 0;
        if (hWorm.length > 0)
        {
            hWormScore = EvaluateWorm(hWorm);
        }
        
        std::cout << "C" << std::to_string(column) << " : HWorm: " << hWormScore << std::endl;
        score += hWormScore;
    }
    return score;
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
