from raspi.io_states.keypad_state import KeypadState

from raspi.puzzles.puzzle import BOCSPuzzle
from raspi.arduino_comm import ArduinoCommEventType as EventType
import time
from raspi.available_io import *


class BunkerHillMonumentPuzzle(BOCSPuzzle):

    PROMPT = "How many opportunities to trip does one face when attempting to view Boston from historic heights?"
    LINE_2_PREFIX = "Input: "
    guess = ""
    ANSWER = "294"

    def __init__(self, init_bundle, register_callback):
        BOCSPuzzle.__init__(self, init_bundle)
        # Set the initial states of the inputs and outputs we'll be using
        self.eink.set_text('{}\n\n{}'.format(self.PROMPT, self.LINE_2_PREFIX))

        # Subscribe to numeric keypad button press events
        register_callback(self.key_pressed)

        # Show keypad
        self.keypad_state = KeypadState(visible=True)
        self.update_io_state(ARDUINO1, self.keypad_state)

    def key_pressed(self, event):

        if event.id == EventType.NUMERIC_KEYPAD_PRESS:

            key = event.data
            pause = 0
            if key == '#':  # Submit
                if self.guess == self.ANSWER:
                    line2 = "Correct!"
                    pause = 5  # Pause for 5 seconds to show message before proceeding to next puzzle
                    self.is_solved = True
                else:
                    line2 = "Sorry, that's incorrect"
            else:
                if key == '*':  # Backspace
                    self.guess = self.guess[0:-1]
                else:  # Digit entry
                    self.guess += str(key)

                line2 = self.LINE_2_PREFIX + self.guess

            self.eink.set_text('{}\n\n{}'.format(self.PROMPT, line2))
            time.sleep(pause)

