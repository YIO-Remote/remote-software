#include <QtDebug>
#include "blind.h"

BlindInterface::~BlindInterface()
{
}

QMetaEnum   Blind::s_metaEnum;
QString     Blind::Type = "blind";

bool Blind::update(const QVariantMap &attributes)
{
    bool chg = false;
    for (QVariantMap::const_iterator iter = attributes.begin(); iter != attributes.end(); ++iter) {
        if (updateAttrByName (iter.key(), iter.value()))
            chg = true;
    }
    return chg;
}
bool Blind::updateAttrByName (const QString& name, const QVariant& value)
{
    int attrIndex = getAttrIndex(name);
    return updateAttrByIndex (attrIndex, value);
}
bool Blind::updateAttrByIndex (int attrIndex, const QVariant& value)
{
    bool chg = false;
    switch (static_cast<BlindDef::Attributes>(attrIndex)) {
        case BlindDef::Attributes::STATE:
            if (m_state != value.toBool()) {
                m_state = value.toBool();
                chg = true;
                emit stateChanged();
            }
            break;
        case BlindDef::Attributes::POSITION:
            if (m_position != value.toInt()) {
                m_position = value.toInt();
                chg = true;
                emit positionChanged();
            }
            break;
        }
    return chg;
}

void* Blind::getSpecificInterface()
{
    return qobject_cast<BlindInterface*>(this);
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

Blind::Blind(const QVariantMap& config, QObject* integrationObj, QObject *parent):
    Entity (Type, config, integrationObj, parent)
{
    if (!s_metaEnum.isValid()) {
        int index = BlindDef::staticMetaObject.indexOfEnumerator("Attributes");
        s_metaEnum = BlindDef::staticMetaObject.enumerator(index);
    }
}

QStringList Blind::allAttributes ()
{
    QStringList list;
    for (int i = 0; i < s_metaEnum.keyCount(); i++)
        list.append(s_metaEnum.key(i));
    return list;
}
QString Blind::getAttrName(int attrIndex)
{
    return s_metaEnum.valueToKey(static_cast<int>(attrIndex));
}
int Blind::getAttrIndex(const QString& str)
{
    return s_metaEnum.keyToValue(str.toUpper().toUtf8());
}


