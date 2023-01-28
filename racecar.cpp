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
    carIcon = new QLabel("");
    if(carInfo.name().startsWith(QStringLiteral("SL-FXX-K Evo")))
        carIcon->setPixmap(QPixmap(":/cars/FXX-K-EVO.png"));
    else if(carInfo.name().startsWith(QStringLiteral("SL-SF1000")))
        carIcon->setPixmap(QPixmap(":/cars/SF1000-Tuscan-GP-Ferrari-1000.png"));
    else if(carInfo.name().startsWith(QStringLiteral("SL-488 CHALLENGE Evo")))
        carIcon->setPixmap(QPixmap(":/cars/488-challenge-evo.png"));
    else if(carInfo.name().startsWith(QStringLiteral("SL-488 GTE")))
        carIcon->setPixmap(QPixmap(":/cars/SL-488 GTE.png"));
    else
        carIcon->setPixmap(QPixmap(":/cars/FXX-K-EVO.png"));


    this->setFixedHeight(128);
    move_up = move_down=false;
    gb_outline = new QGroupBox;
    QCommonStyle style;
    pb_up = new QPushButton();
    pb_down = new QPushButton();
    pb_up->setIcon(style.standardIcon(QStyle::SP_TitleBarShadeButton));
    pb_down->setIcon(style.standardIcon(QStyle::SP_TitleBarUnshadeButton));
    pb_up->setIcon(QIcon(QPixmap(":/darkbluestyle/icon_arrow_up.png")));
    pb_down->setIcon(QIcon(QPixmap(":/darkbluestyle/icon_arrow_down.png")));

    lo_main = new QGridLayout();
    lo_outline = new QGridLayout();

    lo_main->setMargin(0);
    lo_main->setContentsMargins(8,8,8,8);
    lo_outline->setMargin(0);
    lo_outline->setContentsMargins(0,0,0,0);

    l_name = new QLabel(carInfo.name());
    pb_batery = new QProgressBar;
    pb_flash = new QPushButton("Flash");
    lo_main->addWidget(carIcon,0,0,3,1);
    lo_main->addWidget(l_name,0,1,1,1);
    lo_main->addWidget(pb_batery,1,1,1,1);
    lo_main->addWidget(pb_flash,2,1,1,1);
    lo_main->addWidget(pb_up,0,2);
    lo_main->addWidget(pb_down,2,2);
    gb_outline->setLayout(lo_main);
    lo_outline->addWidget(gb_outline);
    this->setLayout(lo_outline);


    m_bleTimer.setSingleShot(false);
    m_bleTimer.setInterval(60);
    connect(&m_bleTimer, &QTimer::timeout, this, &RaceCar::send_ctrl);

    flash_num=0;
    m_flashTimer.setSingleShot(true);
    m_flashTimer.setInterval(350);
    connect(&m_flashTimer, &QTimer::timeout, this, &RaceCar::flashLampTo);


    m_controller = new QLowEnergyController(carInfo);
    m_controller->setRemoteAddressType(QLowEnergyController::RandomAddress);

    connect(m_controller, &QLowEnergyController::connected, this, &RaceCar::deviceConnected);
    connect(m_controller, &QLowEnergyController::disconnected, this, &RaceCar::deviceDisconnected);
    connect(m_controller, qOverload<QLowEnergyController::Error>(&QLowEnergyController::error), this, &RaceCar::errorReceived);
    connect(m_controller, &QLowEnergyController::serviceDiscovered, this, &RaceCar::serviceDiscovered);
    connect(m_controller, &QLowEnergyController::discoveryFinished, this, &RaceCar::serviceScanDone);
    connect(pb_flash, &QPushButton::released, this, &RaceCar::flashLamp);

    m_controller->setRemoteAddressType(QLowEnergyController::PublicAddress);

    connect(pb_up, &QPushButton::pressed, this, [this]() {move_up=true; emit place_change();});
    connect(pb_down, &QPushButton::pressed, this, [this]() {move_down=true; emit place_change();});


}

bool RaceCar::connectToDevice()
{
    disconnect(m_batteryService, nullptr, this, nullptr);
    delete m_batteryService;
    m_batteryService = nullptr;

    if (m_controlService) {
        disconnect(m_controlService, nullptr, this, nullptr);
        delete m_controlService;
        m_controlService = nullptr;
    }

    if (m_controller->state() == QLowEnergyController::ConnectedState) {
        //m_controller->disconnectFromDevice();
        //m_reconnecting = true;
    } else {
        m_controller->connectToDevice();
        //setConnectionState(Connecting);
        //setConnectionStateString(tr("Connecting to device"));
    }
    return true;
}

void RaceCar::setThrottle(float throttle)
{
    m_throttle=throttle;
}

void RaceCar::setSteer(float steer)
{
    m_steering=steer;
}

void RaceCar::setLamp(bool lamp)
{
    m_lamp=lamp;
}

void RaceCar::setTurbo(bool turbo)
{
    m_turbo=turbo;
}

void RaceCar::toggleDirectDrive()
{
    m_directdrive=!m_directdrive;
}

void RaceCar::send_ctrl()
{
    QByteArray data = QByteArrayLiteral("\x01\x00\x00\x00\x00\x00\x00\x00");

    float resolution=4;

    if(m_directdrive){
        pwm_cntr=0;
    } else{
        pwm_cntr++;
        if(pwm_cntr==resolution) {
            pwm_cntr=0.0;
        }
    }

    if (m_turbo) {
        data[6] = 1;
    }

    if (m_lamp) {
        data[5] = 1;
    }

    if(m_throttle>0) {
        if(pwm_cntr<=m_throttle*resolution) {
            data[1] = 1;
        }
    } else if (m_throttle<0){
        if(pwm_cntr<=-1*m_throttle*resolution) {
            data[2] = 1;
        }
    }

    if(m_steering>0) {
        data[4] = 1;
    } else if(m_steering<0) {
        data[3] = 1;
    }

    m_controlService->writeCharacteristic(m_controlCharacteristics, data, QLowEnergyService::WriteWithoutResponse);
    QLowEnergyService::ServiceError error = m_controlService->error();
    if(error!=QLowEnergyService::NoError){
        qDebug() << "Error: " << error;
    }
}

void RaceCar::deviceDisconnected()
{
}

void RaceCar::deviceConnected()
{
    m_connectTimer.restart();
    m_controller->discoverServices();
    qDebug() << "Discovering services";

}

void RaceCar::errorReceived(QLowEnergyController::Error error)
{
    qDebug() << error << m_connectTimer.elapsed();
    qDebug() << (tr("Discovery error: %1").arg(error));
}

void RaceCar::serviceDiscovered(const QBluetoothUuid &uuid)
{
    qDebug() << "service discovered" << uuid;

    if (uuid == CONTROL_SERVICE_UUID && m_controlService == nullptr) {
        m_controlService = m_controller->createServiceObject(uuid);
        if (m_controlService->state() == QLowEnergyService::DiscoveryRequired) {
            connect(m_controlService, &QLowEnergyService::stateChanged, this, &RaceCar::controlServiceDetailsDiscovered);
            m_controlService->discoverDetails();
        }
    }

    if (uuid == BATTERY_SERVICE_UUID && m_batteryService == nullptr) {
        m_batteryService = m_controller->createServiceObject(uuid);
        if (m_batteryService->state() == QLowEnergyService::DiscoveryRequired) {
            connect(m_batteryService, &QLowEnergyService::stateChanged, this, &RaceCar::batteryServiceDetailsDiscovered);
            m_batteryService->discoverDetails();
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
    m_controlCharacteristics = m_controlService->characteristic(CONTROL_CHARACTERISTICS_UUID);
    if (m_controlCharacteristics.isValid()) {
        qDebug() << "Connected!";
        //setConnectionState(Connected);
        m_bleTimer.start();
    } else {
        qDebug() << "No characteristics " << CONTROL_CHARACTERISTICS_UUID << " have been found in the control service";
    }
}

void RaceCar::batteryServiceDetailsDiscovered(QLowEnergyService::ServiceState newState)
{
    if (newState == QLowEnergyService::ServiceDiscovered) {
        auto batteryCharacteristic = m_batteryService->characteristic(BATTERY_CHARACTERISTICS_UUID);
        if (batteryCharacteristic.isValid()) {
            QLowEnergyDescriptor notification = batteryCharacteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
            if (notification.isValid()) {
                m_batteryService->writeDescriptor(notification, QByteArray::fromHex("0100"));
                connect(m_batteryService, &QLowEnergyService::characteristicChanged,
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
            if (m_batteryPercentage != value.at(0)) {
                m_batteryPercentage = value.at(0);
                qDebug() << "Battery: " << m_batteryPercentage << " %";
                emit(battery_changed(m_batteryPercentage));
                pb_batery->setValue(m_batteryPercentage);
            }
        }
    }
}

void RaceCar::flashLamp()
{
    flash_num=4;
    m_lamp=true;
    m_flashTimer.start();
}

void RaceCar::flashLampTo()
{
    m_lamp=!m_lamp;
    if(flash_num){
        m_flashTimer.start();
        flash_num--;
    }
}

RaceCar::~RaceCar()
{
    m_controller->disconnectFromDevice();
}

