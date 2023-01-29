#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QGroupBox>
#include <QTimer>
#include "led.h"


constexpr int flashingIntervalMs = 80;

/*!
 * \brief The Controller class is a widget that displays the name of the controller, and flashes an "LED" when there is input action.
 */
class Controller : public QWidget
{
    Q_OBJECT
public:
    explicit Controller(QString name, QWidget *parent = nullptr);
private:
    QGroupBox *gbOutline;
    QGridLayout *loOutline;
    QGridLayout *loMain;
    QLabel *laName;
    Led *indicator;
    QTimer flashTimer;

public slots:
    void flashIndicator();
    void flashIndicatorTo();

};

#endif // CONTROLLER_H
