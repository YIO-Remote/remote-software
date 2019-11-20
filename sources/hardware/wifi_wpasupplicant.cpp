/******************************************************************************
 *
 * Copyright (C) 2019 Markus Zehnder <business@markuszehnder.ch>
 * Copyright (C) 2018 Thomas Ruschival <thomas@ruschival.de>
 *                    wpa_supplicant integration code based on
 *                    https://github.com/truschival/DigitalRoosterGui
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

#include <QLoggingCategory>
#include <QVector>
#include <cerrno>
#include <exception>

#include "wifi_wpasupplicant.h"
#include "wpa_ctrl.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "WifiWpaSupplicant");

WifiWpaSupplicant::WifiWpaSupplicant(QObject *parent) : WifiControl(parent)
  , ctrl(nullptr)
      , reply_size(sizeof(reply)) {
      qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/****************************************************************************/
WifiWpaSupplicant::~WifiWpaSupplicant() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (ctrl) {
        ctrl_notifier->setEnabled(false);
        ctrl_notifier->disconnect();
        wpa_ctrl_detach(ctrl);
        wpa_ctrl_close(ctrl);
    }
}

bool WifiWpaSupplicant::init()
{
    if (!ctrl) {
        // TODO make wpa control socket configurable
        wpa_supplicant_sock_path = "/var/run/wpa_supplicant/wlan0";
        try {
            connectWpaControlSocket();
            ctrl_notifier = std::make_unique<QSocketNotifier>(
                wpa_ctrl_get_fd(ctrl), QSocketNotifier::Read);

            connect(ctrl_notifier.get(), SIGNAL(activated(int)), this, SLOT(controlEvent(int)));
        } catch (std::system_error& exc) {
            qCCritical(CLASS_LC) << exc.what();
            return false;
        } catch (std::exception& exc) {
            qCCritical(CLASS_LC) << exc.what();
            return false;
        }
    }
    return true;
}

void WifiWpaSupplicant::on()
{
    // FIXME implement me
}

void WifiWpaSupplicant::off()
{
    // FIXME implement me
}

void WifiWpaSupplicant::reset()
{
    // FIXME implement me
}

void WifiWpaSupplicant::join(const QString &ssid, const QString &password)
{
    // FIXME implement me
}

bool WifiWpaSupplicant::isConnected()
{
    // FIXME implement me
    return false;
}

QString WifiWpaSupplicant::getMacAddress()
{
    // FIXME implement me
    return "de:ad:be:ef:00:00";
}

QString WifiWpaSupplicant::getCurrentSSID()
{
    // FIXME implement me
    return "Not Implemented";
}

int WifiWpaSupplicant::getCurrentSignalStrength()
{
    // FIXME implement me
    return 0;
}

QString WifiWpaSupplicant::getCurrentIp()
{
    // FIXME implement me
    return "127.0.0.1";
}

void WifiWpaSupplicant::startNetworkScan()
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    assert(ctrl);
    std::lock_guard<std::mutex> lock(wpa_mtx);
    try {
        requestWrapper("SCAN");
    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << " SCAN:" << exc.what();
    }
}

/****************************************************************************/
void WifiWpaSupplicant::wps_pbc_auth(const WifiNetwork& network) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    std::lock_guard<std::mutex> lock(wpa_mtx);
    QString cmd("WPS_PBC ");
    try {
        requestWrapper(cmd + network.bssid);
    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << exc.what();
    }
}

/****************************************************************************/
void WifiWpaSupplicant::connectWpaControlSocket() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << wpa_supplicant_sock_path;
    ctrl = wpa_ctrl_open(wpa_supplicant_sock_path.toStdString().c_str());
    if (!ctrl) {
        throw std::system_error(
            errno, std::generic_category(), "wpa_ctrl_open() failed");
    }
    if (wpa_ctrl_attach(ctrl) < 0) {
        qCCritical(CLASS_LC) << " notifier attach failed!";
    }
}

/****************************************************************************/

void WifiWpaSupplicant::parseEvent(const QString& e_string) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    /**
     * Did we get a scan_results event?
     */
    if (e_string.contains(WPA_EVENT_SCAN_RESULTS)) {
        qCDebug(CLASS_LC) << " scan_results available!";
        setScanStatus(ScanOk);
        readScanResults();
        emit networksFound(m_scanResults);
    }
    if (e_string.contains(WPA_EVENT_SCAN_STARTED)) {
        qCDebug(CLASS_LC) << "scan started!";
        setScanStatus(Scanning);
    }
    if (e_string.contains(WPA_EVENT_SCAN_FAILED)) {
        qCDebug(CLASS_LC) << "scan failed!";
        setScanStatus(ScanFailed);
    }
    if (e_string.contains(WPA_EVENT_CONNECTED)) {
        qCDebug(CLASS_LC) << " connected!";
    }
    if (e_string.contains(WPS_EVENT_AP_AVAILABLE_PBC)) {
        qCDebug(CLASS_LC) << " WPS PBC available!";
    }
    if (e_string.contains(WPA_EVENT_DISCONNECTED)) {
        qCDebug(CLASS_LC) << " disconnected!";
    }
    if (e_string.contains(WPS_EVENT_ACTIVE)) {
        qCDebug(CLASS_LC) << " Push button Configuration active!";
    }
}

/****************************************************************************/
void WifiWpaSupplicant::controlEvent(int fd) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    char buf[128] = {};
    while (wpa_ctrl_pending(ctrl) > 0) {
        auto buf_len = sizeof(buf);
        wpa_ctrl_recv(ctrl, buf, &buf_len);
        auto e_string = QString::fromLocal8Bit(buf, buf_len);
        qCDebug(CLASS_LC) << "[monitor] CTRL:" << e_string << ":" << buf_len;
        parseEvent(e_string);
    }
}

/****************************************************************************/
void WifiWpaSupplicant::requestWrapper(const QString& cmd) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << cmd;
    assert(ctrl);
    size_t buf_size = sizeof(reply);
    auto res = wpa_ctrl_request(
        ctrl, cmd.toStdString().c_str(), cmd.size(), reply, &buf_size, NULL);
    if (res < 0) {
        throw std::runtime_error("wpa_ctrl_request failed");
    }
}

/****************************************************************************/
void WifiWpaSupplicant::readScanResults() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    assert(ctrl);
    std::lock_guard<std::mutex> lock(wpa_mtx);
    try {
        requestWrapper("SCAN_RESULTS");
        m_scanResults = parseScanresult(reply, reply_size);
    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << exc.what();
    }
}

/****************************************************************************/
WifiNetwork WifiWpaSupplicant::lineToNetwork(const QStringRef& line) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QVector<QStringRef> list = line.split("\t");

    if (list.size() > 3) {
        auto name = list.at(4).toString();
        auto bssid = list.at(0).toString();
        auto signal = list.at(2).toInt();
        qCDebug(CLASS_LC) << "network:" << name << bssid << signal;
        WifiNetwork nw{name, bssid, signal, false, false};
        return nw;
    } else {
        throw std::runtime_error("parse error");
    }
}

/****************************************************************************/
QList<WifiNetwork> WifiWpaSupplicant::parseScanresult(const char* buffer, size_t len) {
    QString results(buffer);
    auto lines = results.splitRef("\n");
    QList<WifiNetwork> cont;
    /*
     * skip first line which is header of table and the last
     * line which is a empty newline
     */
    for (int i = 1; i < lines.length() - 1; i++) {
        try {
            cont.push_back(lineToNetwork(lines[i]));
        } catch (std::exception& e) {
            qCCritical(CLASS_LC) << e.what() << lines[i];
        }
    }
    return cont;
}
