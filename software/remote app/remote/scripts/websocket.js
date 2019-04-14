////////////////////////////////////////////////////////////////////
// WEBSOCKET SERVER MESSAGING
// Parses messages coming from the python app
////////////////////////////////////////////////////////////////////

function parseWSServerMessage (message) {
    switch (message) {
    case String(message.match(/^battery,.*/)):
        var chargingType = message.split(',');
        chargingScreen.state = "hidden";
        battery_level = chargingType[1];
        if (battery_time == 0) {
            var d = new Date();
            battery_time = d.getTime()
        }
        break;
    case String(message.match(/^usb,.*/)):
        chargingScreen.state = "visible";
        battery_time = 0;
        break;
    case String(message.match(/^voltage,.*/)):
        var voltage = message.split(',');
        battery_voltage = voltage[1];
        break;
    case "low battery":
        lowBatteryNotification.open();
        if (displayOff.visible) {
            displayOff.visible = false;
        }
        //        loader_main.item.pageSettings.setBrightness(100);
        if (socketServer.clientId != undefined) {
            socketServer.clientId.sendTextMessage("wakeup");
        }
        break;
    case "wifi off":
        socket.active = false;
        websocketReconnect.running = false;
        break;
    case "wifi on":
        connectionLoader.state = "connecting"
        websocketReconnect.tries = 0;
        socket.active = true;
        break;
    case "display on": // remove this when standby mode implemented
        displayOff.visible = false;
        break;
    case "display off": // remove this when standby mode implemented
        displayOff.visible = true;
        break;
    case String(message.match(/^brightness,.*/)):
        var brightness = message.split(',');
        display_brightness = brightness[1];
        break;
    }
}

