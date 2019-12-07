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
    command("CLOSE", "");
}

void Blind::open()
{
    command("OPEN", "");
}

void Blind::stop()
{
    command("STOP", "");
}

void Blind::setPosition(int value)
{
     command("POSITION", value);
}

Blind::Blind(const QVariantMap& config, IntegrationInterface* integrationObj, QObject *parent):
    Entity (Type, config, integrationObj, parent)
{
    static QMetaEnum metaEnumAttr;
    static QMetaEnum metaEnumState;
    if (!metaEnumAttr.isValid()) {
        int index = BlindDef::staticMetaObject.indexOfEnumerator("Attributes");
        metaEnumAttr = BlindDef::staticMetaObject.enumerator(index);
        index = BlindDef::staticMetaObject.indexOfEnumerator("States");
        metaEnumState = BlindDef::staticMetaObject.enumerator(index);
        qmlRegisterUncreatableType<BlindDef>("Entity.Blind", 1, 0, "Blind", "Not creatable as it is an enum type.");
    }
    m_enumAttr = &metaEnumAttr;
    m_enumState = &metaEnumState;
    m_specificInterface = qobject_cast<BlindInterface*>(this);
}


