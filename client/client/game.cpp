#include "game.h"
#include "ui_mainwindow.h"

#include <string>
#include <vector>

MainWindow* Game::mainWindow;
std::vector<std::vector<Game::Field>> Game::board;
int Game::player;
std::string Game::whoWonLastGame;
bool Game::started;
bool Game::paused;
int Game::numberOfVoters;
std::string Game::notCompleteMessage;


std::string Game::convertToBoard(std::string message){
    board.clear();
    int width = message.find('N');
    int length = message.find('S');
    int height = length / (width + 1);
    for(int x = 0; x < height; x++){
        std::vector<Game::Field> temp;
        for(int y = 0; y < (width); y++){
            switch(message[y + x * (width + 1)]){
            case '1':
                temp.push_back(PLAYER1);
                break;
            case '2':
                temp.push_back(PLAYER2);
                break;
            case '3':
                temp.push_back(PLAYER3);
                break;
            case '4':
                temp.push_back(PLAYER4);
                break;
            case 'B':
                temp.push_back(BOMB);
                break;
            case 'D':
                temp.push_back(DESTRUCTIBLE);
                break;
            case '#':
                temp.push_back(WALL);
                break;
            case '*':
                temp.push_back(FLAME);
                break;
            case '.':
                temp.push_back(EMPTY);
                break;
            case 'N':
                break;
            case 'S':
                break;
            default:
                throw "Corrupted message";
                break;
            }
        }
        board.push_back(temp);
    }
    return message.substr(length + 1, message.length() - length - 1);
}
std::string Game::setGameVariables(std::string message){
    if(message[0] == 'T'){
        if(!started){
            started = true;
            mainWindow->startButton(started, paused);
        }
    }
    else if(message[0] == 'F'){
        if(started){
            started = false;
            mainWindow->startButton(started, paused);
        }
    }

    if(message[1] == 'T'){
        if(!paused){
            paused = true;
            mainWindow->startButton(started, paused);
        }
    }
    else if(message[1] == 'F'){
        if(paused){
            paused = false;
            mainWindow->startButton(started, paused);
        }
    }

    if(player != '0' - message[2]){
        player = '0' - message[2];
    }

    if(message[3] == 'T'){
        mainWindow->endedGame();
    }

    whoWonLastGame = message[4];
    if(message[5] != numberOfVoters){
        numberOfVoters = '0' - message[5];
        mainWindow->votersMessage(started, paused, message[5]);
    }
    if(message[7] == '\0'){
        return "";
    }
    return message.substr(7);
}

void Game::clearBoard(){
    board.clear();;
}

void Game::interpretMessage(std::string message){
    message = notCompleteMessage + message;
    while((int)message.find('E') != -1){
        if(message.size() >= 2){
            std::string typeOfMessage = message.substr(0,2);
            if(typeOfMessage == "/b"){
                message = convertToBoard(message.substr(2, message.length() - 2));
                message = setGameVariables(message);
            }
            else{
                break;
            }
        }
    }
    notCompleteMessage = message;
}

void Game::setMainWindow(MainWindow *mainWindow){
    Game::mainWindow = mainWindow;
}


std::vector<std::vector<Game::Field>> Game::getBoard(){
    return board;
}
