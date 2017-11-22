from raspi.arduino_comm import ArduinoCommEventType as EventType
from raspi.io_states.lcd_state import LCDState
from raspi.available_io import *


class BirthdayParadoxPuzzle:

    PROMPT = "How many people are needed in a room for the probability that 2 have the " \
            "same birthday to be greater than 50%?"
    LINE_2_PREFIX = "Input: "
    guess = ""
    ANSWER = "23"
    is_solved = False

    def __init__(self, update_io_state, register_callback):
        # Set the initial states of the inputs and outputs we'll be using
        self.lcd_state = LCDState(self.PROMPT, self.LINE_2_PREFIX)
        update_io_state(LCD_1, self.lcd_state)
        self.update_io_state = update_io_state

        # Subscribe to input events
        register_callback(self.key_pressed)

    def key_pressed(self, event):
        if event.id == EventType.NUMERIC_KEYPAD_PRESS:
            key = event.data
            if key == '#':  # Submit
                if self.guess == self.ANSWER:
                    line2 = "Correct!"
                    self.is_solved = True
                else:
                    line2 = "Sorry, that's incorrect"
            else:
                if key == '*':  # Backspace
                    self.guess = self.guess[0:-1]
                else:  # Digit entry
                    self.guess += str(key)

                line2 = self.LINE_2_PREFIX + self.guess

            self.lcd_state.set_line(LCDState.BOTTOM_LINE, line2)
            self.update_io_state(ARDUINO1, self.lcd_state)
