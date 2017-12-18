from raspi.io_states.cube_state import CubeState
from raspi.puzzles.puzzle import BOCSPuzzle
from raspi.io_states.trellis_state import TrellisState, TrellisLightConfig
from raspi.arduino_comm import ArduinoCommEventType as EventType
from raspi.available_io import *


class BinaryTrellis(BOCSPuzzle):

    PUZZLE_ID = 'Binary'

    ANSWER = 10
    LIGHT_CONFIG_OFF = TrellisLightConfig(encoding=0)
    guess = 0

    def __init__(self, init_bundle, register_callback):
        BOCSPuzzle.__init__(self, init_bundle)

        self.eink.set_text('What is 8 + 2?')

        self.cube_state = CubeState(mode=CubeState.TRELLIS)
        self.update_io_state(ARDUINO1, self.cube_state)

        self.trellis_state = TrellisState(repeat_pattern=True)
        self.trellis_light_config = TrellisLightConfig()
        self.trellis_state.add_blink_frame(self.trellis_light_config, 10)

        register_callback(self.input_event)

    def input_event(self, event):

        if event.id == EventType.TRELLIS_BUTTON_PRESS:

            if event.data != '0':
                self.guess ^= int(event.data)
                print('Guess:', self.guess)
                self.trellis_light_config.set_lights(self.guess)

                if self.guess == self.ANSWER:
                    self.report_attempt(self.PUZZLE_ID)
                    self.trellis_state = TrellisState(repeat_pattern=False)
                    correct_config = self.trellis_light_config
                    self.trellis_state.add_blink_frame(self.LIGHT_CONFIG_OFF, 500)
                    self.trellis_state.add_blink_frame(correct_config, 500)
                    self.trellis_state.add_blink_frame(self.LIGHT_CONFIG_OFF, 500)
                    self.trellis_state.add_blink_frame(correct_config, 500)
                    self.trellis_state.add_blink_frame(self.LIGHT_CONFIG_OFF, 500)
                    self.trellis_state.add_blink_frame(correct_config, 1500)
                    self.update_io_state(ARDUINO1, self.trellis_state)
                    self.eink.set_text('Correct!')
                    # Hide the Trellis
                    self.cube_state.set_mode(CubeState.CLOSED)
                    self.update_io_state(ARDUINO1, self.cube_state)
                    self.pause(5)
                    self.is_solved = True
                else:
                    self.trellis_state.clear_pattern()
                    self.trellis_state.add_blink_frame(self.trellis_light_config, 10)
                    self.update_io_state(ARDUINO1, self.trellis_state)



