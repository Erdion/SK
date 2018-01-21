#ifndef PAINTER_H
#define PAINTER_H

#include <QWidget>
#include "mainwindow.h"
#include "game.h"
#include <map>

class Painter : public QWidget
{
    Q_OBJECT

    MainWindow* mainWindow;
    std::map<Game::Field, QPixmap*> pixmaps;

public:
    explicit Painter(QWidget *parent = nullptr);
    void setMainWindow(MainWindow* mainWindow);
    void paintEvent(QPaintEvent *event) override;

signals:

public slots:
};

#endif // PAINTER_H
