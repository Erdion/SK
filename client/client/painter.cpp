#include "painter.h"
#include "qpainter.h"
#include "game.h"
#include "mainwindow.h"
#include <vector>
#include <map>
#include <string>


Painter::Painter(QWidget *parent) : QWidget(parent)
{
    std::map<Game::Field, std::string> pixmapData;
    //renderArea = new RenderArea;
    pixmapData[Game::PLAYER1] = "player1.png";
    pixmapData[Game::PLAYER2] = "player2.png";
    pixmapData[Game::PLAYER3] = "player3.png";
    pixmapData[Game::PLAYER4] = "player4.png";
    pixmapData[Game::BOMB] = "bomb.png";
    pixmapData[Game::DESTRUCTIBLE] = "destructible.png";
    pixmapData[Game::WALL] = "wall.png";
    pixmapData[Game::FLAME] = "flame.png";
    pixmapData[Game::EMPTY] = "empty.png";
    for(auto data: pixmapData){
        pixmaps[data.first] = new QPixmap(QString::fromStdString(":/Resources/" + data.second));
    }
}

void Painter::setMainWindow(MainWindow *mainWindow){
    this->mainWindow = mainWindow;
}

void Painter::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    auto board = Game::getBoard();
    for(unsigned int i = 0; i < board.size(); i++){
        for(unsigned int j = 0; j < board[i].size(); j++){
            painter.drawPixmap(40*j,40*i,30,30, *pixmaps[board[i][j]]);
        }
    }
}
