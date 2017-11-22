import time
from raspi.puzzles.puzzle import BOCSPuzzle
from raspi.arduino_comm import ArduinoCommEventType as EventType
from raspi.available_io import *
from raspi.io_states.start_button_state import StartButtonState


class StartPrompt(BOCSPuzzle):

    def __init__(self, update_io_state, register_callback):
        BOCSPuzzle.__init__(self, update_io_state)
        # Show the BOCS logo and "Press Start" message on the e-ink display
        self.eink.set_image('bocs-start.png')

        # Turn on start button LED
        start_button_state = StartButtonState()
        start_button_state.set_led_on(True)
        update_io_state(ARDUINO1, start_button_state)

        # Subscribe to input events
        register_callback(self.key_pressed)

    def key_pressed(self, event):
        if event.id == EventType.START_BUTTON_PRESS:
            # Turn off start button LED
            start_button_led_state = StartButtonState()
            start_button_led_state.set_led_on(False)
            self.update_io_state(ARDUINO1, start_button_led_state)

            # Play Mac startup chime
            time.sleep(2)  # Wait a couple seconds, to emulate a Mac starting up
            self.play_sound('media/Mac-chime.wav')

            # Mark puzzle as solved
            self.is_solved = True
