#include "mainwindow.h"
#include "game.h"
#include "ui_mainwindow.h"
#include "painter.h"
#include "input.h"
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    startButtonBehaviour = 'S';

    painter = new Painter();
    input = new Input();
    Game::setMainWindow(this);
    ui->setupUi(this);
    ui->gridGame->addWidget(painter);
    painter->setMainWindow(this);
    input->setMainWindow(this);
    connectBool = true;
    QWidget::setFocus();

    connect(ui->spinPort, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MainWindow::setPort);
    connect(ui->textIP, &QLineEdit::textChanged, this, &MainWindow::setIP);
    connect(ui->buttonConnect, &QPushButton::clicked, this, &MainWindow::connectTCP);
    connect(ui->buttonStart, &QPushButton::clicked, this, &MainWindow::startGame);
    QWidget::setFocusPolicy(Qt::StrongFocus);
}

void MainWindow::keyPressEvent(QKeyEvent* event){
    QKeyEvent* qKeyEvent = static_cast<QKeyEvent *>(event);
    input->interpretInput(qKeyEvent->key());
}

void MainWindow::setIP(){
    this->IP = ui->textIP->text();
}


void MainWindow::setPort(){
    this->port = ui->spinPort->value();
}

void MainWindow::connectTCP(){
    if(connectBool) {
        this->IP = ui->textIP->text();
        this->port = ui->spinPort->value();

        sock = new QTcpSocket(this);
        connect(sock, &QTcpSocket::readyRead, this, &MainWindow::readData);
        connect(sock, &QTcpSocket::disconnected, this, &MainWindow::socketDisconnected);
        sock->connectToHost(IP, port);
        if(!sock->waitForConnected(3000))
        {
            socketDisconnected();
        }
        else{
            ui->textIP->setEnabled(false);
            ui->spinPort->setEnabled(false);
            ui->buttonConnect->setText("Disconnect");
            ui->buttonStart->setEnabled(true);
            connectBool = false;
        }
    }
    else{
        ui->textIP->setEnabled(true);
        ui->spinPort->setEnabled(true);
        ui->buttonConnect->setText("Connect");
        ui->buttonStart->setEnabled(false);
        ui->buttonStart->setText("start");
        sock->disconnectFromHost();
        connectBool = true;
        Game::clearBoard();
        QWidget::update();
    }

}

void MainWindow::endedGame(){
    votersMessage(false, false, 0);
    std::string text = "This game has ended!";
    QMessageBox::information(
        this,
        tr("Game ended"),
        tr(text.c_str()));
}


void MainWindow::socketDisconnected(){
    std::string text = "Error: " + sock->errorString().toStdString() + ".";
    if(sock->error() != -1){
        QMessageBox::information(
            this,
            tr("Error"),
            tr(text.c_str()));
    }
    ui->textIP->setEnabled(true);
    ui->spinPort->setEnabled(true);
    ui->buttonConnect->setText("Connect");
    connectBool = true;
    ui->buttonStart->setEnabled(false);
    ui->buttonStart->setText("start");
    Game::clearBoard();
    QWidget::update();
}

void MainWindow::readData(){
    QByteArray data = sock->readAll();
    std::string dataStr = data.toStdString();
    Game::interpretMessage(dataStr);
    QWidget::update();
}

void MainWindow::startGame(){
    QWidget::setFocus();
    if(startButtonBehaviour == 'S'){
        writeData("/s");
    }
    if(startButtonBehaviour == 'P'){
        writeData("/p");
    }
    if(startButtonBehaviour == 'R'){
        writeData("/w");
    }
}

void MainWindow::writeData(std::string message){
    message += "E";
    sock->write(message.c_str());
}



void MainWindow::startButton(bool start, bool pause){
    if(!start){
        startButtonBehaviour = 'S';
        ui->buttonStart->setText("Start");
    }
    else if(pause){
        startButtonBehaviour = 'R';
        ui->buttonStart->setText("Resume");
    }
    else{
        startButtonBehaviour = 'P';
        ui->buttonStart->setText("Pause");
    }
}

void MainWindow::currentPlayer(){

}

void MainWindow::votersMessage(bool start, bool pause, char numberOfVoters){
    std::string playerVer = "players";
    if(numberOfVoters == '1'){
        playerVer = "player";
    }
    std::string text = "";
    if(!start){
        text = "starting";
    }
    else if(pause){
        text = "pausing";
    }
    else{
        text = "resuming";
    }
    text = std::string(1, numberOfVoters) + std::string(" ") + playerVer + std::string(" voted for ") + text + std::string(" the game.");
    ui->labelVoters->setText(QString::fromStdString(text));
}

MainWindow::~MainWindow()
{
    delete painter;
    delete input;
    delete ui;
}
