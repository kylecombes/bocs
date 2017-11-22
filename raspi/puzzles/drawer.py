from raspi.arduino_comm import ArduinoCommEventType as EventType
from raspi.io_states.lcd_state import LCDState
from raspi.available_io import *


class DrawerPuzzle:

    PROMPT = "Put something in the drawer."
    is_solved = False

    def __init__(self, update_io_state, register_callback):
        # Set the initial states of the inputs and outputs we'll be using
        self.lcd_state = LCDState(self.PROMPT)
        update_io_state(ARDUINO1, self.lcd_state)
        self.update_io_state = update_io_state

        # Subscribe to input events
        register_callback(self.event_received)

    def event_received(self, event):
        if event.id == EventType.DRAWER_STATE_CHANGE:
            if event.data == '1':  # Drawer open
                self.lcd_state.set_line(LCDState.TOP_LINE, "Place an item and close the drawer")
            elif event.data == '0':  # Drawer closed
                if event.options:
                    self.lcd_state.set_line(LCDState.TOP_LINE, "Object detected with height of {} cm".format(event.options))
                else:
                    self.lcd_state.set_line(LCDState.TOP_LINE, "Put in an object, ya dweeb")

            self.update_io_state(ARDUINO1, self.lcd_state)
