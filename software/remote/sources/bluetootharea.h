#ifndef BLUETOOTHAREA_H
#define BLUETOOTHAREA_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QMap>
#include <QTimer>
#include <QThread>
#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>
#include <QBluetoothDeviceDiscoveryAgent>
#include "qbluetoothlocaldevice.h"

#include <QtDebug>


class BluetoothArea : public QObject
{
    Q_OBJECT

public:
    // the current area
    Q_PROPERTY  (QString currentArea    READ currentArea                            NOTIFY currentAreaChanged)
    Q_PROPERTY  (int     interval       READ interval       WRITE setInterval       NOTIFY intervalChanged)

    QString currentArea ()              { return m_currentArea; }
    int     interval ()                 { return m_timerInterval; }

    void setInterval(int interval)
    {
        m_timerInterval = interval;
        emit intervalChanged();
    }

    // initialise with the config from config.json
    Q_INVOKABLE void init               (const QVariantMap& config);
    Q_INVOKABLE void turnOff            ();

    Q_INVOKABLE void startScan();
    Q_INVOKABLE void stopScan();


    explicit BluetoothArea(QObject *parent = nullptr);
    virtual ~BluetoothArea() {}


signals:
    void currentAreaChanged();
    void intervalChanged();

    void startScanSignal();
    void stopScanSignal();
    void turnOffSignal();

public slots:
    void deviceDiscovered(const QString &);


private:
    QString                             m_currentArea;
    QMap<QString, QString>              m_areas; // "Living room", "xx:xx:xx:xx:xx:"
    int                                 m_timerInterval=5000;

    QThread                             m_thread;

};


class BluetoothThread : public QObject
{
    Q_OBJECT

public:
    bool running = false;

    BluetoothThread(QMap<QString, QString> areas, int interval);

signals:
    void foundRoom(const QString &area);


public slots:
    void startScan();
    void stopScan();
    void turnOff();
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void discoveryFinished();

    void onTimeout();

private:
    QString                             m_currentArea;
    QMap<QString, QString>              m_areas; // "Living room", "xx:xx:xx:xx:xx:"
    QMap<QString, int>                  m_rssi;  // "Living room", -89

    QBluetoothDeviceDiscoveryAgent*     m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    QBluetoothLocalDevice               m_localDevice;
    QString                             m_localDeviceName;
    QBluetoothAddress                   m_localDeviceAddr;

    QTimer*                             m_timer = new QTimer(this);
};

#endif // BLUETOOTHAREA_H
