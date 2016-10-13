#pragma once

#include <string>
#include <vector>
enum class CurrentGameState
{
    GameNotStarted = 0,
    RedWon = 1,
    YellowWon = 2,
    RedToPlay = 3,
    YellowToPlay = 4,
    Draw = 5
};

enum class CellContent
{
    Empty = 0,
    Red = 1,
    Yellow = 2
};

struct Location
{
    int column;
    int row;
    int score;
    bool operator < (const Location& location) const
    {
        if (score < location.score)
        {
            return true;
        }
        return false;
    }
};

struct Worm
{
    CellContent cells[7];
    int length = 0;
};

struct Game
{
    Game();
    std::string GetStatusString(bool& finished);
    std::vector<Location> GetValidMoves();
    bool IsPlayerTurn(const std::string& player);
    int EvaluatePosition();
    int EvaluateWorm(Worm& worm);
    void GetVerticalWorm(Worm& worm, CellContent player, int column);
    void GetHorizontalWorm(Worm& worm, CellContent currentPlayer, int column);
    void GetDiagonalWorm(Worm& worm, CellContent currentPlayer, int column, bool forward);
    static const int NUMBER_OF_COLUMNS = 7;
    static const int NUMBER_OF_ROWS = 6;
    static const int WON_VALUE = 1000000;

    CellContent Cells[NUMBER_OF_COLUMNS][NUMBER_OF_ROWS];
    int EvalNum[NUMBER_OF_COLUMNS][NUMBER_OF_ROWS];
    CurrentGameState CurrentState;
    std::string YellowPlayerID;
    std::string RedPlayerID;
    std::string RedPlayerName;
    std::string YellowPlayerName;
    std::string ID;
    int EvalCount = 0;
};


