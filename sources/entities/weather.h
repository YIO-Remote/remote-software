#pragma once

#include <QMetaEnum>
#include <QObject>
#include <QQmlListProperty>
#include <QString>
#include <QVariant>

#include "entity.h"
#include "yio-interface/entities/weatherinterface.h"

class QWeatherItem : public QObject, public WeatherItem {
    Q_OBJECT
 public:
    explicit QWeatherItem(QObject* parent) : QObject(parent) {}
    Q_PROPERTY(QString date READ date CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(QString imageurl READ imageurl CONSTANT)
    Q_PROPERTY(QString temp READ temp CONSTANT)
    Q_PROPERTY(QString rain READ rain CONSTANT)
    Q_PROPERTY(QString snow READ snow CONSTANT)
    Q_PROPERTY(QString wind READ wind CONSTANT)
    Q_PROPERTY(QString humidity READ humidity CONSTANT)
};

class Weather : public Entity, WeatherInterface {
    Q_OBJECT
    Q_INTERFACES(WeatherInterface)
 public:
    Q_PROPERTY(QWeatherItem* current READ current NOTIFY currentChanged)
    Q_PROPERTY(QObject* forecast READ forecast NOTIFY forecastChanged)

    // update an entity's attributes
    bool updateAttrByIndex(int attrIndex, const QVariant& value) override;

    QWeatherItem* current() { return &m_current; }
    QObject*      forecast() { return m_forecast; }
    bool          isOn() override { return m_state == WeatherDef::ONLINE; }
    bool          supportsOn() override { return false; }

    Weather(const QVariantMap& config, IntegrationInterface* integrationObj, QObject* parent = nullptr);

    void setForecast(QObject* model) override;
    void setCurrent(const WeatherItem& current) override;
 signals:
    void currentChanged();
    void forecastChanged();

 public:
    static QString Type;

 private:
    QWeatherItem m_current;
    QObject*     m_forecast;
};
