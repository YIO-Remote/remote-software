#!/usr/bin/env python

#####################################################################################################################
###
### Wakes up and puts the display to sleep, as well as handling gesture volume control and wifi-standby
###
#####################################################################################################################

from apds9960.const import *
from apds9960 import APDS9960

import smbus
from time import sleep
import time
import subprocess

# touch things
from evdev import InputDevice
from select import select

dev = InputDevice('/dev/input/event0')

# multi threading
import thread

# websocket
import websocket

port = 3
bus = smbus.SMBus(port)

apds = APDS9960(bus)

dirs = {
    APDS9960_DIR_NONE: "none",
    APDS9960_DIR_LEFT: "left",
    APDS9960_DIR_RIGHT: "right",
    APDS9960_DIR_UP: "up",
    APDS9960_DIR_DOWN: "down",
    APDS9960_DIR_NEAR: "near",
    APDS9960_DIR_FAR: "far",
}

gesture_detect = False

proximity_detected = False
proximity_treshold = 100

dim_light = False

touch_detected = True
was_touch_event = False

sleep_mode = False

start = time.time()
end = time.time()

dimlevel_current = 90
brightness = 100

def on_message(ws, message):
    global touch_detected
    print(message)
    if message == "wakeup":
        touch_detected = True

def on_error(ws, error):
    print(error)

def on_close(ws):
    print("### Websocket closed")

def on_open(ws):
    print("### Websocket opened")
    ws.send("display")

ws = websocket.WebSocketApp("ws://localhost:8234",
                        on_message = on_message,
                        on_error = on_error,
                        on_close = on_close)

def websocket_start():
    ws.on_open = on_open
    ws.run_forever()

def mapFromTo(x,a,b,c,d):
    leftSpan = b-a
    rightSpan = d-c

    valueScaled = float(x -a) / float(leftSpan)

    return int(c + (valueScaled * rightSpan))

def watchdog():
    global touch_detected
    global was_touch_event
    global dim_light
    global gesture_detect
    global sleep_mode
    global start
    global end
    global dimlevel_current
    while True:
        sleep(1)
        end = time.time()
        if end - start > 10 and proximity_detected == True and dim_light == False:
            start = end
            dim_light = True
            tmp = (brightness-40)/3
            subprocess.call("/usr/bin/display_brightness.sh " + str(dimlevel_current) + " " + str(tmp), shell=True)
            dimlevel_current = tmp
            ws.send("brightness," + str(dimlevel_current))
            print "Dimming brightness"

        if end - start > 15 and proximity_detected == True and was_touch_event == True:
            touch_detected = False
            #turn off gesture detection
            gesture_detect = False
            sleep(0.5)
            print "Gesture detection off"

        if end - start > 15 and proximity_detected == True and was_touch_event == False:
            was_touch_event = True
            touch_detected = False
            #turn off gesture detection
            gesture_detect = False
            sleep(0.5)
            print "Gesture detection off"

        if end - start > 600 and was_touch_event == False and sleep_mode == False:
            touch_detected = False
            sleep_mode = True
            print "Wifi off"
            subprocess.call("systemctl stop wpa_supplicant@wlan0.service", shell=True)
            ws.send("wifi off")
            # subprocess.call("echo -e powersave > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", shell=True)

        if end - start > 5400 and was_touch_event == False and sleep_mode == True:
            print "No activity, shutdown"
            subprocess.call("halt", shell=True)

def gesture_detection():
    global gesture_detect
    global dim_light
    global brightness
    global dimlevel_current
    while True:
        sleep(0.5)
        if gesture_detect == True:
            # sleep(0.5)
            if apds.isGestureAvailable():
                motion = apds.readGesture()
                dim_light = False
                # print("Gesture={}".format(dirs.get(motion, "unknown")))
                if dirs.get(motion, "unknown") == "right":
                    ws.send("volume down")
                    ws.send("display on")
                    subprocess.call("/usr/bin/display_brightness.sh " + str(dimlevel_current) + " " + str(brightness), shell=True)
                    dimlevel_current = brightness
                    ws.send("brightness," + str(dimlevel_current))

                if dirs.get(motion, "unknown") == "left":
                    ws.send("volume up")
                    ws.send("display on")
                    subprocess.call("/usr/bin/display_brightness.sh " + str(dimlevel_current) + " " + str(brightness), shell=True)
                    dimlevel_current = brightness
                    ws.send("brightness," + str(dimlevel_current))

                if dirs.get(motion, "unknown") == "up":
                    ws.send("right")
                    ws.send("display on")
                    subprocess.call("/usr/bin/display_brightness.sh " + str(dimlevel_current) + " " + str(brightness), shell=True)
                    dimlevel_current = brightness
                    ws.send("brightness," + str(dimlevel_current))

                if dirs.get(motion, "unknown") == "down":
                    ws.send("left")
                    ws.send("display on")
                    subprocess.call("/usr/bin/display_brightness.sh " + str(dimlevel_current) + " " + str(brightness), shell=True)
                    dimlevel_current = brightness
                    ws.send("brightness," + str(dimlevel_current))

def touch_event_checker():
    global proximity_detected
    global touch_detected
    global was_touch_event
    global start
    global dev
    global dim_light
    global brightness
    global dimlevel_current
    while True:
        sleep(1)
        start = time.time()
        r,w,x = select([dev], [], [])
        for event in dev.read():
            if event:
                touch_detected = True
                was_touch_event = True
                if dim_light == True:
                    ws.send("display on")
                    subprocess.call("/usr/bin/display_brightness.sh " + str(dimlevel_current)  + " " + str(brightness), shell=True)
                    dimlevel_current = brightness
                    ws.send("brightness," + str(dimlevel_current))
                    dim_light = False
                break
            break

thread.start_new_thread(watchdog, ())
thread.start_new_thread(touch_event_checker, ())
thread.start_new_thread(gesture_detection, ())
thread.start_new_thread(websocket_start, ())

# Enable proximity sensor
apds.enableProximitySensor()
apds.setProximityGain(3)
apds.setLEDBoost(0)

# Enable ambient light sensor
apds.enableLightSensor()

sleep(0.5)

# try:
while True:
    sleep(1)

    proximity = apds.readProximity()

    if proximity > proximity_treshold and proximity_detected == False and touch_detected == False:
        subprocess.call("echo -e ondemand > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", shell=True)
        print "Proximity detected"
        print proximity
        proximity_detected = True
        start = time.time()

        ambientlight = apds.readAmbientLight()
        brightness = mapFromTo(ambientlight,0,300,20,100)
        brightness = brightness + 40
        if brightness > 100:
            brightness = 100

        # turn on wifi
        if sleep_mode == True:
            subprocess.call("systemctl start wpa_supplicant@wlan0.service", shell=True)
            sleep_mode = False
            sleep(0.5)
            ws.send("wifi on")
        # turn off proximity sensor
        apds.disableProximitySensor()
        apds.disableLightSensor()

        # turn on the display
        ws.send("display on")
        subprocess.call("/usr/bin/display_brightness.sh " + str(dimlevel_current) + " " + str(brightness), shell=True)
        dimlevel_current = brightness
        ws.send("brightness," + str(dimlevel_current))
        sleep(0.5)

        # turn on gesture sensor
        apds.enableGestureSensor()
        apds.setGestureGain(0)
        apds.setLEDBoost(0)
        sleep(1)
        gesture_detect = True

    if proximity_detected == False and touch_detected == True:
        subprocess.call("echo -e ondemand > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", shell=True)
        print "Touch detected"
        proximity_detected = True
        start = time.time()

        ambientlight = apds.readAmbientLight()
        brightness = mapFromTo(ambientlight,0,300,20,100)
        brightness = brightness + 40
        if brightness > 100:
            brightness = 100

        # turn on wifi
        if sleep_mode == True:
            subprocess.call("systemctl start wpa_supplicant@wlan0.service", shell=True)
            sleep_mode = False
            sleep(0.5)
            ws.send("wifi on")

        # turn off proximity sensor
        apds.disableProximitySensor()
        apds.disableLightSensor()

        # turn on the display
        ws.send("display on")
        subprocess.call("/usr/bin/display_brightness.sh " + str(dimlevel_current) + " " + str(brightness), shell=True)
        dimlevel_current = brightness
        ws.send("brightness," + str(dimlevel_current))
        sleep(0.5)

        # turn on gesture sensor
        apds.enableGestureSensor()
        apds.setGestureGain(0)
        apds.setLEDBoost(0)
        sleep(1)
        gesture_detect = True

    if proximity_detected == True and was_touch_event == True and touch_detected == False:
        proximity_detected = False
        was_touch_event = False
        dim_light = False
        subprocess.call("/usr/bin/display_brightness.sh " + str(dimlevel_current) + " 0", shell=True)
        dimlevel_current = 0
        ws.send("brightness," + str(dimlevel_current))
        ws.send("display off")
        print "Display off"
        subprocess.call("echo -e powersave > /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", shell=True)

        apds.disableGestureSensor()

        # Enable proximity sensor
        apds.enableProximitySensor()
        apds.setProximityGain(3)
        apds.setLEDBoost(0)

        apds.enableLightSensor()
        sleep(0.5)
