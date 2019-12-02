#include <QQmlApplicationEngine>
#include "weather.h"

QString     Weather::Type = "weather";

bool Weather::updateAttrByIndex (int attrIndex, const QVariant& value)
{
    bool chg = false;
    switch (attrIndex) {
        case WeatherDef::STATE:
            if (value.type() == QVariant::String)
                chg = setStateText(value.toString());
            else
                chg = setState(value.toInt());
            break;
        case WeatherDef::CURRENT:
            m_current = value.toMap();
            chg = true;
            emit currentChanged();
            break;
        case WeatherDef::FORECAST:
            m_forecast = value.toList();
            chg = true;
            emit forecastChanged();
            break;
    }
    return chg;
}

Weather::Weather(const QVariantMap& config, QObject* integrationObj, QObject *parent):
    Entity (Type, config, integrationObj, parent)
{
    static QMetaEnum metaEnumAttr;
    static QMetaEnum metaEnumState;
    if (!metaEnumAttr.isValid()) {
        int index = WeatherDef::staticMetaObject.indexOfEnumerator("Attributes");
        metaEnumAttr = WeatherDef::staticMetaObject.enumerator(index);
        index = WeatherDef::staticMetaObject.indexOfEnumerator("States");
        metaEnumState = WeatherDef::staticMetaObject.enumerator(index);
        qmlRegisterUncreatableType<WeatherDef>("Entity.Weather", 1, 0, "Weather", "Not creatable as it is an enum type.");
    }
    m_enumAttr = &metaEnumAttr;
    m_enumState = &metaEnumState;
    m_specificInterface = nullptr;
}

