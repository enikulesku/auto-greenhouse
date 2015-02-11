#! /usr/bin/python2.7

import serial
import argparse
import unittest
import sys
import time
from datetime import datetime
#import timeout_decorator

class AutoGreenhouseTest(unittest.TestCase):
    baudrate = 0
    device = ''

    @classmethod
    def setUpClass(cls):
        cls.ser = serial.Serial(cls.device, cls.baudrate, timeout=1)
        cls.messages = MessagesManager(cls.ser)

    @classmethod
    def tearDownClass(cls):
        cls.ser.close()

    def setUp(self):
        self.debug_id = 0

        self.messages = AutoGreenhouseTest.messages
        self.messages.publish_message(Reset(self.debug_id))
        self.messages.get_reset(self.debug_id)

    def tearDown(self):
        #ToDo: send reset
        print "tearDown"

    def test_stub(self):
        #ToDo: put test here

        self.assertTrue(True)


class MessagesManager:
    def __init__(self, ser):
        self.ser = ser

        self.resets = []
        self.controls = []
        self.sensors = []
        self.message = ''

    def publish_message(self, message):
        self.ser.write(message)

    def get_reset(self, debug_id):
        return self.pop_element(self.resets, debug_id)

    def get_controls(self, debug_id):
        return self.pop_element(self.controls, debug_id)

    def get_sensors(self, debug_id):
        return self.pop_element(self.sensors, debug_id)

    #@timeout_decorator.timeout(5)
    def pop_element(self, collection, debug_id):
        while True:
            if len(collection) == 0:
                self._process_()

            if len(collection) == 0:
                time.sleep(0.1)
                continue

            while len(collection) != 0:
                obj = collection.pop(0)

                if obj.debug_id == debug_id:
                    return obj

    def _process_(self):
        if ser.inWaiting() == 0:
            return

        self.message = ser.read(ser.inWaiting())

        messages = list(mess.translate(None, '\n\r^ ') for mess in self.message.split('$'))

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
    def __init__(self, debug_id, date_time, sunrise, sunset, humidity, temperature, soil_moisture):
        self.debug_id = int(debug_id)
        self.date_time = Sensors.int_to_date(date_time)
        self.sunrise = Sensors.int_to_date(sunrise)
        self.sunset = Sensors.int_to_date(sunset)
        self.humidity = int(humidity)
        self.temperature = int(temperature)
        self.soil_moisture = int(soil_moisture)

    def __str__(self):
        return '^S,{},{},{},{},{}$'.format(self.debug_id, self.date_time.total_seconds(), self.humidity,
                                           self.temperature, self.soil_moisture)

    @staticmethod
    def int_to_date(value):
        if isinstance(value, datetime):
            return value

        return datetime.fromtimestamp(int(value))

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
