#ifndef PAINTER_H
#define PAINTER_H

#include <QWidget>

class Painter : public QWidget
{
    Q_OBJECT
public:
    explicit Painter(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // PAINTER_H