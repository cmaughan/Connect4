#include "Api.h"

#include "rest/Response.hpp"
#include "rest/UrlRequest.hpp"

#include "json.hpp"
using json = nlohmann::json;

namespace Api
{

const std::string site("yorkdojoconnect4.azurewebsites.net");

BotPlayer::BotPlayer(const std::string& teamName, const std::string& password)
{
#ifdef WIN32
    auto wVersionRequested = MAKEWORD(2, 2);
    WSAData wsaData;
    int ret = WSAStartup(wVersionRequested, &wsaData);
#endif

    UrlRequest request;
    request.host(site.c_str());
    request.uri(registerApi.c_str(),
    {
        {teamNameParam.c_str(), teamName.c_str()},
        {passwordParam.c_str(), password.c_str()}
    });
    request.method("POST");
    request.addHeader("Content-Type: application/json\nContent-Length: 0");
    try
    {
        auto response = std::move(request.perform());
        if (response.statusCode() == 200)
        {
            std::string body = response.body();
            // Unquote the response
            body = body.erase(0, 1);
            body = body.erase(body.size() - 1);
            clientID = body;
        }
        else
        {
            cout << "status code = " << response.statusCode() << ", description = " << response.statusDescription() << endl;
        }
    }
    catch (std::exception&)
    {
    }
    this->password = password;
}

bool BotPlayer::GetNextMove(Game& game)
{
    UrlRequest request;
    request.host(site.c_str());
    request.uri(getGameApi.c_str(),
    {
        {playerIDParam.c_str(), clientID}
    });
    request.addHeader("Content-Type: application/json\nContent-Length: 0");
    try
    {
        auto response = std::move(request.perform());
        if (response.statusCode() == 200)
        {
            std::string body = response.body();
            auto gameData = json::parse(body);
            game.CurrentState = CurrentGameState(gameData["CurrentState"].get<int>());
            game.YellowPlayerID = gameData["YellowPlayerID"].get<std::string>();
            game.RedPlayerID = gameData["RedPlayerID"].get<std::string>();
            game.ID = gameData["ID"].get<std::string>();

            auto cells = gameData["Cells"];
            for (int column = 0; column < Game::NUMBER_OF_COLUMNS; column++)
            {
                for (int row = 0; row < Game::NUMBER_OF_ROWS; row++)
                {
                    game.Cells[column][row] = CellContent(cells[column][row].get<int>());
                }
            }
            return true;
        }
        else
        {
            cout << "status code = " << response.statusCode() << ", description = " << response.statusDescription() << endl;
            return "";
        }
    }
    catch (std::exception&)
    {
        return "";
    }
}
bool BotPlayer::AddMove(Game& game, int column)
{
    UrlRequest request;
    request.host(site.c_str());
    request.uri(makeMoveApi.c_str(),
    {
        {playerIDParam.c_str(), clientID},
        {passwordParam.c_str(), password},
        {columnNumberParam.c_str(), column},
    });
    request.method("POST");
    request.addHeader("Content-Type: application/json\nContent-Length: 0");
    try
    {
        auto response = std::move(request.perform());
        if (response.statusCode() == 200)
        {
            return true;
        }
        else
        {
            cout << "status code = " << response.statusCode() << ", description = " << response.statusDescription() << endl;
            return false;
        }
    }
    catch (std::exception&)
    {
        return "";
    }
}

bool BotPlayer::NewGame(Game& game)
{
    UrlRequest request;
    request.host(site.c_str());
    request.uri(newGameApi.c_str(),
    {
        {playerIDParam.c_str(), clientID}
    });
    request.method("POST");
    request.addHeader("Content-Type: application/json\nContent-Length: 0");
    try
    {
        auto response = std::move(request.perform());
        if (response.statusCode() == 200)
        {
            return true;
        }
        else
        {
            cout << "status code = " << response.statusCode() << ", description = " << response.statusDescription() << endl;
            return false;
        }
    }
    catch (std::exception&)
    {
        return false;
    }
}



HumanPlayer::HumanPlayer()
{
}
 
bool HumanPlayer::GetNextMove(Game& game)
{
    /*
    if (game.CurrentState == CurrentGameState::YellowToPlay)
    {
        
    }
    */
    return true;
}

bool HumanPlayer::AddMove(Game& game, int columnNumber)
{
    for (int row = 0; row < Game::NUMBER_OF_ROWS; row++)
    {
        if (game.Cells[columnNumber][row] == CellContent::Empty)
        {
            if (game.CurrentState == CurrentGameState::YellowToPlay)
            {
                game.Cells[columnNumber][row] = CellContent::Yellow;
                game.CurrentState = CurrentGameState::RedToPlay;
            }
            else
            {
                game.Cells[columnNumber][row] = CellContent::Red;
                game.CurrentState = CurrentGameState::YellowToPlay;
            }
            return true;
        }
    }
    return false;
}

bool HumanPlayer::NewGame(Game& game)
{
    for (int column = 0; column < Game::NUMBER_OF_COLUMNS; column++)
    {
        for (int row = 0; row < Game::NUMBER_OF_ROWS; row++)
        {
            game.Cells[column][row] = CellContent::Empty;
        }
    }
    game.CurrentState = (rand() % 2) == 0 ? CurrentGameState::YellowToPlay : CurrentGameState::RedToPlay;
    game.YellowPlayerID = "Yellow";
    game.RedPlayerID = "Red";
    game.ID = "Game";
    return true;
}

}
