#include <QLabel>
#include <QGridLayout>
#include <QTimer>
#include <QGroupBox>
#include "controller.h"
#include "led.h"

Controller::Controller(QString name, QWidget *parent)
    : QWidget{parent}
{
    l_name = new QLabel(name);
    indicator = new Led;
    lo_main = new QGridLayout;
    lo_outline = new QGridLayout;
    gb_outline = new QGroupBox;

    lo_main->addWidget(l_name,0,0);
    lo_main->addWidget(indicator,0,1);
    gb_outline->setLayout(lo_main);
    lo_outline->addWidget(gb_outline);
    this->setLayout(lo_outline);

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
