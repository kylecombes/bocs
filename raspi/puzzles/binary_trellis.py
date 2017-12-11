from raspi.puzzles.puzzle import BOCSPuzzle
from raspi.io_states.trellis_state import TrellisState, TrellisLightConfig
from raspi.arduino_comm import ArduinoCommEventType as EventType
from raspi.available_io import *


class BinaryTrellis(BOCSPuzzle):

    def __init__(self, init_bundle, register_callback):
        BOCSPuzzle.__init__(self, init_bundle)

        self.trellis_state = TrellisState(False)
        self.trellis_light_config = TrellisLightConfig()
        self.trellis_state.add_blink_frame(self.trellis_light_config, 10)

        register_callback(self.input_event)

    def input_event(self, event):

        if event.id == EventType.TRELLIS_BUTTON_PRESS:

            if event.data != '0':

                print(event.data)
                self.trellis_light_config.set_led_on(int(event.data), True)
                self.trellis_state.clear_pattern()
                self.trellis_state.add_blink_frame(self.trellis_light_config, 10)
                self.update_io_state(ARDUINO1, self.trellis_state)
