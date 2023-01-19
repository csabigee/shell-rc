#ifndef RACECAR_H
#define RACECAR_H

#include <QObject>
#include <QTimer>
#include <QLowEnergyController>
#include <QElapsedTimer>

class RaceCar : public QObject
{
    Q_OBJECT
public:
    explicit RaceCar(const QBluetoothDeviceInfo &carInfo, QObject *parent = nullptr);
    bool connectToDevice();
    void setThrottle(float throttle);
    void setSteer(float steer);
    void setLamp(bool lamp);
    void setTurbo(bool turbo);
    void toggleDirectDrive();
    uint8_t getBattery();

private:
    QTimer m_bleTimer;
    QLowEnergyController *m_controller = nullptr;
    QElapsedTimer m_connectTimer;
    QLowEnergyService *m_controlService = nullptr;
    QLowEnergyService *m_batteryService = nullptr;
    QLowEnergyCharacteristic m_controlCharacteristics;

    static QBluetoothUuid CONTROL_SERVICE_UUID;
    static QBluetoothUuid BATTERY_SERVICE_UUID;
    static QBluetoothUuid CONTROL_CHARACTERISTICS_UUID;
    static QBluetoothUuid BATTERY_CHARACTERISTICS_UUID;

    bool m_turbo = false;
    bool m_lamp = false;
    bool m_directdrive = false;
    quint8 m_batteryPercentage = 0.0;
    float m_throttle=0.0;
    float m_steering=0.0;
    float pwm_cntr=0.0;

signals:
    void battery_changed(uint8_t percentage);

private slots:
    void send_ctrl();
    void deviceDisconnected();
    void deviceConnected();
    void errorReceived(QLowEnergyController::Error error);
    void serviceDiscovered(const QBluetoothUuid &uuid);
    void serviceScanDone();
    void controlServiceDetailsDiscovered(QLowEnergyService::ServiceState newState);
    void batteryServiceDetailsDiscovered(QLowEnergyService::ServiceState newState);
    void batteryCharacteristicChanged(const QLowEnergyCharacteristic &info, const QByteArray &value);


};

#endif // RACECAR_H