from datetime import datetime, timedelta
import timeout_decorator
import pytz
import time

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

    def echo_message(self, obj):
        self._process_()
        self.publish_message(obj)

        if isinstance(obj, Reset):
            return self.pop_element(self.resets, obj.debug_id)
        elif isinstance(obj, Sensors):
            return (self.pop_element(self.sensors, obj.debug_id),
                    self.pop_element(self.controls, obj.debug_id))

        return None

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

        print (messages)

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
    def __init__(self, debug_id, date_time=None, humidity=0, temperature=0, soil_moisture=0, sunrise=None, sunset=None):
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
    def __init__(self, debug_id, water_pump=False, lamp=False, humidifier=False, heater=False):
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
    if value is None:
        return 0

    return (value.replace(tzinfo=None) - datetime(1970, 1, 1)).total_seconds()

def int_to_date(value):
    if value is None or isinstance(value, datetime):
        return value

    return datetime.fromtimestamp(int(value), tz=pytz.utc).replace(tzinfo=None)
