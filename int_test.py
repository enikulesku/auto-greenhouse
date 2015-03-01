#! /usr/bin/python2.7

import argparse
import unittest
import sys
from datetime import datetime, timedelta
from astral import *
from test_util import *

DRY_SOIL = 50
MOISTURED_SOIL = 20

MAX_WORKING_TIME = timedelta(minutes=1)
MIN_WORKING_DELAY = timedelta(minutes=10)
EXPECTED_DAY_DURATION = timedelta(hours=12)

class AutoGreenhouseTest(unittest.TestCase):
    baudrate = 0
    device = ''

    @classmethod
    def setUpClass(cls):
        cls.messages = MessagesManager(cls.device, cls.baudrate)

    @classmethod
    def tearDownClass(cls):
        cls.messages.close()

    def setUp(self):
        self.debug_id = 0

        self.messages = AutoGreenhouseTest.messages
        self.messages.echo_message(Reset(self.next_debug_id()))

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

    def test_water_pump_by_value_and_max_delay(self):
        actual, controls = self.messages.echo_message(Sensors(self.next_debug_id(), date_time=datetime(2015, 1, 1, second=0), soil_moisture=MOISTURED_SOIL))
        self.assertFalse(controls.water_pump)

        actual, controls = self.messages.echo_message(Sensors(self.next_debug_id(), date_time=datetime(2015, 1, 1, second=1), soil_moisture=DRY_SOIL))
        self.assertTrue(controls.water_pump)

        actual, controls = self.messages.echo_message(Sensors(self.next_debug_id(), date_time=datetime(2015, 1, 1, second=2), soil_moisture=MOISTURED_SOIL))
        self.assertFalse(controls.water_pump)

        # Ignored by max delay
        actual, controls = self.messages.echo_message(Sensors(self.next_debug_id(), date_time=datetime(2015, 1, 1, second=3), soil_moisture=DRY_SOIL))
        self.assertFalse(controls.water_pump)

        actual, controls = self.messages.echo_message(Sensors(self.next_debug_id(), date_time=datetime(2015, 1, 1, second=4) + MIN_WORKING_DELAY, soil_moisture=DRY_SOIL))
        self.assertTrue(controls.water_pump)

    def test_water_pump_by_timeout(self):
        initial_date = datetime(2015, 1, 1)
        max_work_time = MAX_WORKING_TIME
        allowed_delta = timedelta(seconds=10)

        actual, controls = self.messages.echo_message(Sensors(self.next_debug_id(), date_time=initial_date, soil_moisture=DRY_SOIL))
        self.assertTrue(controls.water_pump)

        current_date = initial_date
        for s in range(0, max_work_time.seconds + allowed_delta.seconds, 15):
            current_date = initial_date + timedelta(seconds=s)
            actual, controls = self.messages.echo_message(Sensors(self.next_debug_id(), date_time=current_date, soil_moisture=DRY_SOIL))

            if not controls.water_pump:
                break

        self.assertAlmostEqual(msg="turn off by timeout", first=initial_date + max_work_time, second=current_date, delta=allowed_delta)

    def test_lamp(self):
        initial_date = datetime(2015, 1, 1)
        location = Location(('Odessa', 'Ukraine', 46.5, 30.77, 'UTC'))
        sun = location.sun(local=False, date=initial_date)

        sunrise_date = sun['sunrise'].replace(tzinfo=None)
        sunset_date = sun['sunset'].replace(tzinfo=None)

        sun_day_duration = sunset_date - sunrise_date
        expected_day_duration = EXPECTED_DAY_DURATION

        expected_sunrise = sunrise_date - (expected_day_duration - sun_day_duration) / 2
        expected_sunset = sunset_date + (expected_day_duration - sun_day_duration) / 2

        delta = timedelta(minutes=15)

        actual, controls = self.messages.echo_message(Sensors(self.next_debug_id(), date_time=expected_sunrise - delta))
        self.assertFalse(controls.lamp)

        actual, controls = self.messages.echo_message(Sensors(self.next_debug_id(), date_time=expected_sunrise + delta))
        self.assertTrue(controls.lamp)

        actual, controls = self.messages.echo_message(Sensors(self.next_debug_id(), date_time=sunrise_date - delta))
        self.assertTrue(controls.lamp)

        actual, controls = self.messages.echo_message(Sensors(self.next_debug_id(), date_time=sunrise_date + delta))
        self.assertFalse(controls.lamp)

        actual, controls = self.messages.echo_message(Sensors(self.next_debug_id(), date_time=sunset_date - delta))
        self.assertFalse(controls.lamp)

        actual, controls = self.messages.echo_message(Sensors(self.next_debug_id(), date_time=sunset_date + delta))
        self.assertTrue(controls.lamp)

        actual, controls = self.messages.echo_message(Sensors(self.next_debug_id(), date_time=expected_sunset - delta))
        self.assertTrue(controls.lamp)

        actual, controls = self.messages.echo_message(Sensors(self.next_debug_id(), date_time=expected_sunset + delta))
        self.assertFalse(controls.lamp)


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
