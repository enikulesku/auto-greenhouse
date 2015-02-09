#! /usr/bin/python2.7

import serial
import argparse
import unittest
import sys
import time

class AutoGreenhouseTest(unittest.TestCase):
    baudrate = 0
    device = ''

    @classmethod
    def setUpClass(cls):
        self.ser = serial.Serial(self.device, self.baudrate, timeout=1)

    @classmethod
    def tearDownClass(cls):
        self.ser.close()

    def setUp(self):
        #ToDo: send reset
        print "setUp"

    def tearDown(self):
        #ToDo: send reset
        print "tearDown"

    def test_stub2(self):
        print "test2"

    def test_stub(self):


        ## ToDo: put test cases here
        #x = ser.read()          # read one byte
        #s = ser.read(10)        # read up to ten bytes (timeout)
        #line = ser.readline()   # read a '\n' terminated line

        ser.write("^R,1$")
        line = ser.readline()
        print (line)

        i = 2
        while(True):
            ser.write("^S,{},1423493380,35,22,800$".format(i))

            line = ser.readline()
            print (line)
            line = ser.readline()
            print (line)
            i += 1
            #ToDo: use inWaiting
            #if ser.inWaiting() > 0:
            #    i += 1
            #    msg = ser.read(ser.inWaiting()) # read everything in the input buffer
            #    print ("Message from arduino: ")
            #    print (msg)

            time.sleep(0.1)



        print "test"

        self.assertTrue(True)


class Sensors:
    def __init__(self, debug_id, date_time, sunrise, sunset, humidity, temperature, soil_moisture):
        self.debug_id = debug_id
        self.date_time = date_time
        self.sunrise = sunrise
        self.sunset = sunset
        self.humidity = humidity
        self.temperature = temperature
        self.soil_moisture = soil_moisture


if __name__ == '__main__':
    if len(sys.argv) > 1:
        parser = argparse.ArgumentParser(description='Auto-Greenhouse Integration Test')
        parser.add_argument('-b', '--baudrate', type=int, default=9600)
        parser.add_argument('-D', '--device', default='/dev/ttyACM0')

        args = parser.parse_args()

        AutoGreenhouseTest.baudrate = args.baudrate
        AutoGreenhouseTest.device = args.device

        del sys.argv[:]
        sys.argv.append("AutoGreenhouseTest")

    unittest.main()
