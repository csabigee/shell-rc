#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QCommonStyle>
#include <QGroupBox>
#include <QCoreApplication>
#include "racecar.h"

QBluetoothUuid RaceCar::CONTROL_SERVICE_UUID         = QBluetoothUuid(QStringLiteral("0000fff0-0000-1000-8000-00805f9b34fb"));
QBluetoothUuid RaceCar::BATTERY_SERVICE_UUID         = QBluetoothUuid(QStringLiteral("0000180f-0000-1000-8000-00805f9b34fb"));
QBluetoothUuid RaceCar::CONTROL_CHARACTERISTICS_UUID = QBluetoothUuid(QStringLiteral("0000fff1-0000-1000-8000-00805f9b34fb"));
QBluetoothUuid RaceCar::BATTERY_CHARACTERISTICS_UUID = QBluetoothUuid(QStringLiteral("00002a19-0000-1000-8000-00805f9b34fb"));

RaceCar::RaceCar(const QBluetoothDeviceInfo &carInfo, QWidget *parent)
    : QWidget{parent}
{
    this->setFixedHeight(128);
    gbOutline = new QGroupBox;
    loMain = new QGridLayout();
    loOutline = new QGridLayout();
    loMain->setMargin(0);
    loMain->setContentsMargins(8,8,8,8);
    loOutline->setMargin(0);
    loOutline->setContentsMargins(0,0,0,0);

    carIcon = new QLabel("");
    if(carInfo.name().startsWith(QStringLiteral("SL-FXX-K Evo")))
        carIcon->setPixmap(QPixmap(":/cars/FXX-K-EVO.png"));
    else if(carInfo.name().startsWith(QStringLiteral("SL-SF1000")))
        carIcon->setPixmap(QPixmap(":/cars/SF1000-Tuscan-GP-Ferrari-1000.png"));
    else if(carInfo.name().startsWith(QStringLiteral("SL-488 CHALLENGE Evo")))
        carIcon->setPixmap(QPixmap(":/cars/488-challenge-evo.png"));
    else if(carInfo.name().startsWith(QStringLiteral("SL-488 GTE")))
        carIcon->setPixmap(QPixmap(":/cars/SL-488 GTE.png"));
    else if(carInfo.name().startsWith(QStringLiteral("SL-330 P4(1967)")))
        carIcon->setPixmap(QPixmap(":/cars/330-P4.png"));
    else if(carInfo.name().startsWith(QStringLiteral("SL-Daytona SP3")))
        carIcon->setPixmap(QPixmap(":/cars/Daytona SP3.png"));
    else if(carInfo.name().startsWith(QStringLiteral("SL-296 GTB")))
        carIcon->setPixmap(QPixmap(":/cars/296 GTB.png"));
    else if(carInfo.name().startsWith(QStringLiteral("SL-F1-75")))
        carIcon->setPixmap(QPixmap(":/cars/F1-75.png"));
    else
        carIcon->setPixmap(QPixmap(":/cars/FXX-K-EVO.png"));

    laName = new QLabel(carInfo.name());
    pbBatery = new QProgressBar;
    pbFlash = new QPushButton("Flash");
    move_up = move_down=false;
    pbUp = new QPushButton();
    pbDown = new QPushButton();
    pbUp->setIcon(QIcon(QPixmap(":/darkbluestyle/icon_arrow_up.png")));
    pbDown->setIcon(QIcon(QPixmap(":/darkbluestyle/icon_arrow_down.png")));

    loMain->addWidget(carIcon,0,0,3,1);
    loMain->addWidget(laName,0,1,1,1);
    loMain->addWidget(pbBatery,1,1,1,1);
    loMain->addWidget(pbFlash,2,1,1,1);
    loMain->addWidget(pbUp,0,2);
    loMain->addWidget(pbDown,2,2);
    gbOutline->setLayout(loMain);
    loOutline->addWidget(gbOutline);
    this->setLayout(loOutline);


    bleTimer.setSingleShot(false);
    bleTimer.setInterval(bleControlIntervalMs);
    connect(&bleTimer, &QTimer::timeout, this, &RaceCar::sendCtrl);

    bleController = new QLowEnergyController(carInfo);
    bleController->setRemoteAddressType(QLowEnergyController::RandomAddress);

    connect(bleController, &QLowEnergyController::connected, this, &RaceCar::deviceConnected);
    connect(bleController, &QLowEnergyController::disconnected, this, &RaceCar::deviceDisconnected);
    connect(bleController, qOverload<QLowEnergyController::Error>(&QLowEnergyController::error), this, &RaceCar::errorReceived);
    connect(bleController, &QLowEnergyController::serviceDiscovered, this, &RaceCar::serviceDiscovered);
    connect(bleController, &QLowEnergyController::discoveryFinished, this, &RaceCar::serviceScanDone);

    bleController->setRemoteAddressType(QLowEnergyController::PublicAddress);

    flashNum=0;
    flashTimer.setSingleShot(true);
    flashTimer.setInterval(lampFlashingIntervalMs);
    connect(pbFlash, &QPushButton::released, this, &RaceCar::flashLamp);
    connect(&flashTimer, &QTimer::timeout, this, &RaceCar::flashLampTo);

    connect(pbUp, &QPushButton::pressed, this, [this]() {move_up=true; emit placeChange();});
    connect(pbDown, &QPushButton::pressed, this, [this]() {move_down=true; emit placeChange();});
}

bool RaceCar::connectToDevice()
{
    if(batteryService){
        disconnect(batteryService, nullptr, this, nullptr);
        delete batteryService;
        batteryService = nullptr;
    }

    if (controlService) {
        disconnect(controlService, nullptr, this, nullptr);
        delete controlService;
        controlService = nullptr;
    }

    if (bleController->state() == QLowEnergyController::ConnectedState) {
    } else {
        bleController->connectToDevice();
    }
    return true;
}

void RaceCar::setThrottle(float throttle)
{
    this->throttle=throttle;
}

void RaceCar::setSteer(float steer)
{
    this->steering=steer;
}

void RaceCar::setLamp(bool lamp)
{
    this->lamp=lamp;
}

void RaceCar::setTurbo(bool turbo)
{
    this->turbo=turbo;
}

void RaceCar::toggleDirectDrive()
{
    directDrive=!directDrive;
}

void RaceCar::sendCtrl()
{
    QByteArray data = QByteArrayLiteral("\x01\x00\x00\x00\x00\x00\x00\x00");

    float resolution=4;

    if(directDrive){
        pwmCntr=0;
    } else{
        pwmCntr++;
        if(pwmCntr==resolution) {
            pwmCntr=0.0;
        }
    }

    if (turbo)
        data[6] = 1;
    if (lamp)
        data[5] = 1;

    if(throttle>0) {
        if(pwmCntr<=throttle*resolution) {
            data[1] = 1;
        }
    } else if (throttle<0){
        if(pwmCntr<=-1*throttle*resolution) {
            data[2] = 1;
        }
    }

    if(steering>0)
        data[4] = 1;
    else if(steering<0)
        data[3] = 1;

    controlService->writeCharacteristic(controlCharacteristics, data, QLowEnergyService::WriteWithoutResponse);
    QLowEnergyService::ServiceError error = controlService->error();
    if(error!=QLowEnergyService::NoError){
        qDebug() << "Error: " << error;
    }
}

void RaceCar::deviceDisconnected()
{
}

void RaceCar::deviceConnected()
{
    connectTimer.restart();
    bleController->discoverServices();
    qDebug() << "Discovering services";

}

void RaceCar::errorReceived(QLowEnergyController::Error error)
{
    qDebug() << error << connectTimer.elapsed();
    qDebug() << (tr("Discovery error: %1").arg(error));
}

void RaceCar::serviceDiscovered(const QBluetoothUuid &uuid)
{
    qDebug() << "service discovered" << uuid;

    if (uuid == CONTROL_SERVICE_UUID && controlService == nullptr) {
        controlService = bleController->createServiceObject(uuid);
        if (controlService->state() == QLowEnergyService::DiscoveryRequired) {
            connect(controlService, &QLowEnergyService::stateChanged, this, &RaceCar::controlServiceDetailsDiscovered);
            controlService->discoverDetails();
        }
    }

    if (uuid == BATTERY_SERVICE_UUID && batteryService == nullptr) {
        batteryService = bleController->createServiceObject(uuid);
        if (batteryService->state() == QLowEnergyService::DiscoveryRequired) {
            connect(batteryService, &QLowEnergyService::stateChanged, this, &RaceCar::batteryServiceDetailsDiscovered);
            batteryService->discoverDetails();
        }
    }
}

void RaceCar::serviceScanDone()
{
    qDebug() << "Discovering services done, connected";
}

void RaceCar::controlServiceDetailsDiscovered(QLowEnergyService::ServiceState newState)
{
    if (newState != QLowEnergyService::ServiceDiscovered)
        return;

    qDebug() << "Control service discovered" << newState;
    controlCharacteristics = controlService->characteristic(CONTROL_CHARACTERISTICS_UUID);
    if (controlCharacteristics.isValid()) {
        qDebug() << "Connected!";
        bleTimer.start();
    } else {
        qDebug() << "No characteristics " << CONTROL_CHARACTERISTICS_UUID << " have been found in the control service";
    }
}

void RaceCar::batteryServiceDetailsDiscovered(QLowEnergyService::ServiceState newState)
{
    if (newState == QLowEnergyService::ServiceDiscovered) {
        auto batteryCharacteristic = batteryService->characteristic(BATTERY_CHARACTERISTICS_UUID);
        if (batteryCharacteristic.isValid()) {
            QLowEnergyDescriptor notification = batteryCharacteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
            if (notification.isValid()) {
                batteryService->writeDescriptor(notification, QByteArray::fromHex("0100"));
                connect(batteryService, &QLowEnergyService::characteristicChanged,
                        this,  &RaceCar::batteryCharacteristicChanged);
            } else {
                qDebug() << "Battery notification invalid";
                return;
            }
        } else {
            qDebug() << "No battery characteristics found";
        }
    }
}

void RaceCar::batteryCharacteristicChanged(const QLowEnergyCharacteristic &info, const QByteArray &value)
{
    if (info.uuid() == BATTERY_CHARACTERISTICS_UUID) {
        if (value.length() == 1) {
            if (batteryPercentage != value.at(0)) {
                batteryPercentage = value.at(0);
                qDebug() << "Battery: " << batteryPercentage << " %";
                emit batteryChanged(batteryPercentage);
                pbBatery->setValue(batteryPercentage);
            }
        }
    }
}

void RaceCar::flashLamp()
{
    flashNum=4;
    lamp=true;
    flashTimer.start();
}

void RaceCar::flashLampTo()
{
    lamp=!lamp;
    if(flashNum){
        flashTimer.start();
        flashNum--;
    }
}

RaceCar::~RaceCar()
{
    bleTimer.stop();
    flashTimer.stop();
    bleController->disconnectFromDevice();
}

