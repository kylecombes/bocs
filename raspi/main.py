from serial.tools.list_ports import comports
from os import environ
from raspi.arduino_comm import ArduinoComm
from raspi.puzzles.binary_trellis import BinaryTrellis
from raspi.puzzles.forty_two import FortyTwoPuzzle
from raspi.puzzles.frequency_puzzle import FrequencyPuzzle
from raspi.puzzles.start import StartPrompt
from raspi.puzzles.nothing_puzzle import NothingPuzzle
from raspi.puzzles.birthday_paradox import BirthdayParadoxPuzzle
from raspi.puzzles.bunker_hill_monument import BunkerHillMonumentPuzzle
from raspi.telemetry.server_comm import ServerComm
from raspi.available_io import *
import time


class BOCSMain:

    do_run = True
    event_callback = None
    current_puzzle = None
    current_puzzle_index = 0
    future_arduino_states = {}  # State updates are queued here until the given Arduino is connected

    outputs = {}

    def __init__(self, telemetry_server=None, sound_server=None):
        # Initialize stuff
        self.state = BOCSState(BOCSState.INITIALIZING)
        available_ports = BOCSMain.get_available_serial_ports()
        for port in available_ports:
            # Try to register each available port as an Arduino
            try:
                ArduinoComm(self.event_fired, self.register_arduino, self.deregister_arduino, port)
            except Exception:
                pass  # Probably not an Arduino connected at this port

        # Connect to the stat/monitoring server
        self.telemetry_server = ServerComm(telemetry_server, True) if telemetry_server else None

        # Connect to the sound-playing server, if desired
        self.sound_server = ServerComm(sound_server) if sound_server else None

        self.puzzles = [StartPrompt, BirthdayParadoxPuzzle, BunkerHillMonumentPuzzle, FrequencyPuzzle, NothingPuzzle]

        # Run the puzzles!
        self.state.phase = BOCSState.RUNNING
        self.puzzle_init_bundle = {
            'update_io_state': self.update_io_state,
            'telemetry_server': self.telemetry_server,
            'sound_server': self.sound_server
        }
        self.current_puzzle = self.puzzles[0](self.puzzle_init_bundle, self.register_callback)
        self.run_puzzles()

    def run_puzzles(self):
        while not self.state.phase == BOCSState.STOPPING:
            if self.current_puzzle.is_solved:
                # Deregister event callback
                self.event_callback = None

                # if type(self.current_puzzle) != self.puzzles[0]:
                #     time.sleep(2)  # Pause for 5 seconds to show message before proceeding to next puzzle

                # Increment the index to the next puzzle
                self.current_puzzle_index += 1
                # Check if the last puzzle was just finished
                if self.current_puzzle_index < len(self.puzzles):
                    puzzle_class = self.puzzles[self.current_puzzle_index]
                    self.current_puzzle = puzzle_class(self.puzzle_init_bundle, self.register_callback)
                else:
                    self.state.phase = BOCSState.STOPPING
            time.sleep(0.005)  # Sleep for 5ms

    def shutdown(self):
        # Stop event listener threads
        for output in self.outputs.values():
            output.stop_listening()

    def update_io_state(self, io_name, new_state):
        if io_name in self.outputs:  # Desired Arduino is currently connected
            self.transmit_io_state_update(io_name, new_state)
        else:  # Desired Arduino not currently connected, so save message to be sent upon connection
            if io_name not in self.future_arduino_states:  # First attempt to update this particular Arduino's state
                self.future_arduino_states[io_name] = []  # Initialize new empty queue
            self.future_arduino_states[io_name].append(new_state)  # Queue the update
        
    def transmit_io_state_update(self, io_name, new_state):

        # Send the new state to the Arduino
        msg = new_state.get_arduino_message()
        print('Transmitting {} to {}'.format(msg, io_name))
        self.outputs[io_name].send_data(msg)

        # Save the state, just in case we want to know what it is
        self.state.io_states[io_name] = new_state

    def register_arduino(self, comm, device_name):
        self.outputs[device_name] = comm

        # Check to see if there is a state update waiting to be sent to this Arduino
        if device_name in self.future_arduino_states:
            for update in self.future_arduino_states[device_name]:
                self.transmit_io_state_update(device_name, update)
            del self.future_arduino_states[device_name]

    def deregister_arduino(self, device_name):
        del self.outputs[device_name]

    def event_fired(self, event):
        if self.event_callback:
            self.event_callback(event)

    def register_callback(self, callback):
        self.event_callback = callback

    @staticmethod
    def get_available_serial_ports():
        """
        Gets all available /dev/ttyACM# COM ports.
        """
        return [port.device for port in comports() if port.name.startswith('ttyACM') or port.name.startswith('ttyUSB')]


class BOCSState:

    # Define phases
    INITIALIZING = 0
    RUNNING = 1
    LOADING_NEXT_PUZZLE = 2
    PAUSED = 3
    STOPPING = 4

    def __init__(self, phase):
        self.phase = phase

    io_states = {}

    def set_io_state(self, class_name, phase):
        self.io_states[class_name] = phase


if __name__ == '__main__':
    telemetry_server = environ.get('BOCS_TELEMETRY_SERVER', False)
    sound_server = environ.get('BOCS_SOUND_SERVER', False)
    bocs = BOCSMain(telemetry_server, sound_server)
    bocs.shutdown()
