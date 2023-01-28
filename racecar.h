#ifndef RACECAR_H
#define RACECAR_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QProgressBar>
#include <QGridLayout>
#include <QPushButton>
#include <QLowEnergyController>
#include <QElapsedTimer>
#include <QGroupBox>

class RaceCar : public QWidget
{
    Q_OBJECT
public:
    explicit RaceCar(const QBluetoothDeviceInfo &carInfo, QWidget *parent = nullptr);
    bool connectToDevice();
    void setThrottle(float throttle);
    void setSteer(float steer);
    void setLamp(bool lamp);
    void setTurbo(bool turbo);
    void toggleDirectDrive();
    uint8_t getBattery();

private:
    QGroupBox* gb_outline;
    QGridLayout* lo_main;
    QGridLayout* lo_outline;
    QLabel* l_name;
    QProgressBar* pb_batery;
    QPushButton* pb_flash;
    QPushButton* pb_up;
    QPushButton* pb_down;
    QTimer m_bleTimer;
    QTimer m_flashTimer;
    uint32_t flash_num;
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
    void place_change(bool up);

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
    void flashLamp();
    void flashLampTo();
    void place_change_pressed();


};

#endif // RACECAR_H
