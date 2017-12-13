from raspi.puzzles.puzzle import BOCSPuzzle
from raspi.arduino_comm import ArduinoCommEventType as EventType
from raspi.available_io import *


class MousePuzzle(BOCSPuzzle):

    PUZZLE_ID = ""
    PROMPT = "I'm hungry for an animal to feed my appetite\n" \
        "A creature that you students can make scurry left and right\n" \
        "Although my understanding of your world is cursory,\n" \
        "I’m certain they’re abundant here so please feed one to me."
    # A unique ID (can be anything) to use when reporting puzzle stats to the server

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

        self.reset_puzzle()

    def reset_puzzle(self):
        # Let’s display the image bocs-start.png on the e-ink display
        self.eink.set_text(self.PROMPT)

    def user_input_event_received(self, event):
        """
        This function is called whenever the user triggers an input event on the BOCS. The majority of your puzzle logic
        should probably be here (or be in another function called from here), as this will run every time the user does
        something to the box.
        :param event: an object with attributes `id` (which event happened), `data` (what was the primary data
        associated with the event, e.g. which key/number) and `options` (extra data, usually empty)
        """

        if event.id == EventType.DRAWER_STATE_CHANGE:
            if event.data == '0':  # Drawer closed
                # if event.extra:
                #     self.eink.set_text("Sorry, that's incorrect.")
                #     self.pause(5)
                #     self.reset_puzzle()
                # else:
                self.eink.set_text('Correct!')
                self.is_solved = True
