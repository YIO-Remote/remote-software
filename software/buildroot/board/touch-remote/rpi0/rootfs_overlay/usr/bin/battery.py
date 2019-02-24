#!/usr/bin/env python

# Writes the fraction of battery remaining as well as the current power source

# format:  <float 0.00 - 1.00>,<string [battery|usb]>
# 0.75,battery
# 1.00,usb

import time
import os
import argparse

from ADS1115 import ADS1115
ads1115 = ADS1115()

from websocket import create_connection
ws = create_connection("ws://localhost:8234")

# read SPI data from MCP3008 chip, 8 possible adc's (0 thru 7)
def readadc(adcnum):
    ads1115.set_channel(adcnum)
    ads1115.config_single_ended()
    time.sleep(0.1)
    adcout = ads1115.read_adc()
    return adcout

# Calculate the output of a voltage divider
# voltage_divider layout is:
# Vin ---[ R1 ]---[ R2 ]---GND
#               |
#              Vout
#
# Vout = R2 / (R1 + R2) * Vin
# e.g. if R1 = 6800 and R2 = 10000 and Vin is 5.2V then Vout is 3.095
#
def voltage_divider(r1, r2, vin):
        vout = vin * (r2 / (r1 + r2))
        return vout

# Shutdown system because of low battery
def low_battery_shutdown():
    global safe_mode

    shutdown_delay = 300 # seconds

    cmd = "wall 'System shutting down in %d seconds'" % shutdown_delay
    os.system(cmd)
    time.sleep(shutdown_delay)
    os.system("halt")

# MAIN -----------------------

# Command Line Arguments
# --log    write time, voltage, etc to a log file
# --debug  write time, voltage, etc to STDOUT

parser = argparse.ArgumentParser(description='Pi Power - Monitor battery status on RasPi projects powered via Adafruit PowerBoost 1000C')

parser.add_argument('-d', '--debug', action='store_true')
parser.add_argument('-l', '--log',   action='store_true')

args = parser.parse_args()

# Vbat to adc #0, Vusb connected to #1
v_bat_adc_pin = 0
v_usb_adc_pin = 1

# Voltage divider drops the PowerBoost voltage from around 5V to under 3.3V which is the limit for the Pi
voltage_divider_r1 =  6800.0
voltage_divider_r2 = 10000.0

# Define the min and max voltage ranges for the inputs

usb_min_voltage = 0.0
usb_max_voltage = 5.2

gpio_min_voltage = 0.0
gpio_max_voltage = 3.3

# LiPo battery voltage range - actual range is 3.7V to 4.2V
# But in practice the measured range is reduced as Vbat always drops from 4.2 to around 4.05 when the
# USB cable is removed - so this is the effective range:

battery_min_voltage = 3.30
battery_max_voltage = 4.15

# this is the effective max voltage, prior to the divider, that the ADC can register
adc_conversion_factor = (gpio_max_voltage / voltage_divider(voltage_divider_r1, voltage_divider_r2, usb_max_voltage)) * usb_max_voltage

pi_power_log_path    = '/var/log/battery_log.csv'

# initialize an empty log file
if(args.log):
        with open(pi_power_log_path, "w") as f:
                f.write('Time,Vbat,Vusb,Frac,Source\n')


# Take a measurement every poll_interval * seconds * - default 60
poll_interval = 5

power_source = ''
power_source_previous = ''

fraction_battery = 1.0

# Define the minimum battery level at which shutdown is triggered

fraction_battery_min = 0.075


if(args.debug):
        print 'Time    Vbat   Vusb   Frac   Source'


elapsed_time = 0
msg = ''

while True:
        # read the analog pins on the ACD (range 0-1023) and convert to 0.0-1.0
        # frac_v_bat = round(readadc(v_bat_adc_pin) * 0.1875) / 1000
        # frac_v_usb = round(readadc(v_usb_adc_pin) * 0.1875) / 1000
        frac_v_bat = round(readadc(v_bat_adc_pin) * 4.096) / 32767
        frac_v_usb = round(readadc(v_usb_adc_pin) * 4.096) / 32767

        # Calculate the true voltage
        v_bat = frac_v_bat * 1.68
        v_usb = frac_v_usb * 1.68


        fraction_battery = (v_bat - battery_min_voltage) / (battery_max_voltage - battery_min_voltage)

        if fraction_battery > 1.0:
               fraction_battery = 1.0
        elif fraction_battery < 0.0:
               fraction_battery = 0.0


        # is the USB cable connected ? Vusb is either 0.0 or around 5.2V
        if v_usb > 1.0:
                power_source = 'usb'
        else:
                power_source = 'battery'

        if power_source == 'usb' and power_source_previous == 'battery':
                print '** USB cable connected'
        elif power_source == 'battery' and power_source_previous == 'usb':
                print '** USB cable disconnected'

        power_source_previous = power_source

        msg = ''
        # If battery is too low then shutdown
        if fraction_battery < fraction_battery_min:
               msg = 'Low Battery - shutdown now'
               ws.send("low battery")
               if(args.debug):
                       print "** LOW BATTERY - shutting down........"
               # shutdown after writing to the log file

        if(args.debug):
                print '{0:6d}  {1:.3f}  {2:.3f}  {3:.3f}  {4:s}  {5:s}'.format(elapsed_time, v_bat, v_usb, fraction_battery, power_source, msg)

        # Open log file, write one line and close
        # This handles the case where the battery is allowed to drain completely and
        # shutdown in which case the file may be corrupted
        if(args.log):
                with open(pi_power_log_path, "a") as f:
                        f.write('{0:d},{1:.3f},{2:.3f},{3:.3f},{4:s},{5:s}\n'.format(elapsed_time, v_bat, v_usb, fraction_battery, power_source, msg))

        ws.send('{0:1s},{1:.3f}'.format(power_source,fraction_battery))
        ws.send('{0:1s},{1:.3f}'.format('voltage',v_bat))

        # Low battery shutdown - specify the time delay in seconds
        if fraction_battery < fraction_battery_min:
                low_battery_shutdown()

        # sleep poll_interval seconds between updates
        time.sleep(poll_interval)

        elapsed_time += poll_interval
