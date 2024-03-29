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
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowTitle("Shell racing");

    /* enable virtual controller for debug purposes */
    joystickController=QJoysticks::getInstance();
    joystickController->setVirtualJoystickRange(1);
    joystickController->setVirtualJoystickEnabled(true);
    joystickController->setVirtualJoystickAxisSensibility(0.7);

    loMain = new QGridLayout();
    loMain->setContentsMargins(10,10,10,10);
    loControllers = new QGridLayout();
    loControllers->setAlignment(Qt::AlignTop);
    loControllers->setContentsMargins(0,0,0,0);
    loRaceCars = new QGridLayout();
    loRaceCars->setAlignment(Qt::AlignTop);
    loRaceCars->setContentsMargins(0,0,0,0);



    pbRescan = new QPushButton("Rescan");
    pbRescan->setFixedHeight(32);
    pbExit = new QPushButton();
    pbExit->setIcon(QIcon(QPixmap(":/darkbluestyle/icon_close.png")));
    pbExit->setFixedSize(32,32);
    loMain->addWidget(pbRescan,0,0,1,1,Qt::AlignLeft);
    loMain->addWidget(pbExit,0,1,1,1,Qt::AlignRight);
    loMain->addLayout(loControllers,1,0,Qt::AlignTop);
    loMain->addLayout(loRaceCars,1,1,Qt::AlignTop);

    connect(pbExit, &QPushButton::released, this, &MainWindow::close);
    connect(pbRescan,&QPushButton::released,this,&MainWindow::startReScan);

    auto central = new QWidget;
    central->setLayout(loMain);
    setCentralWidget(central);

    bleAgent = new QBluetoothDeviceDiscoveryAgent();
    bleAgent->setLowEnergyDiscoveryTimeout(30000);
    connect(bleAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &MainWindow::deviceDiscovered);
//    connect(bleAgent, qOverload<QBluetoothDeviceDiscoveryAgent::Error>(&QBluetoothDeviceDiscoveryAgent::error), this, &MainWindow::deviceScanError);
    connect(bleAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred, this, &MainWindow::deviceScanError);
    connect(bleAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &MainWindow::deviceScanFinished);

    /* create default controllers */

    /* scan controllers */
    connect(joystickController, &QJoysticks::countChanged,this, &MainWindow::controllerDiscovered);

    /* scan cars */
    pbRescan->setEnabled(false);
    bleAgent->start();

    connect(joystickController,&QJoysticks::axisChanged, this, &MainWindow::axisAction);
    connect(joystickController,&QJoysticks::buttonChanged, this, &MainWindow::buttonAction);

}

void MainWindow::startReScan()
{
    pbRescan->setEnabled(false);

    disconnect(joystickController,&QJoysticks::axisChanged, this, &MainWindow::axisAction);
    disconnect(joystickController,&QJoysticks::buttonChanged, this, &MainWindow::buttonAction);

    /* disconnect everything, destroy every controller and racecar */
    for(uint8_t ii=0; ii<raceCars.length(); ii++){
        raceCars[ii]->setVisible(false);
        disconnect(raceCars[ii], &RaceCar::placeChange, this, &MainWindow::reorderCars);
        loRaceCars->removeWidget(raceCars[ii]);
        delete raceCars[ii];
    }
    raceCars.clear();
    for(uint8_t ii=0; ii<controllers.length(); ii++){
        controllers[ii]->setVisible(false);
        loControllers->removeWidget(controllers[ii]);
        delete controllers[ii];
    }
    controllers.clear();

    bleAgent->start();
    joystickController->updateInterfaces();

    connect(joystickController,&QJoysticks::axisChanged, this, &MainWindow::axisAction);
    connect(joystickController,&QJoysticks::buttonChanged, this, &MainWindow::buttonAction);
}

void MainWindow::controllerDiscovered(){
    auto controller_ids = joystickController->deviceNames();
    controller_ids = joystickController->deviceNames();
    if (controller_ids.isEmpty()) {
        qDebug() << "Did not find any connected controllers";
        return;
    } else {
        for(const auto &controller: qAsConst(controller_ids)){
            QString controller_name = controller;
            if(controller_name==QString()){
                controller_name="Generic controller";
            }
            qDebug() << "found new controller: " << controller_name;
            controllers.append(new Controller(controller_name,this));
            loControllers->addWidget(controllers.last(),controllers.size(),0,Qt::AlignTop);
        }
    }
}

void MainWindow::deviceDiscovered(const QBluetoothDeviceInfo & device)
{
    for ( const auto& ii : validCarsBleId  ){
        if(device.name().contains(ii)){
            qDebug() << "Found new car:" << device.name() << '(' << device.address().toString() << ')';
            /* connect to the device */
            raceCars.append(new RaceCar(device,this));
            raceCars.last()->connectToDevice();
            connect(raceCars.last(), &RaceCar::placeChange, this, &MainWindow::reorderCars);
            loRaceCars->addWidget(raceCars.last(),raceCars.size(),1,Qt::AlignTop);
        }
    }
}

void MainWindow::deviceScanFinished()
{
    qDebug() << "ble device scan done";
    pbRescan->setEnabled(true);

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

void MainWindow::axisAction(int js,int axis,qreal value)
{
    controllers[js]->flashIndicator();
    //qDebug() << "axis " << axis << "value" << value;
    if(js<raceCars.length()){
        if(axis==1){
            raceCars[js]->setThrottle(-1*value);
        }
        if(axis==2)
            raceCars[js]->setSteer(value);
    }
}

void MainWindow::buttonAction(int js,int button, bool pressed)
{
    controllers[js]->flashIndicator();
    //qDebug() << "js" << js << " button" << button << pressed;
    if(js<raceCars.length()){
        if(button==1){
            if(pressed){
                raceCars[js]->setSteer(1);
            } else{
                raceCars[js]->setSteer(0);
            }
        }
        if(button==2){
            if(pressed){
                raceCars[js]->setSteer(-1);
            } else {
                raceCars[js]->setSteer(0);
            }
        }
        if(button==3){
            if(pressed){
                raceCars[js]->toggleDirectDrive();
            }
        }
        if(button==4){
            if(pressed){
                raceCars[js]->setThrottle(1);
            } else {
                raceCars[js]->setThrottle(0);
            }
        }
        if(button==5){
            if(pressed){
                raceCars[js]->setThrottle(-1);
            } else {
                raceCars[js]->setThrottle(0);
            }

        }
        if(button==11){
            raceCars[js]->setLamp(pressed);
        }
        if(button==12){
            raceCars[js]->setTurbo(pressed);
        }
    }
}

void MainWindow::reorderCars()
{
    if(raceCars.length()>1){
        for(uint8_t ii=1; ii< raceCars.length(); ii++){
            if(raceCars[ii]->move_up){
                raceCars[ii]->move_up=false;
                raceCars[ii]->move_down=false;
                raceCars.swapItemsAt(ii-1,ii);
            }
        }
        for(uint8_t ii=0; ii< raceCars.length()-1; ii++){
            if(raceCars[ii]->move_down){
                raceCars[ii]->move_up=false;
                raceCars[ii]->move_down=false;
                raceCars.swapItemsAt(ii,ii+1);
            }
        }

        /* remove widgets from layout */
        for(uint8_t ii=0; ii<raceCars.length(); ii++)
            loRaceCars->removeWidget(raceCars[ii]);

        /* add widgets on the new order */
        for(uint8_t ii=0; ii<raceCars.length(); ii++)
            loRaceCars->addWidget(raceCars[ii],ii,1,Qt::AlignTop);
    }
}

/**
  * @brief This function is called when a mouse button is pressed
  */
void MainWindow::mousePressEvent(QMouseEvent *event){
    mpos = event->pos();
}

/**
  * @brief This function is called when the mouse is moving, and sets the window position if the left button is pressed
  */
void MainWindow::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::RightButton) {
        QPoint diff = event->pos() - mpos;
        QPoint newpos = this->pos() + diff;

        this->move(newpos);
    }
}

MainWindow::~MainWindow()
{
}
