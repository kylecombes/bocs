from raspi.arduino_comm_event import ArduinoCommEventType as EventType
from raspi.io_states.lcd_2_line_state import LCD2LineState
from raspi.available_io import *


class BirthdayParadoxPuzzle:

    PROMPT = "How many 3 are needed ?"
    LINE_2_PREFIX = "Input: "
    guess = ""
    ANSWER = "23"
    is_solved = False

    def __init__(self, update_io_state, register_callback):
        # Set the initial states of the inputs and outputs we'll be using
        self.io_state = LCD2LineState(self.PROMPT, self.LINE_2_PREFIX)
        update_io_state(LCD_1, self.io_state)
        self.update_io_state = update_io_state

        # Subscribe to numeric keypad button press events
        register_callback(EventType.NUMERIC_KEYPAD_PRESS, self.key_pressed)

    def key_pressed(self, event):
        key = event.data
        if key == -13:  # Pound sign (ASCII shifted by 48)
            if self.guess == self.ANSWER:
                print("Correct!")
                self.is_solved = True
            else:
                print("Sorry, that's incorrect")
        else:
            self.guess += str(key)
            self.io_state.set_line(LCD2LineState.BOTTOM_LINE, self.LINE_2_PREFIX + self.guess)
            self.update_io_state(LCD_1, self.io_state)
