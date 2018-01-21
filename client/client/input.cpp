#include "input.h"
#include <Qt>

Input::Input()
{

}

void Input::interpretInput(int key){
    switch(key){
    case Qt::Key_W:
        mainWindow->writeData("/u");
        break;
    case Qt::Key_S:
        mainWindow->writeData("/d");
        break;
    case Qt::Key_A:
        mainWindow->writeData("/l");
        break;
    case Qt::Key_D:
        mainWindow->writeData("/r");
        break;
    case Qt::Key_Space:
        mainWindow->writeData("/b");
        break;
    }
}


void Input::setMainWindow(MainWindow *mainWindow){
    this->mainWindow = mainWindow;
}
