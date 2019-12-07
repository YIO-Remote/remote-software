#ifndef LIGHT_H
#define LIGHT_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QVariant>
#include <QMetaEnum>
#include "entity.h"
#include "lightinterface.h"

class Light : public Entity, LightInterface
{
    Q_OBJECT
    Q_INTERFACES(LightInterface)

public:
    Q_PROPERTY  (int                brightness  READ    brightness  NOTIFY brightnessChanged)
    Q_PROPERTY  (QColor             color       READ    color       NOTIFY colorChanged)
    Q_PROPERTY  (int                colorTemp   READ    colorTemp   NOTIFY colorTempChanged)

    // light commands
    Q_INVOKABLE void            toggle                  ();
    Q_INVOKABLE void            setBrightness           (int value);
    Q_INVOKABLE void            setColor                (QColor value);
    Q_INVOKABLE void            setColorTemp            (int value);

    bool                        updateAttrByIndex       (int attrIndex, const QVariant& value) override;

    int                         brightness() override   { return m_brightness; }
    QColor                      color() override        { return m_color; }
    int                         colorTemp() override    { return m_colorTemp; }
    bool                        isOn() override         { return m_state == LightDef::ON; }

    explicit Light(const QVariantMap& config, IntegrationInterface* integrationObj, QObject *parent = nullptr);

signals:
    void brightnessChanged();
    void colorChanged();
    void colorTempChanged();

public:
    static QString Type;

private:
    QColor                      m_color;
    int                         m_brightness;
    int                         m_colorTemp;
};

#endif // LIGHT_H
