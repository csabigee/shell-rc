/*! \file led.cpp
 *  \brief This is a simple coloured circle that can indicate a binary state.
 *	\details
 *	\author Csaba Hegedűs
 *	\copyright Csaba Hegedűs
 *	\date 2016
 */

#include <QWidget>
#include <QtGui>
#include <QtCore>
#include <QColor>

#include "led.h"

/*!
 * \brief The constructor of the led widget.
 * \details The constructor sets the circle color to red and shows it.
 * \param parent
 */
Led::Led(QWidget *parent) : QWidget(parent)
{
    /*!
     * Set the size of the widget.
     */
    setMinimumWidth(24);
    setMinimumHeight(24);
    setMaximumWidth(24);
    setMaximumHeight(24);

    /*!
     * Set the color of the circle to red.
     */
    ledColor = Qt::darkGreen;

    /*!
     * Show the widget.
     */
    show();
}

/*!
 * \brief Led::setState
 * \param state
 */
void Led::setState(bool state)
{
    /*!
     * If \p<state> is true, set the color of the circle to green,
     * if \p<state> is false, set the color of the circle to red.
     */
    if(state)
        ledColor = Qt::green;
    else
        ledColor = Qt::darkGreen;

    /*!
     * Update the widget, which triggers a paintEvent(QPaintEvent *e).
     */
    update();
}

/*!
 * \brief Led::paintEvent
 * \param e
 */
void Led::paintEvent(QPaintEvent *e)
{
    // draw circle
    QPainter painter(this);

    painter.setBrush(ledColor);
    painter.setPen(Qt::black);
    painter.drawEllipse(QRect(2,2,20,20));
}

/*!
 * \brief Led::~Led
 */
Led::~Led()
{

}
