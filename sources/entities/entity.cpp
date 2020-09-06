/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

#include "entity.h"

#include <QDebug>
#include <QTimer>

#include "../config.h"

EntityInterface::~EntityInterface() {}

Entity::Entity(const QString& type, const QVariantMap& config, IntegrationInterface* integrationObj, QObject* parent)
    : QObject(parent),
      m_integrationObj(integrationObj),
      m_type(type),
      m_area(config.value("area").toString()),
      m_friendly_name(config.value(Config::KEY_FRIENDLYNAME).toString()),
      m_integration(config.value(Config::KEY_INTEGRATION).toString()),
      m_favorite(false),
      m_connected(false),
      m_state(0),
      m_enumState(nullptr),
      m_enumAttr(nullptr),
      m_enumFeatures(nullptr),
      m_enumCommands(nullptr),
      m_specificInterface(nullptr),
      m_custom_features(config.value(Config::KEY_CUSTOM_FEATURES).toStringList()) {
    memset(m_supported_features, 0, sizeof(m_supported_features));

    QString entityId = config.value("entity_id").toString();
    setObjectName(entityId);

    QStringList f = Config::getInstance()->profileFavorites();
    m_favorite = f.contains(entityId);
}

Entity::~Entity() {}

void Entity::command(int command, const QVariant& param) {
    if (m_integrationObj)
        m_integrationObj->sendCommand(m_type, entity_id(), command, param);
}

bool Entity::update(const QVariantMap& attributes) {
    bool chg = false;
    for (QVariantMap::const_iterator iter = attributes.cbegin(); iter != attributes.cend(); ++iter) {
        if (updateAttrByName(iter.key(), iter.value()))
            chg = true;
    }
    return chg;
}
bool Entity::updateAttrByName(const QString& name, const QVariant& value) {
    int attrIndex = getAttrIndex(name);
    return updateAttrByIndex(attrIndex, value);
}
QString Entity::getAttrName(int attrIndex) {
    Q_ASSERT(m_enumAttr != nullptr);
    return m_enumAttr->valueToKey(attrIndex);
}
int Entity::getAttrIndex(const QString& attrName) {
    Q_ASSERT(m_enumAttr != nullptr);
    return m_enumAttr->keyToValue(attrName.toUpper().toUtf8());
}
QString Entity::getFeatureName(int featureIndex) {
    Q_ASSERT(m_enumFeatures != nullptr);
    return QString(m_enumFeatures->valueToKey(featureIndex)).mid(2);
}
int Entity::getFeatureIndex(const QString& featureName) {
    Q_ASSERT(m_enumFeatures != nullptr);
    QString name = featureName;
    return m_enumFeatures->keyToValue(name.prepend("F_").toUpper().toUtf8());
}

QString Entity::getCommandName(int commandIndex) {
    Q_ASSERT(m_enumCommands != nullptr);
    return QString(m_enumCommands->valueToKey(commandIndex)).mid(2);
}
int Entity::getCommandIndex(const QString& commandName) {
    Q_ASSERT(m_enumCommands != nullptr);
    QString name = commandName;
    return m_enumCommands->keyToValue(name.prepend("C_").toUpper().toUtf8());
}

QStringList Entity::allAttributes() {
    Q_ASSERT(m_enumAttr != nullptr);
    QStringList list;
    for (int i = 0; i < m_enumAttr->keyCount(); i++) list.append(m_enumAttr->key(i));
    return list;
}
QStringList Entity::allCommands() {
    Q_ASSERT(m_enumAttr != nullptr);
    QStringList list;
    for (int i = 0; i < m_enumCommands->keyCount(); i++) {
        QString name(m_enumCommands->key(i));
        list.append(name.mid(2));
    }
    return list;
}
bool Entity::isSupported(int feature) {
    int byte = feature / 8;
    int bit  = feature % 8;
    Q_ASSERT(byte < static_cast<int>(sizeof(m_supported_features)));
    return !!(m_supported_features[byte] & (1 << bit));
}

QStringList Entity::customButtons() {
    qDebug() << "m_custom_features size:" << m_custom_features.size();
    return m_custom_features;
}

void Entity::clickCustomButton(int index) {
    qDebug() << "clicking custom button" << index;
    if (m_integrationObj) {
        qDebug() << "sending custom command" << m_type << entity_id() << index;
        m_integrationObj->sendCommand(m_type, entity_id(), index, "custom_command");
    } else {
        qWarning() << "m_integrationObj is false";
    }
}

QStringList Entity::supported_features() {
    QStringList list;
    for (int i = 0; i < MAX_FEATURES; i++) {
        if (isSupported(i))
            list.append(getFeatureName(i));
    }
    return list;
}

QStringList Entity::allFeatures() {
    Q_ASSERT(m_enumAttr != nullptr);
    QStringList list;
    for (int i = 0; i < m_enumFeatures->keyCount(); i++) {
        QString name(m_enumFeatures->key(i));
        list.append(name.mid(2));
    }
    return list;
}
QStringList Entity::allStates() {
    Q_ASSERT(m_enumState != nullptr);
    QStringList list;
    for (int i = 0; i < m_enumState->keyCount(); i++) list.append(m_enumState->key(i));
    return list;
}

bool Entity::setState(int state) {
    if (m_state != state) {
        m_state = state;
        emit stateChanged();
        emit onChanged();
        emit stateTextChanged();
        return true;
    }
    return false;
}
QString Entity::stateText() {
    Q_ASSERT(m_enumState != nullptr);
    return m_enumState->valueToKey(m_state);
}
bool Entity::setStateText(const QString& stateText) {
    Q_ASSERT(m_enumState != nullptr);
    return setState(m_enumState->keyToValue(stateText.toUpper().toUtf8()));
}

bool Entity::updateAttrByIndex(int idx, const QVariant& value) {
    Q_UNUSED(idx)
    Q_UNUSED(value)
    Q_ASSERT(false);  // Must be overriden in specific entity
    return false;
}
bool Entity::isOn() {
    return m_state != 0;  // Otherwise : Override in specific entity
}
bool Entity::supportsOn() {
    return true;  // Otherwise : Override in specific entity
}
void Entity::setConnected(bool value) {
    if (m_connected != value) {
        m_connected = value;
        emit connectedChanged();
    }
}

void Entity::setFavorite(bool value) {
    QTimer::singleShot(1000, this, [=]() {
        // Set Favorite
        Config::getInstance()->setFavorite(entity_id(), value);
        m_favorite = value;
        emit favoriteChanged();
    });
}

void Entity::initializeSupportedFeatures(const QVariantMap& config) {
    QStringList features = config.value("supported_features").toStringList();
    for (int i = 0; i < features.length(); i++) {
        int feature = getFeatureIndex(features[i]);
        if (feature < 0) {
            qWarning() << "not defined feature" << features[i];
            continue;
        }
        int byte = feature / 8;
        int bit  = feature % 8;
        Q_ASSERT(byte < static_cast<int>(sizeof(m_supported_features)));
        m_supported_features[byte] |= (1 << bit);
    }
}
