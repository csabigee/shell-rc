#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "racecar.h"

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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QListWidget* lw;
    QJoysticks* joystickController;
    QGridLayout* lo_main;
    QPushButton* pb_scan;
    QGridLayout* lo_controllers;
    QGridLayout* lo_cars;
    QGroupBox* gb_controllers;
    QGroupBox* gb_cars;
    QList<QLabel*> l_controllers;
    QList<QLabel*> l_cars;
    QList<QProgressBar*> pb_cars;

    QBluetoothDeviceDiscoveryAgent *bleAgent;
    QList<QBluetoothDeviceInfo> bleDeviceList;

    QList<RaceCar*> racecars;
    //QList<QGamepad*> controllers;

private slots:
    void startScan();
    void deviceDiscovered(const QBluetoothDeviceInfo&);
    void deviceScanFinished();
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error);

    void axis_slot(int,int,qreal);
    void button_slot(int,int,bool);

};
#endif // MAINWINDOW_H
