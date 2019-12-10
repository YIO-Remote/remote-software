#include <QtDebug>
#include <QQmlApplicationEngine>
#include "blind.h"

BlindInterface::~BlindInterface()
{
}

QString Blind::Type = "blind";

bool Blind::updateAttrByIndex (int attrIndex, const QVariant& value)
{
    bool chg = false;
    switch (attrIndex) {
        case BlindDef::STATE:
            if (value.type() == QVariant::String)
                chg = setStateText(value.toString());
            else
                chg = setState(value.toInt());
            break;
        case BlindDef::POSITION:
            if (m_position != value.toInt()) {
                m_position = value.toInt();
                chg = true;
                emit positionChanged();
            }
            break;
        }
    return chg;
}

void Blind::close()
{
    command(BlindDef::C_CLOSE, "");
}

void Blind::open()
{
    command(BlindDef::C_OPEN, "");
}

void Blind::stop()
{
    command(BlindDef::C_STOP, "");
}

void Blind::setPosition(int value)
{
     command(BlindDef::C_STOP, value);
}

Blind::Blind(const QVariantMap& config, IntegrationInterface* integrationObj, QObject *parent):
    Entity (Type, config, integrationObj, parent),
    m_position(0)
{
    static QMetaEnum metaEnumAttr;
    static QMetaEnum metaEnumFeatures;
    static QMetaEnum metaEnumCommands;
    static QMetaEnum metaEnumState;
    if (!metaEnumAttr.isValid()) {
        int index = BlindDef::staticMetaObject.indexOfEnumerator("Attributes");
        metaEnumAttr = BlindDef::staticMetaObject.enumerator(index);
        index = BlindDef::staticMetaObject.indexOfEnumerator("States");
        metaEnumState = BlindDef::staticMetaObject.enumerator(index);
        index = BlindDef::staticMetaObject.indexOfEnumerator("Features");
        metaEnumFeatures = BlindDef::staticMetaObject.enumerator(index);
        index = BlindDef::staticMetaObject.indexOfEnumerator("Commands");
        metaEnumCommands = BlindDef::staticMetaObject.enumerator(index);
        qmlRegisterUncreatableType<BlindDef>("Entity.Blind", 1, 0, "Blind", "Not creatable as it is an enum type.");
    }
    m_enumAttr = &metaEnumAttr;
    m_enumFeatures = &metaEnumFeatures;
    m_enumCommands = &metaEnumCommands;
    m_enumState = &metaEnumState;
    m_specificInterface = qobject_cast<BlindInterface*>(this);
    initializeSupportedFeatures(config);
}


