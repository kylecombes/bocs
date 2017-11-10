from raspi.arduino_comm_event import ArduinoCommEventType as EventType
from raspi.io_states.lcd_state import LCDState
from raspi.available_io import *


class StartPrompt:

    PROMPT = "Press the button to start"
    is_solved = False

    def __init__(self, update_io_state, register_callback):
        # Set the initial states of the inputs and outputs we'll be using
        self.lcd_state = LCDState(self.PROMPT)
        update_io_state(LCD_1, self.lcd_state)
        self.update_io_state = update_io_state

        # Subscribe to input events
        register_callback(self.key_pressed)

    def key_pressed(self, event):
        if event.id == EventType.START_BUTTON_PRESS:
            # TODO Shut off light
            self.is_solved = True
