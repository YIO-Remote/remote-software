/******************************************************************************
 *
 * Copyright (C) 2019 Markus Zehnder <business@markuszehnder.ch>
 *
 * Third party work used:
 *
 * DigitalRooster - QT/QML internet radio, podcast player and alarmclock.
 * Copyright (C) 2018 Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GPL 3.0 or later.
 *
 * wpaCute - A graphical wpa_supplicant front end.
 * Copyright (C) 2018 loh.tar@googlemail.com
 * Licensed under BSD license.
 *
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

#include <cstdio>
#include <cerrno>
#include <exception>

#include "wifi_wpasupplicant.h"
#include "wpa_ctrl.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "WpaCtrl");

const size_t WPA_BUF_SIZE = 2048;

WifiWpaSupplicant::WifiWpaSupplicant(QObject *parent)
    : WifiControl(parent)
    , m_ctrl(nullptr)
    , m_scriptProcess(new QProcess(this))
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

/****************************************************************************/
WifiWpaSupplicant::~WifiWpaSupplicant()
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    if (m_ctrl) {
        if (m_ctrlNotifier) {
            m_ctrlNotifier->setEnabled(false);
            m_ctrlNotifier->disconnect();
        }
        wpa_ctrl_detach(m_ctrl);
        wpa_ctrl_close(m_ctrl);
    }
}

QDebug operator<<(QDebug debug, const WifiNetwork& wn)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "("
                    << "id: " << wn.id() << ", "
                    << "ssid: " << wn.name() << ", "
                    << "bssid: " << wn.bssid() << ", "
                    << "signal: " << wn.rssi() << ", "
                    << "signalStrength: " << wn.signalStrength() << ", "
                    << "encrypted: " << wn.isEncrypted() << ", "
                    << "security: " << wn.security() << ", "
                    << "wpsAvailable: " << wn.isWpsAvailable() << ", "
                    << "connected: " << wn.isConnected()
                    << ")";
    return debug;
}

QDebug operator<<(QDebug debug, const WifiStatus& wifiStatus)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "("
                    << "ssid: " << wifiStatus.name << ", "
                    << "ip: " << wifiStatus.ipAddress  << ", "
                    << "mac: " << wifiStatus.macAddress
                    << ")";
    return debug;
}

bool WifiWpaSupplicant::init()
{
    if (!m_ctrl) {
        qCDebug(CLASS_LC) << "initializing driver...";

        // TODO use a configuration object, pass as argument to init() and read m_wifiOnScript, m_wifiOffScript, wpaSupplicantSocketPath from configuration
        m_wifiOnScript = "systemctl start wpa_supplicant@wlan0.service";
        m_wifiOffScript = "systemctl stop wpa_supplicant@wlan0.service";
        QString wpaSupplicantSocketPath = "/var/run/wpa_supplicant/wlan0";
        if (!connectWpaControlSocket(wpaSupplicantSocketPath)) {
            return false;
        }
        m_ctrlNotifier = std::make_unique<QSocketNotifier>(
            wpa_ctrl_get_fd(m_ctrl), QSocketNotifier::Read);

        connect(m_ctrlNotifier.get(), SIGNAL(activated(int)), this, SLOT(controlEvent(int)));

        checkConnection();
        // TODO signal & status scanning should be started by the external initialization or a signal when the user switched to the configuration screen
        startSignalStrengthScanning();
        startWifiStatusScanning();

        qCDebug(CLASS_LC) << "wpa_supplicant control interface successfully initialized. WiFi connection:" << isConnected();
    }

    return true;
}

void WifiWpaSupplicant::on()
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    // TODO use a system-service class instead of launching shell script within the wifi control driver
    launch(m_scriptProcess, m_wifiOnScript);
    checkConnection();
    startScanTimer();
}

void WifiWpaSupplicant::off()
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    stopScanTimer();

    // TODO what about wpa_supplicant TERMINATE command? Or does that interfere with systemd service auto restart?
    // https://w1.fi/wpa_supplicant/devel/ctrl_iface_page.html
    // TODO use a system-service class instead of launching shell script within the wifi control driver
    launch(m_scriptProcess, m_wifiOffScript);
    setConnected(false);
}

bool WifiWpaSupplicant::reset()
{
    qCDebug(CLASS_LC) << "Disconnecting and removing all networks...";

    if (!controlRequest("DISCONNECT")) {
        return false;
    }

    setConnected(false);

    if (!controlRequest("REMOVE_NETWORK all")) {
        return false;
    }

    if (!controlRequest("SAVE_CONFIG")) {
        return false;
    }

    stopScanTimer();

    qCDebug(CLASS_LC) << "All networks removed and disconnected";

    return true;
}

bool WifiWpaSupplicant::join(const QString &ssid, WifiNetwork::Security security, const QString &password)
{
    // For more details about network creation & security option see:
    // https://github.com/loh-tar/wpa-cute/blob/master/src/networkconfig.cpp#L198

    if (!validateAuthentication(security, password)) {
        return false;
    }

    QString keyMgmnt;
    QString authAlg;

    switch (security) {
    case WifiNetwork::Security::Default :
        break;
    case WifiNetwork::Security::NoneOpen :
        keyMgmnt = "NONE";
        break;
    // TODO either test NoneWep & NoneWepShared or remove them. Do we even need to distinguish them?
    case WifiNetwork::Security::NoneWep  :
        keyMgmnt = "NONE";
        break;
    case WifiNetwork::Security::NoneWepShared :
        keyMgmnt = "NONE";
        authAlg = "SHARED";
        break;
    case WifiNetwork::Security::WPA_PSK  :
        keyMgmnt = "WPA-PSK";
        break;
    case WifiNetwork::Security::WPA2_PSK :
        keyMgmnt = "WPA2_PSK";
        break;
    default :
        qWarning(CLASS_LC) << "Authentication mode not (yet) implemented:" << security;
        return false;
    }

    char buf[WPA_BUF_SIZE];
    QList<WifiNetwork> networks = WifiWpaSupplicant::getConfiguredNetworks();

    // FIXME don't remove all networks, but check first if the SSID is already configured
    QString cmd = "REMOVE_NETWORK %1";
    // TODO check if ssid is within networks and get its networkId

    if (!controlRequest("REMOVE_NETWORK all", buf, WPA_BUF_SIZE)) {
        return false;
    }

    if (!controlRequest("ADD_NETWORK", buf, WPA_BUF_SIZE)) {
        return false;
    }
    QString networkId = QString(buf);

    if (!setNetworkParam(networkId, "ssid", ssid, true)) {
        return false;
    }

    if (!authAlg.isEmpty()) {
        if (!setNetworkParam(networkId, "auth_alg", authAlg)) {
            return false;
        }
    }

    if (!keyMgmnt.isEmpty()) {
        if (!setNetworkParam(networkId, "key_mgmt", keyMgmnt)) {
            return false;
        }
    }

    if (security == WifiNetwork::Security::NoneWep || security == WifiNetwork::Security::NoneWepShared) {
        writeWepKey(networkId, password, 0);
        setNetworkParam(networkId, "wep_tx_keyidx", "0");
    } else if (!password.isEmpty()) {
        if (!setNetworkParam(networkId, "psk", password, password.length() != 64)) {
            return false;
        }
    }

    cmd = "ENABLE_NETWORK %1";
    if (!controlRequest(cmd.arg(networkId), buf, WPA_BUF_SIZE)) {
        return false;
    }

    // TODO only save configuration after connection has been successfully established
    // - start timer to call STATUS and check for 'wpa_state=COMPLETED'
    // - repeat 3 times and pause 3s in between
    // - emit signal for connection result
    if (!controlRequest("SAVE_CONFIG", buf, WPA_BUF_SIZE)) {
        return false;
    }

    // not sure if required, but should make sure the connection is really established
    // controlRequest("REASSOCIATE", buf, WPA_BUF_SIZE);

    return true;
}

bool WifiWpaSupplicant::setNetworkParam(const QString& networkId, const QString& parm, const QString& val, bool quote/* = false*/)
{
    QString cmd;
    if (quote)
        cmd = "SET_NETWORK %1 %2 \"%3\"";
    else
        cmd = "SET_NETWORK %1 %2 %3";

    return controlRequest(cmd.arg(networkId).arg(parm).arg(val));
}

bool WifiWpaSupplicant::writeWepKey(const QString& networkId, const QString& value, int keyId)
{
    // Assume hex key if only hex characters are present and length matches with 40, 104, or 128-bit key
    auto len = value.size();
    auto hex = value.contains(QRegExp("^[0-9A-F]+$"));

    if (hex && len != 10 && len != 26 && len != 32) {
        hex = false;
    }
    QString var("wep_key%1");
    return setNetworkParam(networkId, var.arg(keyId), value, !hex);
}

void WifiWpaSupplicant::startNetworkScan()
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    controlRequest("SCAN");
}

bool WifiWpaSupplicant::startAccessPoint()
{
    qCDebug(CLASS_LC) << "TODO starting access point...";

    // FIXME Implement access point configuration
    return false;
}

/****************************************************************************/
bool WifiWpaSupplicant::wpsPushButtonConfigurationAuth(const WifiNetwork& network) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QString cmd("WPS_PBC %1");
    return controlRequest(cmd.arg(network.bssid()));
}

/****************************************************************************/
bool WifiWpaSupplicant::connectWpaControlSocket(const QString &wpaSupplicantSocketPath) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    m_ctrl = wpa_ctrl_open(wpaSupplicantSocketPath.toStdString().c_str());
    if (!m_ctrl) {
        qCCritical(CLASS_LC) << "wpa_ctrl_open() failed. Errno:" << errno;
        return false;
    }
    auto res = wpa_ctrl_attach(m_ctrl);
    if (res < 0) {
        qCCritical(CLASS_LC) << "notifier attach failed with error:" << res;
        wpa_ctrl_close(m_ctrl);
        m_ctrl = nullptr;
        return false;
    }
    return true;
}

/****************************************************************************/
bool WifiWpaSupplicant::controlRequest(const QString& cmd) {
    char buf[WPA_BUF_SIZE];
    return controlRequest(cmd, buf, WPA_BUF_SIZE);
}

bool WifiWpaSupplicant::controlRequest(const QString& cmd, char* buf, size_t buflen) {
    if (!m_ctrl) {
        return false;
    }

    // we might have multiple threads accessing this driver (e.g. QML user interface & pollers)
    std::lock_guard<std::mutex> lock(m_wpaMutex);

    auto res = wpa_ctrl_request(
        m_ctrl, cmd.toStdString().c_str(), cmd.size(), buf, &buflen, nullptr);

    buf[buflen] = '\0';
    if (res < 0) {
        qCCritical(CLASS_LC) << "wpa_ctrl_request failed for command" << cmd << "with error:" << res;
        return false;
    }

    // check response, e.g. when requesting information from an invalid network id
    QString response = QString::fromLocal8Bit(buf);
    qCDebug(CLASS_LC()) << cmd << "response:" << response;

    if (response.startsWith("FAIL")) {
        return false;
    }

    return true;
}

/****************************************************************************/
void WifiWpaSupplicant::controlEvent(int fd) {
    Q_UNUSED(fd)

    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    char buf[256] = {};
    while (wpa_ctrl_pending(m_ctrl) > 0) {
        auto buf_len = sizeof(buf) - 1;
        wpa_ctrl_recv(m_ctrl, buf, &buf_len);
        buf[buf_len] = '\0';
        parseEvent(buf);
    }
}

/****************************************************************************/

void WifiWpaSupplicant::parseEvent(char* msg) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    // skip priority
    char* pos = msg;
    int priority = 2;

    if (*pos == '<') {
        pos++;
        priority = atoi(pos);
        pos = strchr(pos, '>');
        if (pos) {
            pos++;
        } else {
            pos = msg;
        }
    }

    QString event = QString::fromLocal8Bit(pos);

    if (event.startsWith(WPA_CTRL_REQ)) {
        qCDebug(CLASS_LC) << " interactive authentication request";
        processCtrlReq(event);
    } else if (event.startsWith(WPA_EVENT_SCAN_RESULTS)) {
        qCDebug(CLASS_LC) << " scan_results available!";
        setScanStatus(ScanOk);
        readScanResults();
    } else if (event.startsWith(WPA_EVENT_SCAN_STARTED)) {
        qCDebug(CLASS_LC) << "scan started!";
        setScanStatus(Scanning);
    } else if (event.startsWith(WPA_EVENT_SCAN_FAILED)) {
        qCDebug(CLASS_LC) << "scan failed!";
        setScanStatus(ScanFailed);
    } else if (event.startsWith(WPA_EVENT_CONNECTED)) {
        qCDebug(CLASS_LC) << " connected!";
        setConnected(true);
    } else if (event.startsWith(WPS_EVENT_AP_AVAILABLE_PBC)) {
        qCDebug(CLASS_LC) << " WPS PBC available!";
    } else if (event.startsWith(WPA_EVENT_NETWORK_NOT_FOUND)) {
        qCDebug(CLASS_LC) << " network not found!";
        // signal?
    } else if (event.startsWith(WPA_EVENT_DISCONNECTED)) {
        qCDebug(CLASS_LC) << " disconnected!";
        setConnected(false);
    } else if (event.startsWith(WPA_EVENT_TERMINATING)) {
        qCDebug(CLASS_LC) << " terminated!";
        setConnected(false);
    } else if (event.startsWith(WPS_EVENT_ACTIVE)) {
        qCDebug(CLASS_LC) << " Push button Configuration active!";
    } else {
        qCDebug(CLASS_LC) << " event:" << event;
    }
}

/****************************************************************************/
/*
From: https://w1.fi/wpa_supplicant/devel/ctrl_iface_page.html#ctrl_iface_interactive

If wpa_supplicant needs additional information during authentication (e.g., password), it will use a specific prefix, CTRL-REQ- (WPA_CTRL_REQ macro) in an unsolicited event message. An external program, e.g., a GUI, can provide such information by using CTRL-RSP- (WPA_CTRL_RSP macro) prefix in a command with matching field name.

The following fields can be requested in this way from the user:

- IDENTITY (EAP identity/user name)
- PASSWORD (EAP password)
- NEW_PASSWORD (New password if the server is requesting password change)
- PIN (PIN code for accessing a SIM or smartcard)
- OTP (one-time password; like password, but the value is used only once)
- PASSPHRASE (passphrase for a private key file)

    CTRL-REQ-<field name>-<network id>-<human readable text>
    CTRL-RSP-<field name>-<network id>-<value>

For example, request from wpa_supplicant:

    CTRL-REQ-PASSWORD-1-Password needed for SSID test-network

And a matching reply from the GUI:

    CTRL-RSP-PASSWORD-1-secret
 */
void WifiWpaSupplicant::processCtrlReq(const QString& req) {

    QString type = req.section('-', 0, 0);
    QString networkId = req.section('-', 1, 1).section(':', 0, 0);
    QString text = req.section(':', 1, 1);

    bool ok;
    int id = networkId.toInt(&ok);
    if (!ok) {
        qCWarning(CLASS_LC()) << "Bad request data:" << req;
        return;
    }

    // TODO This is only a proof of concept implementation!
    // IF we really require this functionality in the future then improve the simple POC code with:
    // - validate type here. It must be one of:  "PASSWORD", "NEW_PASSWORD", "IDENTITY", "PASSPHRASE", "OTP"
    // - create an enum for the type. Don't pass string value to client
    emit authenticationRequest(type, id, text);
}

void WifiWpaSupplicant::authenticationResponse(const QString& type, int networkId, const QString& response)
{
    // Response to processCtrlReq above, cmd has to be: "CTRL-RSP-PASSWORD-1:myPassword", "CTRL-RSP-OTP-2:1234" etc
    QString cmd = QString("%1%2-%3:%4").arg(WPA_CTRL_RSP).arg(type).arg(networkId).arg(response);
    controlRequest(cmd);
}

/****************************************************************************/
void WifiWpaSupplicant::readScanResults() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    // Note: the simple all-in-one "SCAN_RESULTS" command might fail if there are too many networks! (response buffer too small)
    // Therefore we are using individual "BSS <networkid>" calls.
    m_scanResults.clear();

    for (int i = 0; i < maxScanResults(); i++) {
        if (!addBSS(i))
            break;
    }
    emit networksFound(m_scanResults);
}

bool WifiWpaSupplicant::addBSS(int networkId) {
    char buf[WPA_BUF_SIZE];
    QString cmd("BSS %1");

    if (!controlRequest(cmd.arg(networkId), buf, WPA_BUF_SIZE)) {
        return false;
    }

    QString bss(buf);
    if (bss.isEmpty()) {
        return false;
    }

    QString id, ssid, bssid, flags, wps_name, pri_dev_type;
    int level = -100;

    QStringList lines = bss.split(QRegExp("\\n"));
    for (QStringList::Iterator it = lines.begin(); it != lines.end(); it++) {
        int pos = (*it).indexOf('=') + 1;
        if (pos < 1) {
            continue;
        }

        if ((*it).startsWith("bssid=")) {
            bssid = (*it).mid(pos);
        } else if ((*it).startsWith("id=")) {
            id = (*it).mid(pos);
        } else if ((*it).startsWith("level=")) {
            level = (*it).mid(pos).toInt();
        } else if ((*it).startsWith("flags=")) {
            flags = (*it).mid(pos);
        } else if ((*it).startsWith("ssid=")) {
            ssid = (*it).mid(pos);
        } else if ((*it).startsWith("wps_device_name=")) {
            wps_name = (*it).mid(pos);
        } else if ((*it).startsWith("wps_primary_device_type=")) {
            pri_dev_type = (*it).mid(pos);
        }
    }

    WifiNetwork::Security security = getSecurityFromFlags(flags, networkId);
    WifiNetwork network { id, ssid, bssid, level, security, flags.contains("[WPS") };
    qCDebug(CLASS_LC) << "Network found:" << network;

    m_scanResults.append(network);

    return true;
}

WifiNetwork::Security WifiWpaSupplicant::getSecurityFromFlags(const QString& flags, int networkId)
{
    // Partial implementation of security flags, e.g. no support for EAP
    // Sufficiant for now...
    WifiNetwork::Security auth;
    if (flags.indexOf("[WPA2-EAP") >= 0) {
        auth = WifiNetwork::WPA2_EAP;
    } else if (flags.indexOf("[WPA-EAP") >= 0) {
        auth = WifiNetwork::WPA_EAP;
    } else if (flags.indexOf("[WPA2-PSK") >= 0) {
        auth = WifiNetwork::WPA2_PSK;
    } else if (flags.indexOf("[WPA-PSK") >= 0) {
        auth = WifiNetwork::WPA_PSK;
    } else {
        auth = WifiNetwork::NoneOpen;
    }

    // UNTESTED WEP implementation. Shouldn't be used anymore anyways...
    if (flags.indexOf("WEP") >= 0) {
        if (auth == WifiNetwork::NoneOpen) {
            auth = WifiNetwork::NoneWep;
        }
        if (networkId >= 0) {
            char buf[WPA_BUF_SIZE];
            QString cmd = "GET_NETWORK %1 auth_alg";
            if (controlRequest(cmd.arg(networkId), buf, WPA_BUF_SIZE)) {
                if (strcmp(buf, "SHARED") == 0) {
                    auth = WifiNetwork::NoneWepShared;
                }
            }
        }
    }

    return auth;
}

QList<WifiNetwork> &WifiWpaSupplicant::getConfiguredNetworks()
{
    static QList<WifiNetwork> networks;
    char buf[WPA_BUF_SIZE];

    networks.clear();
    if (!controlRequest("LIST_NETWORKS", buf, WPA_BUF_SIZE)) {
        return networks;
    }

    for(QString line : QString(buf).split('\n')) {
        QStringList data = line.split('\t');
        // assume network id is always a number, it's not completely clear in the docs...
        if (!data.at(0).contains(QRegExp("^[0-9]+$")))
            continue;

        WifiNetwork wn {data.at(0), data.at(1), data.at(2), 0, WifiNetwork::Security::Default, false, data.at(3).contains("[CURRENT]") };
        networks.append(wn);
    }

    return networks;
}

/****************************************************************************/
WifiStatus WifiWpaSupplicant::parseStatus(const char* buffer) {
    QString results(buffer);
    auto lines = results.splitRef("\n");
    WifiStatus wifiStatus { "", "", "", "" };

    for (int i = 0; i < lines.length(); i++) {
        int pos = lines[i].indexOf("=");
        if (pos > 0) {
            auto key = lines[i].left(pos);
            auto value = lines[i].mid(pos + 1);
            if ("bssid" == key) {
                wifiStatus.bssid = value.toString();
                // TODO is there a better way to determine if we are connected to an AP?
                wifiStatus.connected = !wifiStatus.bssid.isEmpty();
            } else if ("ssid" == key) {
                wifiStatus.name = value.toString();
            } else if ("ip_address" == key) {
                wifiStatus.ipAddress = value.toString();
            } else if ("address" == key) {
                wifiStatus.macAddress = value.toString();
            }
        }
    }

    return wifiStatus;
}

// code based on WpaGui::updateSignalMeter()
int WifiWpaSupplicant::parseSignalStrength(const char* buffer) {
    const char* rssi;
    int rssiValue = -100;

    /* In order to eliminate signal strength fluctuations, try
     * to obtain averaged RSSI value in the first place. */
    if ((rssi = strstr(buffer, "AVG_RSSI=")) != NULL)
        rssiValue = atoi(&rssi[sizeof("AVG_RSSI")]);
    else if ((rssi = strstr(buffer, "RSSI=")) != NULL)
        rssiValue = atoi(&rssi[sizeof("RSSI")]);
    else {
        qCDebug(CLASS_LC) << "Failed to get RSSI value";
    }

    return rssiValue;
}

bool WifiWpaSupplicant::checkConnection()
{
    char buf[WPA_BUF_SIZE];
    if (!controlRequest("STATUS", buf, WPA_BUF_SIZE)) {
        return false;
    }

    WifiStatus wifiStatus = parseStatus(buf);
    setConnected(wifiStatus.connected);
    return true;
}

void WifiWpaSupplicant::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)

    if (!(m_wifiStatusScanning || m_signalStrengthScanning)) {
        return;
    }
    if (!isConnected()) {
        qCDebug(CLASS_LC) << "Ignoring scanning event: WiFi is not connected!";
        return;
    }

    char buf[WPA_BUF_SIZE];
    if (m_wifiStatusScanning) {
        if (controlRequest("STATUS", buf, WPA_BUF_SIZE)) {
            WifiStatus wifiStatus = parseStatus(buf);

            if (wifiStatus.name != m_wifiStatus.name) {
                emit networkNameChanged(wifiStatus.name);
            }

            if (wifiStatus.ipAddress != m_wifiStatus.ipAddress) {
                emit ipAddressChanged(wifiStatus.ipAddress);
            }

            if (wifiStatus.macAddress != m_wifiStatus.macAddress) {
                emit macAddressChanged(wifiStatus.macAddress);
            }
            // HACK clean up WifiStatus
            int oldSignalStrength = m_wifiStatus.signalStrength;
            m_wifiStatus = wifiStatus;
            m_wifiStatus.signalStrength = oldSignalStrength;
            setConnected(m_wifiStatus.connected);
        }
    }

    if (m_signalStrengthScanning) {
        if (controlRequest("SIGNAL_POLL", buf, WPA_BUF_SIZE)) {
            int value = parseSignalStrength(buf);
            if (value != m_wifiStatus.signalStrength) {
                m_wifiStatus.signalStrength = value;
                emit signalStrengthChanged(value);
            }
        }
    }
}
