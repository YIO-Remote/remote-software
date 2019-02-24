#!/bin/sh
mount /dev/mmcblk0p1 /mnt/boot
export QT_QPA_EVDEV_TOUCHSCREEN_PARAMETERS=/dev/input/event0:rotate=90:invertx
/usr/bin/remote-touch &
sleep 6
/usr/bin/standby_control.py &
sleep 5
/usr/bin/battery.py -l &