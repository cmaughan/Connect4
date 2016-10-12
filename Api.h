#pragma once
#include <string>
#include "Game.h"

namespace Api
{

const std::string passwordParam("password");
const std::string playerIDParam("playerID");

// Register
const std::string registerApi("/api/Register");
const std::string teamNameParam("teamName");

// GameState
const std::string getGameApi("/api/GameState");

// MakeMove
const std::string makeMoveApi("/api/MakeMove");
const std::string columnNumberParam("columnNumber");

// NewGame
const std::string newGameApi("/api/NewGame");

class Player
{
public:
    virtual bool GetNextMove(Game& game) = 0;
    virtual bool AddMove(Game& game, int columnNumber) = 0;
    virtual bool NewGame(Game& game) = 0;
    virtual bool Valid() { return true; }
    virtual std::string GetOpponentID() { return "Player"; }
};

class BotPlayer : public Player
{
public:
    BotPlayer(const std::string& teamName, const std::string& password);

    virtual bool Valid() override { return !clientID.empty(); }
    virtual bool GetNextMove(Game& game);
    virtual bool AddMove(Game& game, int columnNumber) override;
    virtual bool NewGame(Game& game) override;
    virtual std::string GetOpponentID() { return clientID; }
private:
    std::string password;
    std::string clientID;
};

class HumanPlayer : public Player
{
public:
    HumanPlayer();

    virtual bool GetNextMove(Game& game);
    virtual bool AddMove(Game& game, int columnNumber) override;
    virtual bool NewGame(Game& game) override;
private:
};


}