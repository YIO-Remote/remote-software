#ifndef INTERRUPT_HANDLER_H
#define INTERRUPT_HANDLER_H

#include <QObject>
#include <QFile>
#include <QSocketNotifier>

#ifdef __arm__
#include "mcp23017.h"
#endif

class InterruptHandler : public QObject
{
    Q_OBJECT

#ifdef __arm__
    QSocketNotifier *notifier;
    QFile *file;
#endif

public:
    Q_PROPERTY(QString button READ getButton NOTIFY buttonPressed)

    Q_INVOKABLE void shutdown()
    {
#ifdef __arm__
        mcp.shutdown();
#endif
    }

public:

    InterruptHandler()
    {
        setupGPIO();
    }

    ~InterruptHandler(){}

    QString getButton() {
        return m_button;
    }

    void setupGPIO()
    {
#ifdef __arm__
        QFile exportFile("/sys/class/gpio/export");
        exportFile.open(QIODevice::WriteOnly);
        exportFile.write("18");

        delay(200);

        QFile directionFile("/sys/class/gpio/gpio18/direction");
        directionFile.open(QIODevice::WriteOnly);
        directionFile.write("in");

        delay(200);

        QFile edgeFile("/sys/class/gpio/gpio18/edge");
        edgeFile.open(QIODevice::WriteOnly);
        edgeFile.write("falling");

        // GPIO to look at; This is connected to the MCP23017 INTA&INTB ports
        file = new QFile("/sys/class/gpio/gpio18/value");
        file->open(QIODevice::ReadOnly);

        // connect to a signal
        notifier = new QSocketNotifier(file->handle(), QSocketNotifier::Exception);
        notifier->setEnabled(true);
        connect(notifier, &QSocketNotifier::activated, this, &InterruptHandler::interruptHandler);
#endif
    }

    void interruptHandler()
    {
#ifdef __arm__
        QFile file("/sys/class/gpio/gpio18/value");
        file.open(QIODevice::ReadOnly);

        int gpioVal = file.readAll().toInt();

        // if the GPIO is 0, then it's a button press
        if (gpioVal == 0) {

            // check the MCP23017 what caused the interrupt
            m_button = mcp.readInterrupt();

            // tell qml that there was a button press
            emit buttonPressed();
        }

        delay(10);
#endif
    }

private:
    QString m_button;
#ifdef __arm__
    MCP23017 mcp = MCP23017();
#endif

signals:
    void buttonPressed();

};

#endif // INTERRUPT_HANDLER_H
