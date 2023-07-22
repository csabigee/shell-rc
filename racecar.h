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
#include <string_view>

constexpr int bleControlIntervalMs = 70;
constexpr int lampFlashingIntervalMs = 250;
static const QStringList validCarsBleId = {"SL-FXX-K Evo","SL-SF1000","SL-488 CHALLENGE Evo","SL-488 GTE","SL-330 P4(1967)","SL-Daytona SP3","SL-296 GTB","SL-F1-75"};

/*!
 * \brief The RaceCar class
 */
class RaceCar : public QWidget
{
    Q_OBJECT
public:
    explicit RaceCar(const QBluetoothDeviceInfo &carInfo, QWidget *parent = nullptr);
    ~RaceCar();
    bool connectToDevice();
    void setThrottle(float throttle);
    void setSteer(float steer);
    void setLamp(bool lamp);
    void setTurbo(bool turbo);
    void toggleDirectDrive();
    uint8_t getBattery();
    bool move_up, move_down;

private:
    QGridLayout *loOutline;
    QGroupBox *gbOutline;
    QGridLayout *loMain;
    QLabel *carIcon;
    QLabel *laName;
    QProgressBar *pbBatery;
    QPushButton *pbFlash;
    QPushButton *pbUp;
    QPushButton *pbDown;

    QTimer bleTimer;
    QLowEnergyController *bleController = nullptr;
    QElapsedTimer connectTimer;
    QLowEnergyService *controlService = nullptr;
    QLowEnergyService *batteryService = nullptr;
    QLowEnergyCharacteristic controlCharacteristics;

    static QBluetoothUuid CONTROL_SERVICE_UUID;
    static QBluetoothUuid BATTERY_SERVICE_UUID;
    static QBluetoothUuid CONTROL_CHARACTERISTICS_UUID;
    static QBluetoothUuid BATTERY_CHARACTERISTICS_UUID;

    QTimer flashTimer;
    uint32_t flashNum;

    bool turbo = false;
    bool lamp = false;
    bool directDrive = false;
    quint8 batteryPercentage = 0.0;
    float throttle=0.0;
    float steering=0.0;
    float pwmCntr=0.0;

signals:
    void batteryChanged(uint8_t percentage);
    void placeChange();

private slots:
    void sendCtrl();
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

};

#endif // RACECAR_H
