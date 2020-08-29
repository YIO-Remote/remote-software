/******************************************************************************
 *
 * Copyright (C) 2018-2020 Marton Borzak <hello@martonborzak.com>
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

#include "translation.h"

#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(CLASS_LC, "translation");

TranslationHandler *TranslationHandler::s_instance = nullptr;

TranslationHandler::TranslationHandler(QQmlEngine *engine) {
    m_translator = new QTranslator(this);
    m_engine     = engine;

    s_instance = this;
}

TranslationHandler::~TranslationHandler() { s_instance = nullptr; }

void TranslationHandler::selectLanguage(QString language) {
    qCDebug(CLASS_LC) << "Language selected:" << language;

    qGuiApp->removeTranslator(m_translator);
    m_translator->load(":/translations/" + language);
    qGuiApp->installTranslator(m_translator);
    qCDebug(CLASS_LC) << "Installed translation for main app" << language;

    // install plugin translations
    QList<QObject *> plugins = Integrations::getInstance()->getAllPlugins();
    for (QList<QObject *>::const_iterator iter = plugins.cbegin(); iter != plugins.cend(); ++iter) {
        PluginInterface *pInterface = qobject_cast<PluginInterface *>(*iter);
        if (pInterface) {
            qGuiApp->removeTranslator(pInterface->pluginTranslator());
            qGuiApp->installTranslator(pInterface->installTranslator(language));
            qCDebug(CLASS_LC) << "Installed translation for plugin" << language << pInterface;
        }
    }

    emit languageChanged();
}
