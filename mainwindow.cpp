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
    setWindowFlags(Qt::FramelessWindowHint); /* No frame */
    setWindowTitle("Shell racing");   /* Set the title of the main window */

    joystickController=QJoysticks::getInstance();

    /* enable virtual controller for debug purposes */
    joystickController->setVirtualJoystickRange(1);
    joystickController->setVirtualJoystickEnabled(true);
    joystickController->setVirtualJoystickAxisSensibility(0.7);

    loMain = new QGridLayout();
    pbExit = new QPushButton();
    pbExit->setIcon(QIcon(QPixmap(":/darkbluestyle/icon_close.png")));
    pbExit->setFixedSize(32,32);
    pbRescan = new QPushButton("Rescan");
    pbRescan->setFixedHeight(32);
    loControllers = new QGridLayout();
    loControllers->setAlignment(Qt::AlignTop);
    loRaceCars = new QGridLayout();
    loRaceCars->setAlignment(Qt::AlignTop);
    loMain->addWidget(pbRescan,0,0,1,1,Qt::AlignLeft);
    loMain->addWidget(pbExit,0,1,1,1,Qt::AlignRight);
    loMain->addLayout(loControllers,1,0);
    loMain->addLayout(loRaceCars,1,1);
    loControllers->setContentsMargins(0,0,0,0);
    loRaceCars->setContentsMargins(0,0,0,0);
    loMain->setContentsMargins(10,10,10,10);
    loMain->setMargin(10);

    connect(pbExit, &QPushButton::released, this, &MainWindow::close);
    connect(pbRescan,&QPushButton::released,this,&MainWindow::startReScan);

    auto central = new QWidget;
    central->setLayout(loMain);

    setCentralWidget(central);

    bleAgent = new QBluetoothDeviceDiscoveryAgent();
    bleAgent->setLowEnergyDiscoveryTimeout(2000);
    connect(bleAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &MainWindow::deviceDiscovered);
    connect(bleAgent, qOverload<QBluetoothDeviceDiscoveryAgent::Error>(&QBluetoothDeviceDiscoveryAgent::error), this, &MainWindow::deviceScanError);
    connect(bleAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &MainWindow::deviceScanFinished);


    /* scan cars */
    bleAgent->start();
    /* scan controllers */
    connect(joystickController, &QJoysticks::countChanged,this, &MainWindow::controllerDiscovered);
}

void MainWindow::startReScan()
{
    /* disconnect everything, destroy every controller and racecar */
    for(uint8_t ii=0; ii<raceCars.length(); ii++){
        loRaceCars->removeWidget(raceCars[ii]);
        delete raceCars[ii];
    }
    raceCars.clear();
    for(uint8_t ii=0; ii<controllers.length(); ii++){
        loControllers->removeWidget(controllers[ii]);
        delete controllers[ii];
    }
    controllers.clear();

    bleAgent->start();
    joystickController->updateInterfaces();


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
            if(controller_name==QString::null){
                controller_name="Generic controller";
            }
            qDebug() << "found new controller: " << controller << controller_name;
            controllers.append(new Controller(controller_name));
            loControllers->addWidget(controllers.last(),controllers.size(),0,Qt::AlignTop);
        }
    }
}

void MainWindow::deviceDiscovered(const QBluetoothDeviceInfo & device)
{
    /* check if bluetooth device is a shell RC racecar */
    if(device.name().startsWith(QStringLiteral("SL-FXX-K Evo"))
            || device.name().startsWith(QStringLiteral("SL-FXX-K Evo"))
            || device.name().startsWith(QStringLiteral("SL-SF1000"))
            || device.name().startsWith(QStringLiteral("SL-488 CHALLENGE Evo"))
            || device.name().startsWith(QStringLiteral("SL-488 GTE"))) {
        qDebug() << "Found new car:" << device.name() << '(' << device.address().toString() << ')';



        /* connect to the device */
        raceCars.append(new RaceCar(device));
        raceCars.last()->connectToDevice();

        connect(raceCars.last(), &RaceCar::place_change, this, &MainWindow::raceCarOrderChange);

        loRaceCars->addWidget(raceCars.last(),raceCars.size(),1,Qt::AlignTop);
        raceCarPositions.append(raceCarPositions.size());
    }
}


void MainWindow::deviceScanFinished()
{
    qDebug() << "ble device scan done";

    connect(joystickController,&QJoysticks::axisChanged, this, &MainWindow::axisAction);
    connect(joystickController,&QJoysticks::buttonChanged, this, &MainWindow::buttonAction);
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
    qDebug() << "axis " << axis << "value" << value;
    if(js<=raceCars.length()){
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
    qDebug() << "js" << js << " button" << button << pressed;
    if(js<=raceCars.length()){
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
            } else{
                raceCars[js]->setSteer(0);
            }
        }
        if(button==3){
            if(pressed){
                raceCars[js]->toggleDirectDrive();
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

void MainWindow::raceCarOrderChange()
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
