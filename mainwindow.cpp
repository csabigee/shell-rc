#include "mainwindow.h"
#include "controller.h"

#include <QDebug>
#include <QThread>
#include <QBluetoothDeviceDiscoveryAgent>
#include "racecar.h"
#include <QWindow>
#include <QGroupBox>
#include <QProgressBar>
#include <QDebug>
#include <QListWidget>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    joystickController=QJoysticks::getInstance();

    /* enable virtual controller for debug purposes */
    joystickController->setVirtualJoystickRange(1);
    joystickController->setVirtualJoystickEnabled(true);
    joystickController->setVirtualJoystickAxisSensibility(0.7);

    lo_main = new QGridLayout();
    pb_scan = new QPushButton("Rescan");
    lo_main->addWidget(pb_scan,0,0,1,2);

    connect(pb_scan,&QPushButton::released,this,&MainWindow::startScan);

    auto central = new QWidget;
    central->setLayout(lo_main);

    setCentralWidget(central);

    bleAgent = new QBluetoothDeviceDiscoveryAgent();
    bleAgent->setLowEnergyDiscoveryTimeout(2000);
    connect(bleAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &MainWindow::deviceDiscovered);
    connect(bleAgent, qOverload<QBluetoothDeviceDiscoveryAgent::Error>(&QBluetoothDeviceDiscoveryAgent::error), this, &MainWindow::deviceScanError);
    connect(bleAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &MainWindow::deviceScanFinished);
}

void MainWindow::startScan()
{
    /* disconnect everything, destroy every controller and racecar */

    /* scan cars */
    bleAgent->start();

    /* scan controllers */
    auto controller_ids = joystickController->deviceNames();
    if (controller_ids.isEmpty()) {
        qDebug() << "Did not find any connected controllers";
        return;
    } else {
        for(auto controller: qAsConst(controller_ids)){
            QString controller_name = controller;
            if(controller_name==QString::null){
                controller_name="Generic controller";
            }
            qDebug() << "found new controller: " << controller << controller_name;
            controllers.append(new Controller(controller_name));
            lo_main->addWidget(controllers.last(),controllers.size(),0);
        }
    }
}

void MainWindow::deviceDiscovered(const QBluetoothDeviceInfo & device)
{
    /* check if bluetooth device is a shell RC racecar */
    if(device.name().startsWith(QStringLiteral("SL-FXX-K Evo"))
            || device.name().startsWith(QStringLiteral("SL-SF1000"))
            || device.name().startsWith(QStringLiteral("SL-488 CHALLENGE Evo"))
            || device.name().startsWith(QStringLiteral("SL-488 GTE"))) {
        qDebug() << "Found new car:" << device.name() << '(' << device.address().toString() << ')';


        /* connect to the device */
        racecars.append(new RaceCar(device));
        racecars.last()->connectToDevice();

        lo_main->addWidget(racecars.last(),racecars.size(),1);
    }
}


void MainWindow::deviceScanFinished()
{
    qDebug() << "ble device scan done";

    connect(joystickController,&QJoysticks::axisChanged, this, &MainWindow::axis_slot);
    connect(joystickController,&QJoysticks::buttonChanged, this, &MainWindow::button_slot);
}

void MainWindow::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    switch (error) {
    case QBluetoothDeviceDiscoveryAgent::NoError:
        break;
    case QBluetoothDeviceDiscoveryAgent::InputOutputError:
        qDebug() << "IO error during discovery";
        break;
    case QBluetoothDeviceDiscoveryAgent::PoweredOffError:
        qDebug() << "Please turn on Bluetooth";
        break;
    case QBluetoothDeviceDiscoveryAgent::InvalidBluetoothAdapterError:
        qDebug() << "Invalid BT adapter";
        break;
    case QBluetoothDeviceDiscoveryAgent::UnsupportedDiscoveryMethod:
        qDebug() << "One of the requested discovery methods is not supported by the current platform.";
        break;
    case QBluetoothDeviceDiscoveryAgent::UnsupportedPlatformError:
        qDebug() << "Platform not supported";
        break;
    case QBluetoothDeviceDiscoveryAgent::UnknownError:
        qDebug() << "Unknown error during discovery";
        break;
    }
}

void MainWindow::axis_slot(int js,int axis,qreal value)
{
    controllers[js]->flashIndicator();
    qDebug() << "axis " << axis << "value" << value;
    if(axis==1){
        racecars[js]->setThrottle(-1*value);
    }
    if(axis==2)
        racecars[js]->setSteer(value);
}

void MainWindow::button_slot(int js,int button, bool pressed)
{
    controllers[js]->flashIndicator();
    qDebug() << "js" << js << " button" << button << pressed;
    if(button==1){
        if(pressed){
            racecars[js]->setSteer(1);
        } else{
            racecars[js]->setSteer(0);
        }
    }
    if(button==2){
        if(pressed){
            racecars[js]->setSteer(-1);
        } else{
            racecars[js]->setSteer(0);
        }
    }
    if(button==3){
        if(pressed){
            racecars[js]->toggleDirectDrive();
        }
    }
    if(button==11){
        racecars[js]->setLamp(pressed);
    }
    if(button==12){
        racecars[js]->setTurbo(pressed);
    }
}

void MainWindow::reconnectControls(const QModelIndexList &indexes)
{
    qDebug() << "racecar indexes changed!";
    for(int i = 0; i < indexes.size(); i++)
        qDebug() << indexes.at(i);
}

MainWindow::~MainWindow()
{
}
