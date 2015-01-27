#! /usr/bin/python2.7

import serial
import argparse

parser = argparse.ArgumentParser(description='Auto-Greenhouse Integration Test')
parser.add_argument('-b', '--baudrate', type=int, default=9600)
parser.add_argument('-D', '--device', default='/dev/ttyACM0')

args = parser.parse_args()

ser = serial.Serial(args.device, args.baudrate, timeout=1)

//ToDo: put test cases here
x = ser.read()          # read one byte
s = ser.read(10)        # read up to ten bytes (timeout)
line = ser.readline()   # read a '\n' terminated line
ser.close()
