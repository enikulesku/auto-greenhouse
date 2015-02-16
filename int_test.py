#! /usr/bin/python2.7

import serial
import argparse
import unittest
import sys
from datetime import datetime, timedelta
from astral import *
from test_util import *

class AutoGreenhouseTest(unittest.TestCase):
    baudrate = 0
    device = ''

    @classmethod
    def setUpClass(cls):
        cls.ser = serial.Serial(cls.device, cls.baudrate, timeout=0.05)
        cls.messages = MessagesManager(cls.ser)

    @classmethod
    def tearDownClass(cls):
        cls.ser.close()

    def setUp(self):
        self.debug_id = 0

        debug_id = self.next_debug_id()

        self.messages = AutoGreenhouseTest.messages
        self.messages.publish_message(Reset(debug_id))
        self.messages.get_reset(debug_id)

    def next_debug_id(self):
        self.debug_id += 1
        return self.debug_id

    def test_datetime_sunrise_sunset(self):
        location = Location(('Odessa', 'Ukraine', 46.5, 30.77, 'UTC'))

        for single_date in daterange(datetime(2015, 1, 1), datetime(2016, 1, 1)):
            debug_id = self.next_debug_id()

            sun = location.sun(local=False, date=single_date)

            expected = Sensors(debug_id, single_date, 0, 0, 0, sun['sunrise'].replace(tzinfo=None), sun['sunset'].replace(tzinfo=None))
            self.messages.publish_message(expected)

            actual = self.messages.get_sensors(debug_id)

            self.assertAlmostEqual(msg="dateTime", first=expected.date_time, second=actual.date_time, delta=timedelta(minutes=1))

            self.assertAlmostEqual(msg="sunrise", first=expected.sunrise, second=actual.sunrise, delta=timedelta(minutes=20))
            self.assertAlmostEqual(msg="sunset", first=expected.sunset, second=actual.sunset, delta=timedelta(minutes=20))

            self.messages.get_controls(debug_id) #just to flush

    def test_stub(self):
        self.assertTrue(True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Auto-Greenhouse Integration Test')
    parser.add_argument('-b', '--baudrate', type=int, default=115200)
    parser.add_argument('-D', '--device', default='/dev/ttyACM0')

    args = parser.parse_args()

    AutoGreenhouseTest.baudrate = args.baudrate
    AutoGreenhouseTest.device = args.device

    del sys.argv[:]
    sys.argv.append("AutoGreenhouseTest")

    unittest.main()
