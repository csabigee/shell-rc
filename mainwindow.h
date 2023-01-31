#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "racecar.h"
#include "controller.h"

#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QProgressBar>
#include <QListWidget>
#include <QMouseEvent>

#include <QBluetoothDeviceDiscoveryAgent>
#include <QJoysticks.h>

#include "led.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent* event);

private:
    QPoint mpos;
    QGridLayout *loMain;
    QGridLayout *loControllers;
    QGridLayout *loRaceCars;
    QPushButton* pbExit;
    QPushButton *pbRescan;

    QJoysticks *joystickController;
    QList<Controller*> controllers;
    QList<RaceCar*> raceCars;

    QBluetoothDeviceDiscoveryAgent *bleAgent;

private slots:
    void startReScan();
    void deviceDiscovered(const QBluetoothDeviceInfo&);
    void deviceScanFinished();
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error);

    void axisAction(int,int,qreal);
    void buttonAction(int,int,bool);
    void reorderCars();

    void controllerDiscovered();

};
#endif // MAINWINDOW_H
