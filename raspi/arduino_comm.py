from serial import Serial
from threading import Thread
from datetime import datetime, timedelta
import json
import time
from raspi.arduino_comm_event import ArduinoCommEvent

"""
This module handles all communication with an Arduino.
If communication with multiple Arduinos is needed, multiple ArduinoComm instances can be created.
"""


class ArduinoComm:

    thread = None

    """
    Creates a new instance of ArduinoComm.
    :param event_callback (function) a callback to use when something is received from the Arduino
    :param port (string) the serial port to connect to
    :param baudrate (int) the baudrate to use (defaults to 9600)
    """
    def __init__(self, event_callback, register_callback, port='/dev/ttyACM0', baudrate=9600):
        self.event_callback = event_callback
        self.register_callback = register_callback
        self.port = port
        self.baudrate = baudrate
        self.cxn = Serial(self.port, baudrate=self.baudrate)

        self.start_listening()

    def start_listening(self):
        self.thread = ArduinoCommThread(self.cxn, self.event_callback, self.register_callback)
        self.thread.setName('ArduinoCommThread for {}'.format(self.port))
        self.thread.start()

    def stop_listening(self):
        self.thread.stop()

    def send_data(self, data):
        if self.cxn.is_open:
            self.cxn.write((data + '\n').encode('utf-8'))
            return True
        else:
            return False


DISCONNECTED = 0
CONNECTING = 1
CONNECTED = 2


class ArduinoCommThread(Thread):

    do_run = True
    state = DISCONNECTED
    device_name = None

    def __init__(self, cxn, event_callback, register_callback, connection_timeout=1000):
        Thread.__init__(self)
        self.cxn = cxn
        self.event_callback = event_callback
        self.register_callback = register_callback
        self.abort_time = datetime.now() + timedelta(milliseconds=connection_timeout)

    def run(self):

        while self.do_run and (self.state == CONNECTED or self.abort_time > datetime.now()):
            while self.cxn.inWaiting() > 1:
                data = self.cxn.readline()
                if data:  # Make sure the data is valid before trying to parse it
                    try:
                        data = data.decode('UTF-8')[0:-2]
                        if self.state == DISCONNECTED:  # Handshake not completed yet
                            if data.startswith('Hello from '):
                                self.device_name = data[11:]
                                self.cxn.write(b'Hello from computer')
                                self.abort_time += timedelta(milliseconds=10000)  # Give 10 seconds to complete
                                self.state = CONNECTING

                        elif self.state == CONNECTING:
                            if data == "Connected":
                                self.state = CONNECTED
                                self.register_callback(self, self.device_name)
                                print("Connected to " + self.device_name)
                            else:
                                self.cxn.write(b'Hello from computer')
                                print("Connecting to " + self.device_name)
                        else:
                            self.process_event(data)
                    except UnicodeDecodeError:
                        pass
            # self.cxn.flushInput()
            time.sleep(0.05)

    """
    Handles deserializing the event data and calling the event callback.
    """
    def process_event(self, raw_data):

        # Parse the JSON into a dictionary
        try:
            raw_data = json.loads(raw_data)
        except json.decoder.JSONDecodeError:
            print('WARNING: Invalid event data format received:\n{}\n'.format(raw_data))
            return False

        # Check to make sure the necessary attributes are present
        if not raw_data:
            return False

        event_id = raw_data.get('event_id', None)
        if not event_id and event_id != 0:
            return False

        # Get the data, if there is any
        data = raw_data.get('data', None)

        # Get the options, if there are any
        options = raw_data.get('options', None)

        # Create an event object
        event = ArduinoCommEvent(int(event_id), data, options)

        # Pass the event to the callback
        if self.event_callback:
            self.event_callback(event)

    def stop(self):
        self.do_run = False
