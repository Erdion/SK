#ifndef INPUT_H
#define INPUT_H

#include "mainwindow.h"

class Input
{
    MainWindow* mainWindow;
public:
    Input();
    void interpretInput(int key);
    void setMainWindow(MainWindow* mainWindow);
};
#endif // INPUT_H
