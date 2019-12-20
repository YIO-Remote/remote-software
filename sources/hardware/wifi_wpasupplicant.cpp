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
#include <QThread>
#include <QFile>
#include <QEventLoop>
#include <QTimer>

#include <cstdio>
#include <cerrno>
#include <exception>

#include "hw_config.h"
#include "systemservice_name.h"
#include "wifi_wpasupplicant.h"
#include "common/wpa_ctrl.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "WpaCtrl");

const size_t WPA_BUF_SIZE = 2048;

WifiWpaSupplicant::WifiWpaSupplicant(WebServerControl *webServerControl,
                                     SystemService *systemService,
                                     QObject *parent)
    : WifiControl(parent)
    , m_ctrl(nullptr)
    , p_webServerControl(webServerControl)
    , p_systemService(systemService)
    , m_wpaSupplicantSocketPath(HW_DEF_WIFI_WPA_SOCKET)
    , m_removeNetworksBeforeJoin(HW_DEF_WIFI_RM_BEFORE_JOIN)
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

        if (!connectWpaControlSocket()) {
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

    p_systemService->startService(SystemServiceName::WIFI);
    checkConnection();
    startScanTimer();
}

void WifiWpaSupplicant::off()
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    stopScanTimer();

    // TODO what about wpa_supplicant TERMINATE command? Or does that interfere with systemd service auto restart?
    // https://w1.fi/wpa_supplicant/devel/ctrl_iface_page.html
    p_systemService->stopService(SystemServiceName::WIFI);
    setConnected(false);
}

bool WifiWpaSupplicant::reset()
{
    if (m_ctrl) {
        if (m_ctrlNotifier) {
            m_ctrlNotifier->setEnabled(false);
            m_ctrlNotifier->disconnect();
        }
        wpa_ctrl_detach(m_ctrl);
        wpa_ctrl_close(m_ctrl);
        m_ctrl = nullptr;
    }

    return init();
}

bool WifiWpaSupplicant::clearConfiguredNetworks()
{
    qCDebug(CLASS_LC) << "Disconnecting and removing all networks...";

    if (!controlRequest("DISCONNECT")) {
        return false;
    }

    setConnected(false);

    if (!controlRequest("REMOVE_NETWORK all")) {
        return false;
    }

    if (!saveConfiguration()) {
        return false;
    }

    stopScanTimer();

    qCDebug(CLASS_LC) << "All networks removed and disconnected";

    return true;
}

bool WifiWpaSupplicant::join(const QString &ssid, const QString &password, WifiSecurity security)
{
    qCDebug(CLASS_LC) << "Joining network:" << ssid << security;

    // For more details about network creation & security option see:
    // https://github.com/loh-tar/wpa-cute/blob/master/src/networkconfig.cpp#L198

    if (!validateAuthentication(security, password)) {
        return false;
    }

    QString keyMgmnt;
    QString authAlg;

    // KISS: as long as we don't need EAP this simple logic is "good enough".
    // Even though it's a classical OCP violation: different security should be handled in their own specialized classes!
    // Note: EAP is validated & rejected in above's validateAuthentication call
    switch (security) {
    case WifiSecurity::DEFAULT :
        break;
    case WifiSecurity::NONE_OPEN :
        keyMgmnt = "NONE";
        break;
    // TODO either test NoneWep & NoneWepShared or remove them. Do we even need to distinguish them?
    case WifiSecurity::NONE_WEP  :
        keyMgmnt = "NONE";
        break;
    case WifiSecurity::NONE_WEP_SHARED :
        keyMgmnt = "NONE";
        authAlg = "SHARED";
        break;
    case WifiSecurity::WPA_PSK  :
        keyMgmnt = "WPA-PSK";
        break;
    case WifiSecurity::WPA2_PSK :
        keyMgmnt = "WPA-PSK";
        break;
    default :
        qCWarning(CLASS_LC) << "Authentication mode not (yet) implemented:" << security;
        return false;
    }

    char buf[WPA_BUF_SIZE];
    QString cmd;
    if (m_removeNetworksBeforeJoin) {
        if (!controlRequest("REMOVE_NETWORK all", buf, WPA_BUF_SIZE)) {
            return false;
        }
    } else {
        cmd = "REMOVE_NETWORK %1";
        QList<WifiNetwork> networks = WifiWpaSupplicant::getConfiguredNetworks();
        for (WifiNetwork network : networks) {
            if (network.name() == ssid) {
                controlRequest(cmd.arg(network.id()), buf, WPA_BUF_SIZE);
            }
        }
    }

    if (!controlRequest("ADD_NETWORK", buf, WPA_BUF_SIZE)) {
        return false;
    }
    QString networkId = QString(buf);
    networkId.remove('\n');

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

    if (security == WifiSecurity::NONE_WEP || security == WifiSecurity::NONE_WEP_SHARED) {
        if (!writeWepKey(networkId, password, 0)) {
            return false;
        }
        if (!setNetworkParam(networkId, "wep_tx_keyidx", "0")) {
            return false;
        }
    } else if (!password.isEmpty()) {
        if (!setNetworkParam(networkId, "psk", password, password.length() != 64)) {
            return false;
        }
    }

    cmd = "ENABLE_NETWORK %1";
    if (!controlRequest(cmd.arg(networkId), buf, WPA_BUF_SIZE)) {
        controlRequest("RECONFIGURE"); // reload from cfg and hope for the best
        return false;
    }

    // wait for successful connection and save configuration once connected.
    // TODO asynchronous check using a timer. Return immediately and notify caller with signal.
    for (int i = 0; i < getNetworkJoinRetryCount(); i++) {
        qCDebug(CLASS_LC) << "Checking Wifi state after enabling network configuration (" << (i+1) << "/" << getNetworkJoinRetryCount() << ")";

        if (checkConnection()) {
            bool resetIfFailed = false;
            return saveConfiguration(resetIfFailed);
        }

        QThread::currentThread()->msleep(getNetworkJoinRetryDelayMs());
    }

    qCWarning(CLASS_LC) << "Failed to establish connection to AP" << ssid << "after" << getNetworkJoinRetryCount() << "retries";
    return false;
}

bool WifiWpaSupplicant::setNetworkParam(const QString& networkId, const QString& parm, const QString& val, bool quote/* = false*/)
{
    QString cmd;
    if (quote)
        cmd = "SET_NETWORK %1 %2 \"%3\"";
    else
        cmd = "SET_NETWORK %1 %2 %3";

    if (!controlRequest(cmd.arg(networkId).arg(parm).arg(val))) {
        controlRequest("RECONFIGURE"); // reload from cfg and hope for the best
        return false;
    }

    return true;
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

QList<WifiNetwork>& WifiWpaSupplicant::scanForAvailableNetworks(int timeout)
{
    QEventLoop loop;
    QTimer timeoutTimer;
    connect(this, SIGNAL(networksFound()), &loop, SLOT(quit()));
    connect(&timeoutTimer, SIGNAL(timeout()), &loop, SLOT(quit()));
    timeoutTimer.start(timeout);
    loop.exec(); //blocks untill either theSignalToWaitFor or timeout was fired

    return scanResult();
}

bool WifiWpaSupplicant::startAccessPoint()
{
    qCDebug(CLASS_LC) << "TODO starting access point...";

    if (!clearConfiguredNetworks()) {
        return false;
    }

    QString cmd = "SET ap_scan 1";
    if (!controlRequest(cmd)) {
        return false;
    }
    controlRequest("SAVE_CONFIG");

    // FIXME set wpa_supplicant access point configuration:
    // # reset configuration file
    // mkdir -p /etc/wpa_supplicant
    // echo "ctrl_interface=/var/run/wpa_supplicant
    // ap_scan=1
    // update_config=1
    // " > /etc/wpa_supplicant/wpa_supplicant-wlan0.conf

    // TODO logic from reset-wifi.sh still required to restart networking, dns & wpa_supplicant?
    //    p_systemService->restartService(SystemServiceName::NETWORKING);
    //    p_systemService->restartService(SystemServiceName::NAME_RESOLUTION);
    //    p_systemService->restartService(SystemServiceName::WIFI);

    // TODO scan for nearby wifis
    //    iw dev wlan0 scan >> /dev/null
    //     /usr/bin/yio-remote/wifi_network_list.sh > /networklist

    QList<WifiNetwork> networks = scanForAvailableNetworks(5000);

    // FIXME legacy function for PHP setup portal
    QFile qFile("/networklist");
    if (!qFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qCWarning(CLASS_LC) << "Error opening file:" << qFile.fileName();
        return false;
    }
    QTextStream out(&qFile);
    for (WifiNetwork network : networks) {
        out << network.rssi() << ',' << network.name() << endl;
    }
    qFile.close();

    // TODO set static IP address
    //    echo "[Match]
    // Name=wlan0
    //
    // [Network]
    // Address=10.0.0.1/24" > /etc/systemd/network/20-wireless.network
    p_systemService->restartService(SystemServiceName::NETWORKING);

    // TODO turn off wlan service
    p_systemService->stopService(SystemServiceName::WIFI);
    // killall -9 wpa_supplicant

    // TODO DHCP and DNS service
    p_systemService->stopService(SystemServiceName::NAME_RESOLUTION);
    // dnsmasq -k --conf-file=/etc/dnsmasq.conf &

    // TODO launch hostapd
    // hostapd -B /etc/hostapd/hostapd.conf &

    // AP up and running, start web portal
    p_webServerControl->startWifiSetupPortal();

    return false;
}

QString WifiWpaSupplicant::countryCode() {
    char buf[WPA_BUF_SIZE];
    if (!controlRequest("GET country", buf, WPA_BUF_SIZE)) {
        return "";
    }

    return QString(buf);
}

void WifiWpaSupplicant::setCountryCode(QString &countryCode) {
    QString cmd = "SET country %1";
    if (controlRequest(cmd.arg(countryCode.toUpper()))) {
        controlRequest("SAVE_CONFIG");
    }
}

/****************************************************************************/
bool WifiWpaSupplicant::wpsPushButtonConfigurationAuth(const WifiNetwork& network) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QString cmd("WPS_PBC %1");
    return controlRequest(cmd.arg(network.bssid()));
}

/****************************************************************************/
bool WifiWpaSupplicant::connectWpaControlSocket() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    m_ctrl = wpa_ctrl_open(m_wpaSupplicantSocketPath.toStdString().c_str());
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
        qCDebug(CLASS_LC) << "Not initialized. Ignoring control request:" << cmd;
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
    qCDebug(CLASS_LC) << cmd << "response:" << response;

    if (response.startsWith("FAIL\n")) {
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
    Q_UNUSED(priority)

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

    WifiSecurity security = getSecurityFromFlags(flags, networkId);
    WifiNetwork network { id, ssid, bssid, level, security, flags.contains("[WPS") };
    qCDebug(CLASS_LC) << "Network found:" << network;

    m_scanResults.append(network);

    return true;
}

WifiSecurity WifiWpaSupplicant::getSecurityFromFlags(const QString& flags, int networkId)
{
    // Partial implementation of security flags, e.g. no support for EAP
    // Sufficiant for now...
    WifiSecurity auth;
    if (flags.indexOf("[WPA2-EAP") >= 0) {
        auth = WifiSecurity::WPA2_EAP;
    } else if (flags.indexOf("[WPA-EAP") >= 0) {
        auth = WifiSecurity::WPA_EAP;
    } else if (flags.indexOf("[WPA2-PSK") >= 0) {
        auth = WifiSecurity::WPA2_PSK;
    } else if (flags.indexOf("[WPA-PSK") >= 0) {
        auth = WifiSecurity::WPA_PSK;
    } else {
        auth = WifiSecurity::NONE_OPEN;
    }

    // UNTESTED WEP implementation. Shouldn't be used anymore anyways...
    if (flags.indexOf("WEP") >= 0) {
        if (auth == WifiSecurity::NONE_OPEN) {
            auth = WifiSecurity::NONE_WEP;
        }
        if (networkId >= 0) {
            char buf[WPA_BUF_SIZE];
            QString cmd = "GET_NETWORK %1 auth_alg";
            if (controlRequest(cmd.arg(networkId), buf, WPA_BUF_SIZE)) {
                if (strcmp(buf, "SHARED") == 0) {
                    auth = WifiSecurity::NONE_WEP_SHARED;
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

        WifiNetwork wn {data.at(0), data.at(1), data.at(2), 0, WifiSecurity::DEFAULT, false, data.at(3).contains("[CURRENT]") };
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
            } else if ("ssid" == key) {
                wifiStatus.name = value.toString();
            } else if ("ip_address" == key) {
                wifiStatus.ipAddress = value.toString();
            } else if ("address" == key) {
                wifiStatus.macAddress = value.toString();
            } else if ("wpa_state" == key) {
                wifiStatus.connected = value == "COMPLETED";
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
    if ((rssi = strstr(buffer, "AVG_RSSI=")) != nullptr)
        rssiValue = atoi(&rssi[sizeof("AVG_RSSI")]);
    else if ((rssi = strstr(buffer, "RSSI=")) != nullptr)
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

bool WifiWpaSupplicant::saveConfiguration(bool resetCfgIfFailed) {
    if (!controlRequest("SAVE_CONFIG")) {
        qCWarning(CLASS_LC) << "Error saving current wpa_supplicant configuration! Please verify that /etc/wpa_supplicant/wpa_supplicant-wlan0.conf has 'update_config=1' set. Otherwise the configration cannot be persisted!";
        if (resetCfgIfFailed) {
            controlRequest("RECONFIGURE"); // reload from cfg and hope for the best
        }
        return false;
    }
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

bool WifiWpaSupplicant::getRemoveNetworksBeforeJoin() const
{
    return m_removeNetworksBeforeJoin;
}

void WifiWpaSupplicant::setRemoveNetworksBeforeJoin(bool removeNetworksBeforeJoin)
{
    m_removeNetworksBeforeJoin = removeNetworksBeforeJoin;
}

QString WifiWpaSupplicant::getWpaSupplicantSocketPath() const
{
    return m_wpaSupplicantSocketPath;
}

void WifiWpaSupplicant::setWpaSupplicantSocketPath(const QString &wpaSupplicantSocketPath)
{
    m_wpaSupplicantSocketPath = wpaSupplicantSocketPath;
}
