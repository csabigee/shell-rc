/*! \file led.h
 *  \brief This is a simple coloured circle that can indicate a binary state.
 *	\details This widget can be used to indicate a state, for example if a connection is established or not.
 *	\author Csaba Hegedűs
 *	\copyright Csaba Hegedűs
 *	\date 2016
 */

#ifndef LED_H
#define LED_H

#include <QWidget>

/*!
 * \brief The Led class is a widget. It draws a fixed size (20x20) red circle. The default contructor sets the color to red. By setting the state to true via the setState(bool state) function, the color changes from dark red to light red. This widget can be used to indicate for example a network connection status.
 */
class Led : public QWidget
{
    Q_OBJECT
public:
    explicit Led(QWidget *parent = 0);
    ~Led();
    void setState(bool state);

private:
    QColor ledColor;

protected:
    void paintEvent(QPaintEvent *e);

signals:

public slots:

private slots:

protected slots:

};

#endif // LED_H
