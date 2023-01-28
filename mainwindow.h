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

#include <QBluetoothDeviceDiscoveryAgent>
#include <QJoysticks.h>

#include "led.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QJoysticks* joystickController;

    QGridLayout* lo_main;
    QPushButton* pb_scan;

    QList<Controller*> controllers;
    QList<RaceCar*> racecars;

    QBluetoothDeviceDiscoveryAgent *bleAgent;

private slots:
    void startScan();
    void deviceDiscovered(const QBluetoothDeviceInfo&);
    void deviceScanFinished();
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error);

    void axis_slot(int,int,qreal);
    void button_slot(int,int,bool);
    void reconnectControls(const QModelIndexList &indexes);

};
#endif // MAINWINDOW_H
