#! /usr/bin/python2.7

import serial
import argparse
import unittest
import sys
import time
from datetime import datetime, timedelta
from astral import *
import timeout_decorator
import pytz

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


class MessagesManager:
    def __init__(self, ser):
        self.ser = ser

        self.resets = []
        self.controls = []
        self.sensors = []
        self.message = ''

    def publish_message(self, message):
        self._process_()
        self.ser.write(message.__str__())
        self.ser.flush()

    def get_reset(self, debug_id):
        return self.pop_element(self.resets, debug_id)

    def get_controls(self, debug_id):
        return self.pop_element(self.controls, debug_id)

    def get_sensors(self, debug_id):
        return self.pop_element(self.sensors, debug_id)

    @timeout_decorator.timeout(20)
    def pop_element(self, collection, debug_id):
        while True:
            if len(collection) == 0:
                self._process_()

            if len(collection) == 0:
                time.sleep(0.05)
                continue

            while len(collection) != 0:
                obj = collection.pop(0)

                if obj.debug_id == debug_id:
                    return obj

    def _process_(self):
        self.message += self.ser.readline()

        messages = list(mess.translate(None, '\n\r^ ') for mess in self.message.split('$'))

        self.message = ''

        #print (messages)

        for idx, mess in enumerate(messages):
            arguments = mess.split(",")

            if len(arguments) == 0:
                continue

            command = arguments[0]

            result = None
            if 'S' == command:
                result = Sensors.parse(arguments)
                if result:
                    self.sensors.append(result)
            elif 'C' == command:
                result = Controls.parse(arguments)
                if result:
                    self.controls.append(result)
            elif 'R' == command:
                result = Reset.parse(arguments)
                if result:
                    self.resets.append(result)

            if not result:
                if idx == len(messages) - 1:
                    self.message = "^%s" % mess
                # Ignore broken first line
                elif idx != 0:
                    raise Exception("Broken Message = %s" % mess)


class Reset:
    def __init__(self, debug_id):
        self.debug_id = int(debug_id)

    def __str__(self):
        return '^R,{}$'.format(self.debug_id)

    @staticmethod
    def parse(raw_args):
        if len(raw_args) != 2:
            return None

        return Reset(*raw_args[1:])


class Sensors:
    def __init__(self, debug_id, date_time, humidity, temperature, soil_moisture, sunrise, sunset):
        self.debug_id = int(debug_id)
        self.date_time = int_to_date(date_time)
        self.humidity = int(humidity)
        self.temperature = int(temperature)
        self.soil_moisture = int(soil_moisture)
        self.sunrise = int_to_date(sunrise)
        self.sunset = int_to_date(sunset)

    def __str__(self):
        return '^S,{},{},{},{},{},{},{}$'.format(self.debug_id, date_to_seconds(self.date_time),
                                   self.humidity, self.temperature, self.soil_moisture,
                                   date_to_seconds(self.sunrise), date_to_seconds(self.sunset))

    @staticmethod
    def parse(raw_args):
        if len(raw_args) != 8:
            return None

        return Sensors(*raw_args[1:])


class Controls:
    def __init__(self, debug_id, water_pump, lamp, humidifier, heater):
        self.debug_id = int(debug_id)
        self.water_pump = bool(int(water_pump))
        self.lamp = bool(int(lamp))
        self.humidifier = bool(int(humidifier))
        self.heater = bool(int(heater))

    def __str__(self):
        return '^C,{},{},{},{},{}$'.format(self.debug_id, int(self.water_pump), int(self.lamp),
                                           int(self.humidifier), int(self.heater))

    @staticmethod
    def parse(raw_args):
        if len(raw_args) != 6:
            return None

        return Controls(*raw_args[1:])


def daterange(start_date, end_date):
    for n in range(int((end_date - start_date).days) / 30):
        yield start_date + timedelta(n * 30)

def date_to_seconds(value):
    return (value.replace(tzinfo=None) - datetime(1970, 1, 1)).total_seconds()

def int_to_date(value):
    if isinstance(value, datetime):
        return value

    return datetime.fromtimestamp(int(value), tz=pytz.utc).replace(tzinfo=None)

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
