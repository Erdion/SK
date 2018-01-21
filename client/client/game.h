#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include "mainwindow.h"

class Game : public QWidget
{
    Q_OBJECT
public:
    enum Field{
        PLAYER1,
        PLAYER2,
        PLAYER3,
        PLAYER4,
        BOMB,
        DESTRUCTIBLE,
        WALL,
        FLAME,
        EMPTY,
    };

private:

    static int player;
    static bool started;
    static bool paused;
    static std::string whoWonLastGame;
    static int numberOfVoters;

    static std::string notCompleteMessage;

    static std::vector<std::vector<Field>> board;
    static std::string convertToBoard(std::string message);
    static std::string setGameVariables(std::string message);

    static MainWindow* mainWindow;

public:
    static void clearBoard();
    static void interpretMessage(std::string message);
    static void setMainWindow(MainWindow* mainWindow);
    static std::vector<std::vector<Field>> getBoard();
};

#endif // GAME_H
