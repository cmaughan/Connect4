#include "Game.h"
#include <cstdio>
#include <iostream>
#include <sstream>

// EvalNum is an easy way to track visitations of disks during evaluation
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

// All possible move locations for any player
std::vector<Location> Game::GetValidMoves()
{
    std::vector<Location> moves;
    for (int column = 0; column < Game::NUMBER_OF_COLUMNS; column++)
    {
        for (int row = 0; row < Game::NUMBER_OF_ROWS; row++)
        {
            if (Cells[column][row] == CellContent::Empty)
            {
                moves.push_back(Location{ column, row, 0 });
                break;
            }
        }
    }
    return moves;
}

// Given a worm (a string of up to 7 cells that are either empty or the current player type)
// Calculate a 'score' based on how valuable we think the worm is
int Game::EvaluateWorm(Worm& worm)
{
    int score = 0;
    int numCounters = 0;
    int continuousString = 0;
    int numEmpty = 0;

    // Can never win with this worm
    if (worm.length < 4)
    {
        return 0;
    }

    for (int i = 0; i < worm.length; i++)
    {
        // Empties are worth a little
        if (worm.cells[i] == CellContent::Empty)
        {
            numEmpty++;
            continuousString = 0;
        }
        else
        {
            // Continuous strings of counters are worth exponentially more
            numCounters++;
            continuousString++;
            if (continuousString == 4)
            {
                return WON_VALUE + score;
            }
            else
            {
                score += (continuousString * continuousString * continuousString * 10);
            }
        }
    }

    // Add the empty bonus
    score += (numEmpty * 1);
    return score;
}

// Find a vertical worm of the players type on the current column
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
    }
}

// Find a continous train of horizontal player or empty slots
void Game::GetHorizontalWorm(Worm& worm, CellContent currentPlayer, int column)
{
    // Find a counter at the top of this column
    worm.length = 0;
    auto row = FindFirstColumnRow(column, currentPlayer);
    if (row < 0 || EvalNum[column][row] == EvalCount)
    {
        return;
    }

    // Walk back until we find the opposition
    while (column >= 0 &&
        (Cells[column][row] != GetOtherPlayer(currentPlayer)))
    {
        column--;
    }
    column++;

    // Remember the chain
    while (column < Game::NUMBER_OF_COLUMNS)
    {
        if (Cells[column][row] == GetOtherPlayer(currentPlayer))
        {
            break;
        }
        worm.cells[worm.length++] = Cells[column][row];
        EvalNum[column][row] = EvalCount;
        column++;
    }
}

// Find the first row that the player is in for a given column.
int Game::FindFirstColumnRow(int column, CellContent player)
{
    int row = Game::NUMBER_OF_ROWS - 1;
    while(row >= 0)
    {
        if (player == Cells[column][row])
        {
            return row;
        }
        else if (GetOtherPlayer(player) == Cells[column][row])
        {
            return -1;
        }
        row--;
    }

    return -1;
}

// Find a continuous row of empty/filled counters of the player's type
void Game::GetDiagonalWorm(Worm& worm, CellContent currentPlayer, int column, bool forward)
{
    worm.length = 0;

    auto row = FindFirstColumnRow(column, currentPlayer);
    if (row <= 0 || EvalNum[column][row] == EvalCount)
    {
        return;
    }

    int stepDir = forward ? 1 : -1;

    // Walk in the step direction until we find something that isn't our counter type
    while (column < Game::NUMBER_OF_COLUMNS &&
        column >= 0 &&
        row < Game::NUMBER_OF_ROWS &&
        row >= 0)
    {
        auto& cell = Cells[column][row];
        if (cell == GetOtherPlayer(currentPlayer))
        {
            break;
        }
        row--;
        column += stepDir;
    }
    row++;
    column -= stepDir;

    // Walk back up the diagonal, finding empties and our player types
    while (column < Game::NUMBER_OF_COLUMNS &&
        column >= 0 &&
        row < Game::NUMBER_OF_ROWS &&
        row >= 0)
    {
        auto& cell = Cells[column][row];
        if (cell != GetOtherPlayer(currentPlayer))
        {
            worm.cells[worm.length++] = cell;
            EvalNum[column][row] = EvalCount;
        }
        else
        {
            break;
        }
        column -= stepDir;
        row++;
    }
}

// Get all the worms, add their scores
int Game::EvaluatePosition()
{
    int score = 0;
    auto currentPlayer = GetCurrentPlayer();

    Worm worm;

    EvalCount++;
    for (int column = 0; column < Game::NUMBER_OF_COLUMNS; column++)
    {
        GetVerticalWorm(worm, currentPlayer, column);
        score += EvaluateWorm(worm);
    }

    EvalCount++;
    for (int column = 0; column < Game::NUMBER_OF_COLUMNS; column++)
    {
        GetHorizontalWorm(worm, currentPlayer, column);
        score += EvaluateWorm(worm);
    }

    EvalCount++;
    for (int column = 0; column < Game::NUMBER_OF_COLUMNS; column++)
    {
        GetDiagonalWorm(worm, currentPlayer, column, false);
        score += EvaluateWorm(worm);
    }

    EvalCount++;
    for (int column = 0; column < Game::NUMBER_OF_COLUMNS; column++)
    {
        GetDiagonalWorm(worm, currentPlayer, column, true);
        score += EvaluateWorm(worm);
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
