////////////////////////////////////////////////////////////////////
// WEBSOCKET SERVER MESSAGING
// Parses messages coming from the python app
////////////////////////////////////////////////////////////////////

function parseWSServerMessage (message) {
    switch (message) {
    case String(message.match(/^battery,.*/)):
        var chargingType = message.split(',');
        chargingScreen.item.state = "hidden";
        battery_level = chargingType[1];
        if (battery_time == 0) {
            var d = new Date();
            battery_time = d.getTime()
        }
        break;

    case String(message.match(/^usb,.*/)):
        chargingScreen.item.state = "visible";
        battery_time = 0;
        break;

    case String(message.match(/^voltage,.*/)):
        var voltage = message.split(',');
        battery_voltage = voltage[1];
        break;

    case String(message.match(/^brightness,.*/)):
        var brightness = message.split(',');
        standbyControl.display_brightness_ambient = brightness[1];
        if (standbyControl.display_autobrightness) {
            standbyControl.display_brightness = brightness[1];
        }
        break;

    case "low battery":
        lowBatteryNotification.open();
        if (displayOff.visible) {
            displayOff.visible = false;
        }
        if (socketServer.clientId != undefined) {
            socketServer.clientId.sendTextMessage("wakeup");
        }
        break;

    case "proximityDetected":
        standbyControl.proximityDetected = true;
        break;

    case "buttonPressDetected":
        standbyControl.buttonPressDetected = true;
        break;
    }
}

