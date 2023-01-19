#include "mainwindow.h"

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
    lw = new QListWidget();
//    for(int i = 1; i < 10; ++i)
//        lw->addItem(new QListWidgetItem(QString("Item %1").arg(i)));
    lw->setDragDropMode(QAbstractItemView::InternalMove);

    //lw.show();

    joystickController=QJoysticks::getInstance();
    joystickController->setVirtualJoystickRange(1);
    joystickController->setVirtualJoystickEnabled(true);
    joystickController->setVirtualJoystickAxisSensibility(0.7);
    lo_main = new QGridLayout();
    lo_controllers = new QGridLayout();
    lo_cars = new QGridLayout();
    gb_controllers = new QGroupBox("controllers");
    gb_cars = new QGroupBox("cars");
    pb_scan = new QPushButton("Scan");

    gb_controllers->setLayout(lo_controllers);
    gb_cars->setLayout(lo_cars);
    lo_main->addWidget(pb_scan,1,1,1,2);
    //lo_main->addWidget(gb_controllers,2,1);
    //lo_main->addWidget(gb_cars,2,2);
    lo_main->addWidget(lw,3,1);

    QListWidgetItem *test = new QListWidgetItem;
    lw->setViewMode(QListView::ListMode);
    lw->addItem(test);
    lw->setItemWidget(test,gb_controllers);
    test->setSizeHint(QSize(232,97));


    QListWidgetItem *test2 = new QListWidgetItem;
    lw->setViewMode(QListView::ListMode);
    lw->addItem(test2);
    lw->setItemWidget(test2,gb_cars);
    test2->setSizeHint(QSize(232,97));


    connect(pb_scan,&QPushButton::released,this,&MainWindow::startScan);

    auto central = new QWidget;
    central->setLayout(lo_main);

    setCentralWidget(central);

    bleAgent = new QBluetoothDeviceDiscoveryAgent();
    bleAgent->setLowEnergyDiscoveryTimeout(5000);
    connect(bleAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &MainWindow::deviceDiscovered);
    connect(bleAgent, qOverload<QBluetoothDeviceDiscoveryAgent::Error>(&QBluetoothDeviceDiscoveryAgent::error), this, &MainWindow::deviceScanError);
    connect(bleAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &MainWindow::deviceScanFinished);
}

void MainWindow::startScan()
{
    /* disconnect everything */

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
            l_controllers.append(new QLabel(controller_name));
            lo_controllers->addWidget(l_controllers.back(),l_controllers.size(),1);
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

        bleDeviceList.append(device);
        l_cars.append(new QLabel(device.name() + "(" + device.address().toString() + ")"));
        pb_cars.append(new QProgressBar);
        lo_cars->addWidget(l_cars.back(),l_cars.size(),1);
        lo_cars->addWidget(pb_cars.back(),pb_cars.size(),2);

        /* connect to the device */
        racecars.append(new RaceCar(device, this));
        racecars.last()->connectToDevice();
    }
}


void MainWindow::deviceScanFinished()
{
    qDebug() << "ble device scan done";

    connect(joystickController,&QJoysticks::axisChanged, this, &MainWindow::axis_slot);
    connect(joystickController,&QJoysticks::buttonChanged, this, &MainWindow::button_slot);
    connect(racecars[0],&RaceCar::battery_changed,pb_cars[0],&QProgressBar::setValue);
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
    qDebug() << "axis " << axis << "value" << value;
    if(axis==1){
        racecars[0]->setThrottle(-1*value);
    }
    if(axis==2)
        racecars[0]->setSteer(value);
    }

void MainWindow::button_slot(int js,int button, bool pressed)
{
    qDebug() << "button" << button << pressed;
    if(button==1){
        if(pressed){
            racecars[0]->setSteer(1);
        } else{
            racecars[0]->setSteer(0);
        }
    }
    if(button==2){
        if(pressed){
            racecars[0]->setSteer(-1);
        } else{
            racecars[0]->setSteer(0);
        }
    }
    if(button==3){
        if(pressed){
            racecars[0]->toggleDirectDrive();
        }
    }
    if(button==11){
        racecars[0]->setLamp(pressed);
    }
    if(button==12){
        racecars[0]->setTurbo(pressed);
    }
}

MainWindow::~MainWindow()
{
}
