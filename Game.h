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
};

struct Game
{
    std::string GetStatusString(bool& finished);
    std::vector<Location> GetValidMoves();
    bool IsPlayerTurn(const std::string& player);

    static const int NUMBER_OF_COLUMNS = 7;
    static const int NUMBER_OF_ROWS = 6;

    CellContent Cells[NUMBER_OF_COLUMNS][NUMBER_OF_ROWS];
    CurrentGameState CurrentState;
    std::string YellowPlayerID;
    std::string RedPlayerID;
    std::string ID;
};


