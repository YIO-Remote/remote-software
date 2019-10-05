#include <QtDebug>
#include "light.h"

QString Light::Type = "light";

bool Light::update(const QVariantMap &attributes)
{
    bool chg = false;
    if (attributes.contains("state") && m_state != attributes.value("state").toBool()) {
        m_state = attributes.value("state").toBool();
        chg = true;
        emit stateChanged();
    }
    if (attributes.contains("brightness") && m_brightness != attributes.value("brightness").toInt()) {
        m_brightness = attributes.value("brightness").toInt();
        chg = true;
        emit brightnessChanged();
    }
    if (attributes.contains("color") && m_color != attributes.value("color")) {
        QColor color(attributes.value("color").toString());
        m_color = color;
        chg = true;
        emit colorChanged();
    }
    return chg;
}

void Light::toggle()
{
//    command("TOGGLE", "");
    if (state()) {
        command("OFF", "");
    }
    else {
        command("ON", "");
    }
}

void Light::turnOn()
{
    command("ON", "");
}

void Light::turnOff()
{
    command("OFF", "");
}

void Light::setBrightness(int value)
{
    command("BRIGHTNESS", value);
}

void Light::setColor(QColor value)
{
    command("COLOR", QVariant(value));
}

void Light::setColorTemp(int value)
{
    command("COLORTEMP", value);
}

Light::Light() :
    Entity (Type, QVariantMap(), NULL)
{

}

Light::Light(const QVariantMap& config, QObject* integrationObj):
    Entity (Type, config, integrationObj)
{
}
