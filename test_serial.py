#! /usr/bin/python2.7

import serial

ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
x = ser.read()          # read one byte
s = ser.read(10)        # read up to ten bytes (timeout)
line = ser.readline()   # read a '\n' terminated line
ser.close()