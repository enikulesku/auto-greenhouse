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
        self.messages.echo_message(Reset(debug_id))

    def next_debug_id(self):
        self.debug_id += 1
        return self.debug_id

    def test_datetime_sunrise_sunset(self):
        location = Location(('Odessa', 'Ukraine', 46.5, 30.77, 'UTC'))

        for single_date in daterange(datetime(2015, 1, 1), datetime(2016, 1, 1)):
            debug_id = self.next_debug_id()

            sun = location.sun(local=False, date=single_date)

            expected = Sensors(debug_id, date_time=single_date, sunrise=sun['sunrise'].replace(tzinfo=None), sunset=sun['sunset'].replace(tzinfo=None))
            actual, controls = self.messages.echo_message(expected)

            self.assertAlmostEqual(msg="dateTime", first=expected.date_time, second=actual.date_time, delta=timedelta(minutes=1))

            self.assertAlmostEqual(msg="sunrise", first=expected.sunrise, second=actual.sunrise, delta=timedelta(minutes=20))
            self.assertAlmostEqual(msg="sunset", first=expected.sunset, second=actual.sunset, delta=timedelta(minutes=20))

    def test_water_pump_by_value(self):
        actual, controls = self.messages.echo_message(Sensors(date_time=datetime(2015, 1, 1, second=0), soil_moisture=10))
        self.assertFalse(controls.water_pump)

        actual, controls = self.messages.echo_message(Sensors(date_time=datetime(2015, 1, 1, second=1), soil_moisture=1000))
        self.assertTrue(controls.water_pump)

        actual, controls = self.messages.echo_message(Sensors(date_time=datetime(2015, 1, 1, second=2), soil_moisture=10))
        self.assertFalse(controls.water_pump)


    def test_water_pump_by_timeout(self):
        initial_date = datetime(2015, 1, 1)
        max_work_time = timedelta(minutes=2)
        allowed_delta = timedelta(seconds=10)

        actual, controls = self.messages.echo_message(Sensors(date_time=initial_date, soil_moisture=1000))
        self.assertTrue(controls.water_pump)

        current_date = initial_date
        for s in range(0, max_work_time.seconds + allowed_delta.seconds):
            current_date = initial_date + timedelta(seconds=s)
            actual, controls = self.messages.echo_message(Sensors(date_time=current_date, soil_moisture=1000))

            if not controls.water_pump:
                break

        self.assertAlmostEqual(msg="turn off by timeout", first=initial_date + max_work_time, second=current_date, delta=allowed_delta)


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
