#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtNetwork>
#include <string>

class Painter;
class Input;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    Painter* painter;
    Input* input;

    char startButtonBehaviour;

    QString IP;
    int port;
    bool connectBool;

protected:
    void keyPressEvent(QKeyEvent *);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setIP();
    void setPort();
    void connectTCP();
    void readData();
    void startGame();
    void writeData(std::string message);
    void handleConnectedTCP();
    void socketDisconnected();

    void startButton(bool start, bool paused);
    void currentPlayer();
    void votersMessage(bool start, bool pause, char numberOfVoters);
    void endedGame();


    QTcpSocket * sock = 0;
};

#endif // MAINWINDOW_H
