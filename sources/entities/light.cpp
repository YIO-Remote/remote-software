#include <QtDebug>
#include "light.h"

LightInterface::~LightInterface()
{
}

QMetaEnum   Light::s_metaEnum;
QString     Light::Type = "light";

bool Light::update(const QVariantMap &attributes)
{
    bool chg = false;
    for (QVariantMap::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter) {
        if (updateAttrByName (iter.key(), iter.value()))
            chg = true;
    }
    return chg;
}
bool Light::updateAttrByName (const QString& name, const QVariant& value)
{
    int attrIndex = getAttrIndex(name);
    return updateAttrByIndex (attrIndex, value);
}
bool Light::updateAttrByIndex (int attrIndex, const QVariant& value)
{
    bool chg = false;
    switch (static_cast<LightDef::Attributes>(attrIndex)) {
        case LightDef::Attributes::STATE:
            if (m_state != value.toBool()) {
                m_state = value.toBool();
                chg = true;
                emit stateChanged();
            }
            break;
        case LightDef::Attributes::BRIGHTNESS:
            if (m_brightness != value.toInt()) {
                m_brightness = value.toInt();
                chg = true;
                emit brightnessChanged();
            }
            break;
        case LightDef::Attributes::COLOR:
           if (m_color != value) {
                m_color = QColor(value.toString());
                chg = true;
                emit colorChanged();
            }
            break;
        case LightDef::Attributes::COLORTEMP:
           if (m_colorTemp != value) {
                m_colorTemp = value.toInt();
                chg = true;
                emit colorTempChanged();
            }
            break;
        }
    return chg;
}

void* Light::getSpecificInterface()
{
    return qobject_cast<LightInterface*>(this);
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

Light::Light(const QVariantMap& config, QObject* integrationObj, QObject *parent):
    Entity (Type, config, integrationObj, parent)
{
    if (!s_metaEnum.isValid()) {
        int index = LightDef::staticMetaObject.indexOfEnumerator("Attributes");
        s_metaEnum = LightDef::staticMetaObject.enumerator(index);
    }
}
QStringList Light::allFeatures ()
{
    QStringList list;
    for (int i = 0; i < s_metaEnum.keyCount(); i++)
        list.append(s_metaEnum.key(i));
    return list;
}
QString Light::getAttrName(int attrIndex)
{
    return s_metaEnum.valueToKey(static_cast<int>(attrIndex));
}
int Light::getAttrIndex(const QString& str)
{
    return s_metaEnum.keyToValue(str.toUpper().toUtf8());
}
