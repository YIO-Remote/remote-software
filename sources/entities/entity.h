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

#ifndef ENTITY_H
#define ENTITY_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

//#include "../integrations/integrationinterface.h"
#include "entityinterface.h"

class Entity : public QObject, EntityInterface
{
    Q_OBJECT
    Q_INTERFACES(EntityInterface)

public:
    // Creation from config.json content and integration hub
    explicit Entity (const QString& type, const QVariantMap& config, QObject *integrationObj, QObject *parent = nullptr);
    virtual ~Entity ();

    Q_PROPERTY	(QString        type                READ type               CONSTANT)
    Q_PROPERTY	(QString        friendly_name       READ friendly_name      CONSTANT)
    Q_PROPERTY	(QString        entity_id           READ entity_id          CONSTANT)
    Q_PROPERTY	(QString        area                READ area               CONSTANT)
    Q_PROPERTY	(QString        integration         READ integration        CONSTANT)
    Q_PROPERTY	(QObject*       integrationObj      READ integrationObj     CONSTANT)
    Q_PROPERTY	(bool           favorite            READ favorite           WRITE       setFavorite     NOTIFY favoriteChanged)
    Q_PROPERTY	(QStringList   	supported_features  READ supported_features CONSTANT)

    Q_PROPERTY  (int            state               READ state              WRITE       setState        NOTIFY stateChanged)
    Q_PROPERTY	(QString        stateText           READ stateText          WRITE       setStateText    NOTIFY stateTextChanged)
    Q_PROPERTY	(bool           isOn                READ isOn                                           NOTIFY onChanged)
    Q_PROPERTY	(bool           supportsOn          READ supportsOn         CONSTANT)
    Q_PROPERTY  (QStringList    allAttributes       READ allAttributes      CONSTANT)
    Q_PROPERTY  (QStringList    allStates           READ allStates          CONSTANT)

    // send command to the integration
    Q_INVOKABLE	void            command             (const QString &command, const QVariant& param);
    Q_INVOKABLE void            turnOn              ();
    Q_INVOKABLE void            turnOff             ();

    // check for feature
    Q_INVOKABLE bool            isSupported         (const QString& feature);

    // get the data of an entity to save in config.json
    Q_INVOKABLE QVariantMap     getDataToSave       ();

    // update an entity with attributes from integration hub, return true in case of change
    Q_INVOKABLE bool            update              (const QVariantMap& attributes);
    Q_INVOKABLE bool            updateAttrByName    (const QString& name, const QVariant& value);
    Q_INVOKABLE bool            updateAttrByIndex   (int attrIndex, const QVariant& value); // must be overriden

    // Attribute name and index
    Q_INVOKABLE QString         getAttrName         (int attrIndex);
    Q_INVOKABLE int             getAttrIndex        (const QString& attrName);

    // Only for C++ integrations
    void*                       getSpecificInterface()  { return m_specificInterface; }     // light, blind, media player */
    QString                     type()                  { return m_type; }
    QString                     area()                  { return m_area; }
    QString                     friendly_name()         { return m_friendly_name; }
    QString                     entity_id()             { return objectName(); }
    QString                     integration()           { return m_integration; }
    QObject*                    integrationObj()        { return m_integrationObj; }
    QStringList                 supported_features()    { return m_supported_features; }
    bool                        favorite()              { return m_favorite; }
    void                        setFavorite             (bool value);
    int                         state()                 { return m_state; }
    bool                        setState                (int state);                // return true on change
    bool                        isOn                    ();
    bool                        supportsOn              ();
    QString                     stateText               ();
    bool                        setStateText            (const QString& state);     // return true on change
    QStringList                 allAttributes           ();
    QStringList                 allStates               ();

signals:
    void favoriteChanged();
    void stateChanged();
    void onChanged();
    void stateTextChanged();

protected:
    //    IntegrationInterface*   m_integrationIf;
    QObject*                m_integrationObj;
    QString                 m_type;
    QString                 m_area;
    QString                 m_friendly_name;
    QString                 m_integration;
    bool                    m_favorite;
    QStringList             m_supported_features;
    int                     m_state;
    QMetaEnum*              m_enumState;
    QMetaEnum*              m_enumAttr;
    void*                   m_specificInterface;
};

#endif // ENTITY_H
