/******************************************************************************
 *
 * Copyright (C) 2019-2020 Markus Zehnder <business@markuszehnder.ch>
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

#include "systemd.h"

#include <QProcess>

#include "../../logging.h"
#include "../hw_config.h"

// TODO(mze) direct d-bus interaction? https://doc.qt.io/qt-5/qtdbus-index.html
Systemd::Systemd(const QMap<SystemServiceName::Enum, QString> &serviceNameMap, QObject *parent)
    : SystemService(parent),
      m_useSudo(HW_DEF_SYSTEMD_SUDO),
      m_systemctlTimeout(HW_DEF_SYSTEMD_TIMEOUT),
      m_serviceNameMap(serviceNameMap) {}

void Systemd::setUseSudo(bool sudo) {
    m_useSudo = sudo;
}

bool Systemd::isUseSudo() {
    return m_useSudo;
}

bool Systemd::startService(SystemServiceName::Enum serviceName) {
    QString cmd = "systemctl start %1";
    return launch(cmd.arg(m_serviceNameMap.value(serviceName)));
}

bool Systemd::stopService(SystemServiceName::Enum serviceName) {
    QString cmd = "systemctl stop %1";
    return launch(cmd.arg(m_serviceNameMap.value(serviceName)));
}

bool Systemd::restartService(SystemServiceName::Enum serviceName) {
    QString cmd = "systemctl restart %1";
    return launch(cmd.arg(m_serviceNameMap.value(serviceName)));
}

bool Systemd::reloadService(SystemServiceName::Enum serviceName) {
    QString cmd = "systemctl reload %1";
    return launch(cmd.arg(m_serviceNameMap.value(serviceName)));
}

bool Systemd::launch(const QString &command) {
    qCDebug(lcSystemd) << command;

    QProcess process;

    if (m_useSudo) {
        process.start(QString("sudo %1").arg(command));
    } else {
        process.start(command);
    }
    process.waitForFinished(m_systemctlTimeout);
    if (process.exitStatus() == QProcess::ExitStatus::NormalExit && process.exitCode() == 0) {
        return true;
    }

    qCWarning(lcSystemd) << "Failed to execute" << command << ":"
                         << "stdout:" << QString::fromLocal8Bit(process.readAllStandardOutput())
                         << "errout:" << QString::fromLocal8Bit(process.readAllStandardError());
    return false;
}

int Systemd::systemctlTimeout() const {
    return m_systemctlTimeout;
}

void Systemd::setSystemctlTimeout(int systemctlTimeout) {
    m_systemctlTimeout = systemctlTimeout;
}
