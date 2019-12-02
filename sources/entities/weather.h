#ifndef WEATHER_H
#define WEATHER_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QMetaEnum>
#include "entity.h"
#include "weatherinterface.h"

class Weather : public Entity
{
    Q_OBJECT

public:
    Q_PROPERTY  (QVariantMap    current     READ    current                     NOTIFY currentChanged)
    Q_PROPERTY  (QVariantList   forecast    READ    forecast                    NOTIFY forecastChanged)

    // update an entity's attributes
    bool                        updateAttrByIndex   (int attrIndex, const QVariant& value) override;

    QVariantMap                 current()               { return m_current; }
    QVariantList                forecast()              { return m_forecast; }
    bool                        isOn() override         { return m_state == WeatherDef::ONLINE; }
    bool                        supportsOn() override   { return false; }

    Weather (const QVariantMap& config, QObject* integrationObj, QObject *parent = nullptr);

signals:
    void currentChanged();
    void forecastChanged();

public:
    static QString Type;
private:
    QVariantMap                 m_current;
    QVariantList                m_forecast;
};


#endif // WEATHER_H
