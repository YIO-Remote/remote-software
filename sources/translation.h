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

#pragma once

#include <QGuiApplication>
#include <QObject>
#include <QQmlEngine>
#include <QTranslator>

#include "integrations/integrations.h"
#include "logger.h"
#include "yio-interface/plugininterface.h"

static Q_LOGGING_CATEGORY(m_log, "translation");

class TranslationHandler : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString emptyString READ getEmptyString NOTIFY languageChanged)

 public:
    explicit TranslationHandler(QQmlEngine *engine) {
        m_translator = new QTranslator(this);
        m_engine     = engine;
    }

    QString getEmptyString() { return ""; }

    Q_INVOKABLE void selectLanguage(QString language) {
        qCDebug(m_log) << "Language selected:" << language;

        qGuiApp->removeTranslator(m_translator);
        m_translator->load(":/translations/" + language);
        qGuiApp->installTranslator(m_translator);
        qCDebug(m_log) << "Installed transslation for main app" << language;

        // install plugin translations
        QList<QObject *> plugins = Integrations::getInstance()->getAllPlugins();
        for (QList<QObject *>::const_iterator iter = plugins.begin(); iter != plugins.end(); ++iter) {
            PluginInterface *pInterface = qobject_cast<PluginInterface *>(*iter);
            if (pInterface) {
                qGuiApp->removeTranslator(pInterface->pluginTranslator());
                qGuiApp->installTranslator(pInterface->installTranslator(language));
                qCDebug(m_log) << "Installed transslation for plugin" << language << pInterface;
            }
        }

        emit languageChanged();
    }

 signals:
    void languageChanged();

 private:
    QTranslator *m_translator;
    QQmlEngine * m_engine;
};
