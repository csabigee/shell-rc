#include <QLabel>
#include <QGridLayout>
#include <QTimer>
#include <QGroupBox>
#include "controller.h"
#include "led.h"

Controller::Controller(QString name, QWidget *parent)
    : QWidget{parent}
{
    this->setFixedHeight(128);
    loOutline = new QGridLayout;
    loOutline->setContentsMargins(0,0,0,0);
    gbOutline = new QGroupBox;
    loMain = new QGridLayout;
    loMain->setContentsMargins(8,8,8,8);
    laName = new QLabel(name);
    indicator = new Led;

    loMain->addWidget(laName,0,0,Qt::AlignCenter);
    loMain->addWidget(indicator,1,0,Qt::AlignCenter);
    gbOutline->setLayout(loMain);
    loOutline->addWidget(gbOutline);
    this->setLayout(loOutline);

    flashTimer.setSingleShot(true);
    flashTimer.setInterval(flashingIntervalMs);
    connect(&flashTimer, &QTimer::timeout, this, &Controller::flashIndicatorTo);

}

void Controller::flashIndicator()
{
    indicator->setState(true);
    flashTimer.start();
}

void Controller::flashIndicatorTo()
{
    indicator->setState(false);
}
