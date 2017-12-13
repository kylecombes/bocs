import time

from raspi.puzzles.puzzle import BOCSPuzzle
from raspi.arduino_comm import ArduinoCommEventType as EventType
from raspi.available_io import *
from raspi.io_states.start_button_state import StartButtonState


class StartPrompt(BOCSPuzzle):

    GREETING = 'Greetings\nI am BOCS. If you can solve my puzzles, you will be duly rewarded.\n\n(FYI: The Internet ' \
        'is a valid resource.)'

    def __init__(self, init_bundle, register_callback):
        BOCSPuzzle.__init__(self, init_bundle)
        # Show the BOCS logo and "Press Start" message on the e-ink display
        self.eink.set_image('media/bocs-start.png')

        # Subscribe to input events
        register_callback(self.key_pressed)

        # Blink the start button repeatedly
        start_button_led_state = StartButtonState(repeat_pattern=True)
        start_button_led_state.add_blink_frame(True, time=800)
        start_button_led_state.add_blink_frame(False, time=800)
        self.update_io_state(ARDUINO1, start_button_led_state)

    def key_pressed(self, event):
        if event.id == EventType.START_BUTTON_PRESS:
            # Play Mac startup chime
            self.play_sound('Mac-chime.wav')

            # Turn off start button LED
            start_button_led_state = StartButtonState()
            start_button_led_state.set_led_on(False)
            self.update_io_state(ARDUINO1, start_button_led_state)

            # Display greeting
            self.eink.clear()
            self.eink.set_text(self.GREETING)

            # Wait 8 seconds before progressing
            time.sleep(5)

            # Transmit start stat
            # self.report_attempt('start')

            # Mark puzzle as solved
            self.is_solved = True
