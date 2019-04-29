#!/usr/bin/env python

#####################################################################################################################
###
### Handling proximity detection, ambient light detection and touch detection
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

proximity_detect = False
proximity_treshold = 100

def on_message(ws, message):
    global gesture_detect
    print(message)
    if message == "gesture off":
        gesture_detect = False
        apds.disableGestureSensor()
    if message == "proximity detect on":
        proximity_detect = True
    if message == "proximity detect off":
        proximity_detect = False

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

def gesture_detection():
    global gesture_detect
    while True:
        sleep(0.5)
        if gesture_detect == True:
            # sleep(0.5)
            if apds.isGestureAvailable():
                motion = apds.readGesture()
                # print("Gesture={}".format(dirs.get(motion, "unknown")))
                if dirs.get(motion, "unknown") == "right":
                    ws.send("gesture down")

                if dirs.get(motion, "unknown") == "left":
                    ws.send("gesture up")

                if dirs.get(motion, "unknown") == "up":
                    ws.send("gesture right")

                if dirs.get(motion, "unknown") == "down":
                    ws.send("gesture left")

def touch_event_checker():
    global dev
    while True:
        sleep(1)
        r,w,x = select([dev], [], [])
        for event in dev.read():
            if event:
                ws.send("touchDetected")
                break
            break

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

    if proximity > proximity_treshold and proximity_detect:
        print "Proximity detected"
        print proximity

        ws.send("proximityDetected")

        # figure out the brightness based on ambient light
        ambientlight = apds.readAmbientLight()
        brightness = mapFromTo(ambientlight,0,200,20,100)
        brightness = brightness + 20
        if brightness > 100:
            brightness = 100

        ws.send("brightness, " + brightness)

        # turn off proximity sensor
        apds.disableProximitySensor()
        apds.disableLightSensor()

        proximity_detect = False

        # turn on gesture sensor
        apds.enableGestureSensor()
        apds.setGestureGain(0)
        apds.setLEDBoost(0)
        sleep(1)
        gesture_detect = True