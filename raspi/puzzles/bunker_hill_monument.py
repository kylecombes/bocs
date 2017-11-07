from raspi.arduino_comm_event import ArduinoCommEventType as EventType
from raspi.io_states.lcd_state import LCDState
from raspi.available_io import *
import time


class BunkerHillMonumentPuzzle:

    PROMPT = "How many opportunities to trip does one face when attempting to view Boston from historic heights?"
    LINE_2_PREFIX = "Input: "
    guess = ""
    ANSWER = "294"
    is_solved = False

    def __init__(self, update_io_state, register_callback):
        # Set the initial states of the inputs and outputs we'll be using
        self.lcd_state = LCDState(self.PROMPT, self.LINE_2_PREFIX)
        update_io_state(LCD_1, self.lcd_state)
        self.update_io_state = update_io_state

        # Subscribe to numeric keypad button press events
        register_callback(self.key_pressed)

    def key_pressed(self, event):

        if event.id == EventType.NUMERIC_KEYPAD_PRESS:
            # if len(self.guess) == 0:
            #     return

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

            self.lcd_state.set_line(LCDState.BOTTOM_LINE, line2)
            self.update_io_state(LCD_1, self.lcd_state)
            time.sleep(pause)
