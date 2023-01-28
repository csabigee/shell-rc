#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QGroupBox>
#include <QTimer>
#include "led.h"

class Controller : public QWidget
{
    Q_OBJECT
public:
    explicit Controller(QString name, QWidget *parent = nullptr);
private:
    QGroupBox* gb_outline;
    QGridLayout* lo_outline;
    QLabel* l_name;
    Led* indicator;
    QGridLayout* lo_main;
    QTimer m_flashTimer;

public slots:
    void flashIndicator();
    void flashIndicatorTo();

signals:

};

#endif // CONTROLLER_H
