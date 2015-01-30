#! /usr/bin/python2.7

import serial
import argparse
import unittest
import sys

class AutoGreenhouseTest(unittest.TestCase):
    baudrate = 0
    device = ''

    @classmethod
    def setUpClass(cls):
        print "setUpClass"

    @classmethod
    def tearDownClass(cls):
        print "tearDownClass"

    def setUp(self):
        print "setUp"

    def tearDown(self):
        print "tearDown"

    def test_stub2(self):
        print "test2"

    def test_stub(self):
        #ser = serial.Serial(self.device, self.baudrate, timeout=1)

        ## ToDo: put test cases here
        #x = ser.read()          # read one byte
        #s = ser.read(10)        # read up to ten bytes (timeout)
        #line = ser.readline()   # read a '\n' terminated line
        #ser.close()

        print "test"

        self.assertTrue(True)

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
