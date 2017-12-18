from raspi.io_states.start_button_state import StartButtonState
from raspi.puzzles.puzzle import BOCSPuzzle
from raspi.arduino_comm import ArduinoCommEventType as EventType
from raspi.available_io import *


class VictoryPuzzle(BOCSPuzzle):

    PUZZLE_ID = 'Victory'  # A unique ID (can be anything) to use when reporting puzzle stats to the server

    is_solved = False  # Set this to True when you want the BOCS to progress to the next puzzle

    def __init__(self, init_bundle, register_callback):
        """
        Runs once, when the puzzle is first started.
        :param init_bundle: an initialization bundle to be passed to the superclass constructor
        :param register_callback: a callback function to register the function that should be called anytime a BOCS
            user input event occurs
        """
        # Perform some standard initialization defined by the BOCSPuzzle base class
        BOCSPuzzle.__init__(self, init_bundle)

        # Register our `event_received` function to be called whenever there is a BOCS input event (e.g. key press)
        register_callback(self.user_input_event_received)

        # Let’s display the image bocs-start.png on the e-ink display
        self.eink.set_image('victory-qr.png')

        # Blink the start button to prompt restarting
        start_button_state = StartButtonState(repeat_pattern=True)
        start_button_state.add_blink_frame(True, 800)
        start_button_state.add_blink_frame(False, 800)
        self.update_io_state(ARDUINO1, start_button_state)

    def user_input_event_received(self, event):
        """
        This function is called whenever the user triggers an input event on the BOCS. The majority of your puzzle logic
        should probably be here (or be in another function called from here), as this will run every time the user does
        something to the box.
        :param event: an object with attributes `id` (which event happened), `data` (what was the primary data
        associated with the event, e.g. which key/number) and `options` (extra data, usually empty)
        """

        if event.id == EventType.START_BUTTON_PRESS:  # See if the event was a start button press
            self.is_solved = True  # Exits this puzzle
