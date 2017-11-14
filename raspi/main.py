from raspi.arduino_comm import ArduinoComm
from raspi.puzzles.start import StartPrompt
from raspi.puzzles.drawer import DrawerPuzzle
from raspi.puzzles.birthday_paradox import BirthdayParadoxPuzzle
from raspi.puzzles.bunker_hill_monument import BunkerHillMonumentPuzzle
from raspi.available_io import *
import time


class BOCSMain:

    do_run = True
    event_callback = None
    current_puzzle = None
    current_puzzle_index = 0

    outputs = {}

    def __init__(self):
        # Initialize stuff
        self.state = BOCSState(BOCSState.INITIALIZING)
        ArduinoComm(self.event_callback, self.register_arduino, '/dev/ttyACM0')
        # self.outputs[DRAWER] = ArduinoComm(self.event_callback, '/dev/ttyACM1')
        self.puzzles = [StartPrompt, BirthdayParadoxPuzzle, BunkerHillMonumentPuzzle, DrawerPuzzle]
        while len(self.outputs) < 1:
            pass  # Wait for the Arduino to connect

        # Run the puzzles!
        self.state.phase = BOCSState.RUNNING
        self.current_puzzle = self.puzzles[0](self.update_io_state, self.register_callback)
        self.run_puzzles()

    def run_puzzles(self):
        while not self.state.phase == BOCSState.STOPPING:
            if self.current_puzzle.is_solved:
                # Deregister event callback
                self.event_callback = None

                time.sleep(5)  # Pause for 5 seconds to show message before proceeding to next puzzle

                # Increment the index to the next puzzle
                self.current_puzzle_index += 1
                # Check if the last puzzle was just finished
                if self.current_puzzle_index < len(self.puzzles):
                    puzzle_class = self.puzzles[self.current_puzzle_index]
                    self.current_puzzle = puzzle_class(self.update_io_state, self.register_callback)
                else:
                    self.state.phase = BOCSState.STOPPING
            time.sleep(0.005)  # Sleep for 50ms

    def shutdown(self):
        # Stop event listener threads
        for output in self.outputs.values():
            output.stop_listening()

    def update_io_state(self, io_name, new_state):
        if io_name not in self.outputs:
            raise ValueError('Specified IO name "{}" is not a registered IO device.'.format(io_name))

        # Send the new state to the Arduino
        json_str = new_state.serialize()
        self.outputs[io_name].send_data(json_str)

        # Save the state, just in case we want to know what it is
        self.state.io_states[io_name] = new_state

    def register_arduino(self, comm, device_name):
        self.outputs[device_name] = comm

    def event_callback(self, event):
        if self.event_callback:
            self.event_callback(event)

    def register_callback(self, callback):
        self.event_callback = callback

    # def unregister_callback(self, event_type, callback):
    #     if event_type in self.event_callbacks and callback in self.event_callbacks[event_type]:
    #         self.event_callbacks[event_type].remove(callback)


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
    bocs = BOCSMain()
    bocs.shutdown()
