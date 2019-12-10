#include <QtDebug>
#include <QQmlApplicationEngine>
#include "light.h"

LightInterface::~LightInterface()
{
}

QString     Light::Type = "light";

bool Light::updateAttrByIndex (int attrIndex, const QVariant& value)
{
    bool chg = false;
    switch (attrIndex) {
        case LightDef::STATE:
            if (value.type() == QVariant::String)
                chg = setStateText(value.toString());
            else
                chg = setState(value.toInt());
            break;
        case LightDef::BRIGHTNESS:
            if (m_brightness != value.toInt()) {
                m_brightness = value.toInt();
                chg = true;
                emit brightnessChanged();
            }
            break;
        case LightDef::COLOR:
           if (m_color != value) {
                m_color = QColor(value.toString());
                chg = true;
                emit colorChanged();
            }
            break;
        case LightDef::COLORTEMP:
           if (m_colorTemp != value) {
                m_colorTemp = value.toInt();
                chg = true;
                emit colorTempChanged();
            }
            break;
        }
    return chg;
}

void Light::turnOn ()
{
    command(LightDef::C_ON, "");
}
void Light::turnOff ()
{
    command(LightDef::C_OFF, "");
}
void Light::toggle()
{
    if (state() == LightDef::ON) {
        turnOff();
    }
    else {
        turnOn();
    }
}

void Light::setBrightness(int value)
{
    command(LightDef::C_BRIGHTNESS, value);
}

void Light::setColor(QColor value)
{
    command(LightDef::C_COLOR, QVariant(value));
}

void Light::setColorTemp(int value)
{
    command(LightDef::C_COLORTEMP, value);
}

Light::Light(const QVariantMap& config, IntegrationInterface* integrationObj, QObject *parent):
    Entity (Type, config, integrationObj, parent),
    m_brightness(0),
    m_colorTemp(0)
{
    static QMetaEnum metaEnumAttr;
    static QMetaEnum metaEnumFeatures;
    static QMetaEnum metaEnumCommands;
    static QMetaEnum metaEnumState;
    if (!metaEnumAttr.isValid()) {
        int index = LightDef::staticMetaObject.indexOfEnumerator("Attributes");
        metaEnumAttr = LightDef::staticMetaObject.enumerator(index);
        index = LightDef::staticMetaObject.indexOfEnumerator("States");
        metaEnumState = LightDef::staticMetaObject.enumerator(index);
        index = LightDef::staticMetaObject.indexOfEnumerator("Features");
        metaEnumFeatures = LightDef::staticMetaObject.enumerator(index);
        index = LightDef::staticMetaObject.indexOfEnumerator("Commands");
        metaEnumCommands = LightDef::staticMetaObject.enumerator(index);
        qmlRegisterUncreatableType<LightDef>("Entity.Light", 1, 0, "Light", "Not creatable as it is an enum type.");
    }
    m_enumAttr = &metaEnumAttr;
    m_enumFeatures = &metaEnumFeatures;
    m_enumCommands = &metaEnumCommands;
    m_enumState = &metaEnumState;
    m_specificInterface = qobject_cast<LightInterface*>(this);
    initializeSupportedFeatures(config);
}
