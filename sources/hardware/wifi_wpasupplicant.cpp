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

WifiWpaSupplicant::WifiWpaSupplicant(QObject *parent)
    : WifiControl(parent)
    , m_ctrl(nullptr)
    , m_process(new QProcess(this))
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/****************************************************************************/
WifiWpaSupplicant::~WifiWpaSupplicant()
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (m_ctrl) {
        m_ctrlNotifier->setEnabled(false);
        m_ctrlNotifier->disconnect();
        wpa_ctrl_detach(m_ctrl);
        wpa_ctrl_close(m_ctrl);
    }
}

bool WifiWpaSupplicant::init()
{
    if (!m_ctrl) {
        // TODO make wpa control socket configurable
        m_wpaSupplicantSocketPath = "/var/run/wpa_supplicant/wlan0";
        try {
            connectWpaControlSocket();
            m_ctrlNotifier = std::make_unique<QSocketNotifier>(
                wpa_ctrl_get_fd(m_ctrl), QSocketNotifier::Read);

            connect(m_ctrlNotifier.get(), SIGNAL(activated(int)), this, SLOT(controlEvent(int)));
            startNetworkScan();
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
    // TODO make configurable
    launch(m_process, "systemctl start wpa_supplicant@wlan0.service");
}

void WifiWpaSupplicant::off()
{
    // TODO make configurable
    // TODO what about TERMINATE command? https://w1.fi/wpa_supplicant/devel/ctrl_iface_page.html
    launch(m_process, "systemctl stop wpa_supplicant@wlan0.service");
}

void WifiWpaSupplicant::reset()
{
    // FIXME implement me
    // use REMOVE_NETWORK & RECONFIGURE commands?
}

void WifiWpaSupplicant::join(const QString &ssid, const QString &password)
{
    // FIXME implement me. Use commands: SET_NETWORK
    // ALso see: https://github.com/loh-tar/wpa-cute/blob/master/src/networkconfig.cpp#L198

    // Does SAVE_CONFIG save the new network into /etc/wpa_supplicant/wpa_supplicant-wlan0.conf? I hope so...
    // Otherwise try: manually save new cfg file & RECONFIGURE command
}

bool WifiWpaSupplicant::isConnected()
{
    // FIXME implement me
    // TODO use STATUS command
    return false;
}

void WifiWpaSupplicant::startNetworkScan()
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    assert(m_ctrl);
    std::lock_guard<std::mutex> lock(m_wpaMutex);
    try {
        requestWrapper("SCAN");
    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << " SCAN:" << exc.what();
    }
}

/****************************************************************************/
void WifiWpaSupplicant::wpsPushButtonConfigurationAuth(const WifiNetwork& network) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    std::lock_guard<std::mutex> lock(m_wpaMutex);
    QString cmd("WPS_PBC ");
    try {
        requestWrapper(cmd + network.bssid());
    } catch (std::exception& exc) {
        qCCritical(CLASS_LC) << exc.what();
    }
}

/****************************************************************************/
void WifiWpaSupplicant::connectWpaControlSocket() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << m_wpaSupplicantSocketPath;
    m_ctrl = wpa_ctrl_open(m_wpaSupplicantSocketPath.toStdString().c_str());
    if (!m_ctrl) {
        throw std::system_error(errno, std::generic_category(), "wpa_ctrl_open() failed");
    }
    if (wpa_ctrl_attach(m_ctrl) < 0) {
        qCCritical(CLASS_LC) << " notifier attach failed!";
    }
}

/****************************************************************************/

void WifiWpaSupplicant::parseEvent(const QString& event) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    if (event.contains(WPA_EVENT_SCAN_RESULTS)) {
        qCDebug(CLASS_LC) << " scan_results available!";
        setScanStatus(ScanOk);
        readScanResults();
        emit networksFound(m_scanResults);
    } else if (event.contains(WPA_EVENT_SCAN_STARTED)) {
        qCDebug(CLASS_LC) << "scan started!";
        setScanStatus(Scanning);
    } else if (event.contains(WPA_EVENT_SCAN_FAILED)) {
        qCDebug(CLASS_LC) << "scan failed!";
        setScanStatus(ScanFailed);
    } else if (event.contains(WPA_EVENT_CONNECTED)) {
        qCDebug(CLASS_LC) << " connected!";
    } else if (event.contains(WPS_EVENT_AP_AVAILABLE_PBC)) {
        qCDebug(CLASS_LC) << " WPS PBC available!";
    } else if (event.contains(WPA_EVENT_DISCONNECTED)) {
        qCDebug(CLASS_LC) << " disconnected!";
    } else if (event.contains(WPS_EVENT_ACTIVE)) {
        qCDebug(CLASS_LC) << " Push button Configuration active!";
    } else {
        qCDebug(CLASS_LC) << " event:" << event;
    }
}

/****************************************************************************/
void WifiWpaSupplicant::controlEvent(int fd) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    char buf[128] = {};
    while (wpa_ctrl_pending(m_ctrl) > 0) {
        auto buf_len = sizeof(buf);
        wpa_ctrl_recv(m_ctrl, buf, &buf_len);
        auto event = QString::fromLocal8Bit(buf, buf_len);
        qCDebug(CLASS_LC) << "[monitor] CTRL:" << event << ":" << buf_len;
        parseEvent(event);
    }
}

/****************************************************************************/
void WifiWpaSupplicant::requestWrapper(const QString& cmd) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << cmd;
    assert(m_ctrl);
    size_t buf_size = sizeof(m_reply);
    auto res = wpa_ctrl_request(
        m_ctrl, cmd.toStdString().c_str(), cmd.size(), m_reply, &buf_size, NULL);
    if (res < 0) {
        throw std::runtime_error("wpa_ctrl_request failed");
    }
}

/****************************************************************************/
void WifiWpaSupplicant::readScanResults() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    assert(m_ctrl);
    std::lock_guard<std::mutex> lock(m_wpaMutex);
    try {
        requestWrapper("SCAN_RESULTS");
        m_scanResults = parseScanresult(m_reply);
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
QList<WifiNetwork> WifiWpaSupplicant::parseScanresult(const char* buffer) {
    QString results(buffer);
    auto lines = results.splitRef("\n");
    QList<WifiNetwork> cont;
    // skip first line which is header of table and the last line which is a empty newline
    for (int i = 1; i < lines.length() - 1; i++) {
        try {
            cont.append(lineToNetwork(lines[i]));
        } catch (std::exception& e) {
            qCCritical(CLASS_LC) << e.what() << lines[i];
        }
    }
    return cont;
}

QDebug operator<<(QDebug debug, const WifiStatus& wifiStatus)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "("
                    << "ssid: " << wifiStatus.name << ", "
                    << "signal: " << wifiStatus.signalStrength  << ", "
                    << "ip: " << wifiStatus.ipAddress  << ", "
                    << "mac: " << wifiStatus.macAddress
                    << ")";
    return debug;
}

WifiStatus WifiWpaSupplicant::parseStatus(const char* buffer) {
    QString results(buffer);
    auto lines = results.splitRef("\n");
    WifiStatus wifiStatus;

    for (int i = 0; i < lines.length(); i++) {
        int pos = lines[i].indexOf("=");
        if (pos > 0) {
            auto key = lines[i].left(pos);
            auto value = lines[i].mid(pos + 1);
            if ("bssid" == key) {
                wifiStatus.bssid = value.toString();
            } else if ("ssid" == key) {
                wifiStatus.name = value.toString();
            } else if ("ip_address" == key) {
                wifiStatus.ipAddress = value.toString();
            } else if ("address" == key) {
                wifiStatus.macAddress = value.toString();
            }
        }
    }

    qCDebug(CLASS_LC) << "Status:" << wifiStatus;
    return wifiStatus;
}

int WifiWpaSupplicant::parseSignalStrength(const char* buffer) {
    QString results(buffer);
    auto lines = results.splitRef("\n");
    int rssi = -100;

    for (int i = 0; i < lines.length(); i++) {
        int pos = lines[i].indexOf("=");
        if (pos > 0) {
            auto key = lines[i].left(pos);
            if ("RSSI" == key) {
                rssi = lines[i].mid(pos + 1).toInt();
                break;
            }
        }
    }

    qCDebug(CLASS_LC) << "Signal strength:" << rssi;
    emit signalStrengthChanged(rssi);

    return rssi;
}

void WifiWpaSupplicant::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    if (m_wifiStatusScanning) {
        std::lock_guard<std::mutex> lock(m_wpaMutex);
        try {
            requestWrapper("STATUS");
            WifiStatus wifiStatus = parseStatus(m_reply);

            if (wifiStatus.name != m_wifiStatus.name) {
                m_wifiStatus.name = wifiStatus.name;
                emit networkNameChanged(wifiStatus.name);
            }

            if (wifiStatus.ipAddress != m_wifiStatus.ipAddress) {
                m_wifiStatus.ipAddress = wifiStatus.ipAddress;
                emit ipAddressChanged(wifiStatus.ipAddress);
            }

            if (wifiStatus.macAddress != m_wifiStatus.macAddress) {
                m_wifiStatus.macAddress = wifiStatus.macAddress;
                emit macAddressChanged(wifiStatus.macAddress);
            }
        } catch (std::exception& exc) {
            qCCritical(CLASS_LC) << exc.what();
        }
    }

    if (m_signalStrengthScanning) {
        std::lock_guard<std::mutex> lock(m_wpaMutex);
        try {
            requestWrapper("SIGNAL_POLL");
            int value = parseSignalStrength(m_reply);
            if (value != m_wifiStatus.signalStrength) {
                m_wifiStatus.signalStrength = value;
                emit signalStrengthChanged(value);
            }
        } catch (std::exception& exc) {
            qCCritical(CLASS_LC) << " SCAN:" << exc.what();
        }
    }
}
