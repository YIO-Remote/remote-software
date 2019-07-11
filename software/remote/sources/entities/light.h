#ifndef LIGHT_H
#define LIGHT_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QVariant>
#include "entity.h"

class Light : public Entity
{
    Q_OBJECT

public:

    Q_PROPERTY  (bool           state       READ    state       NOTIFY stateChanged)
    Q_PROPERTY  (int            brightness  READ    brightness  NOTIFY brightnessChanged)
    Q_PROPERTY  (QColor         color       READ    color       NOTIFY colorChanged)
    Q_PROPERTY  (int            colorTemp   READ    colorTemp   NOTIFY colorTempChanged)

    Q_INVOKABLE bool            update(const QVariantMap& attributes) override;

    Q_INVOKABLE void            toggle();
    Q_INVOKABLE void            turnOn();
    Q_INVOKABLE void            turnOff();
    Q_INVOKABLE void            setBrightness(int value);
    Q_INVOKABLE void            setColor(QColor value);
    Q_INVOKABLE void            setColorTemp(int value);

    explicit Light();
    Light(const QVariantMap& config, QObject* integrationObj);

    bool                        state() const { return m_state; }
    int                         brightness() const { return m_brightness; }
    QColor                      color() const { return m_color; }
    int                         colorTemp() const { return m_colorTemp; }

    void                        init(const QVariantMap& config);

signals:
    void stateChanged();
    void brightnessChanged();
    void colorChanged();
    void colorTempChanged();

public:
    static QString Type;


private:
    bool            m_state;
    QColor          m_color;
    int             m_brightness;
    int             m_colorTemp;
};

#endif // LIGHT_H
