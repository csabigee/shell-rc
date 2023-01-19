#include <QLabel>
#include <QGridLayout>
#include <QTimer>
#include "controller.h"
#include "led.h"

Controller::Controller(QString name, QWidget *parent)
    : QWidget{parent}
{
    l_name = new QLabel(name);
    indicator = new Led;
    lo_main = new QGridLayout;

    lo_main->addWidget(l_name,0,0);
    lo_main->addWidget(indicator,0,1);
    this->setLayout(lo_main);

    m_flashTimer.setSingleShot(true);
    m_flashTimer.setInterval(100);
    connect(&m_flashTimer, &QTimer::timeout, this, &Controller::flashIndicatorTo);
}

void Controller::flashIndicator()
{
    indicator->setState(true);
    m_flashTimer.start();
}

void Controller::flashIndicatorTo()
{
    indicator->setState(false);
}
