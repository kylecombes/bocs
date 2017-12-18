from raspi.puzzles.puzzle import BOCSPuzzle
from raspi.arduino_comm import ArduinoCommEventType as EventType
from raspi.io_states.cube_state import RotaryState
from raspi.io_states.start_button_state import StartButtonState
from raspi.available_io import *
from datetime import datetime
import math


class TelegraphPuzzle(BOCSPuzzle):
    PUZZLE_ID = '6x9'  # A unique ID (can be anything) to use when reporting puzzle stats to the server

    PROMPT = "Here is something you may recognize.\n" \
             "After all, this truth was discovered by one of your kind.\n\n" \
             "As you travel across the stars,\n" \
             "Finding rides as they come,\n" \
             "Never forget this truth or ours,\n" \
             "For there is only one.\n\n" \
             "Answer this question right and true,\n" \
             "And I may have some more respect for you.\n" \
             "What is 6x9?"
    RESPONSES = [
        "And here I thought you had potential.\nPerhaps you are simply pestulential.\n\nTry again.",
        "That is really too bad.\nMaybe it is not intelligence they said you humans had.",
        "I am starting to grow weary of this.\nI think something has gone amiss.",
        "Now I am very much perturbed.\nI'll grant you another turn.",
        "You have exhausted my patience and my creativity. Unless you give me the correct answer, I will say nothing "
        "more than \"wrong\"."
    ]
    CORRECT_RESPONSE = "Marvelous!\nYou have discovered the truth of 42.\nPerhaps not so much is beyond you."

    attempt_count = 0
    SCALE = 0.5
    THRESHOLD = 1 * SCALE  # error threshold (seconds)
    SHORT = 1 * SCALE
    LONG = 1.5 * SCALE
    CORRECT_SEQUENCE_CLOSED = [SHORT, SHORT, SHORT, SHORT, LONG, SHORT, LONG, LONG, LONG, LONG]
    CORRECT_SEQUENCE_OPEN = [SHORT, SHORT, SHORT, SHORT, SHORT, SHORT, SHORT, SHORT, SHORT, SHORT]
    SLOT_COUNT = len(CORRECT_SEQUENCE_CLOSED) + len(CORRECT_SEQUENCE_OPEN)  # Number of slots to correctly enter
    RESET_TIME = 5  # Number of seconds to reset input after
    current_pos = 0
    current_correct_count = 0

    state = 'initializing'

    def __init__(self, init_bundle, register_callback):
        """
        Runs once, when the puzzle is first started.
        :param init_bundle: a callback function to update the state of an I/O device
        :param register_callback: a callback function to register the function that should be called anytime a BOCS
            user input event occurs
        """
        # Perform some standard initialization defined by the BOCSPuzzle base class
        BOCSPuzzle.__init__(self, init_bundle)

        # Register our `event_received` function to be called whenever there is a BOCS input event (e.g. key press)
        register_callback(self.user_input_event_received)

        # Save start time
        self.last_press_time = datetime.now()

        # Display puzzle prompt
        self.eink.set_text(self.PROMPT)

        self.state = 'initialized'
        print('42 puzzled initialized')

        # Show telegraph
        self.rotary_state = RotaryState()
        self.rotary_state.set_mode(RotaryState.TELEGRAPH)
        self.update_io_state(ARDUINO1, self.rotary_state)

        # Correct input blink
        self.start_button_correct_blink = StartButtonState()
        self.start_button_correct_blink.add_blink_frame(True, 100)

        # Incorrect input blink
        self.start_button_incorrect_blink = StartButtonState()
        self.start_button_incorrect_blink.add_blink_frame(True, 200)
        self.start_button_incorrect_blink.add_blink_frame(False, 200)
        self.start_button_incorrect_blink.add_blink_frame(True, 200)

    def user_input_event_received(self, event):
        """
        This function is called whenever the user triggers an input event on the BOCS. The majority of your puzzle logic
        should probably be here (or be in another function called from here), as this will run every time the user does
        something to the box.
        :param event: an object with attributes `id` (which event happened), `data` (what was the primary data
        associated with the event, e.g. which key/number) and `options` (extra data, usually empty)
        """

        # Check that the event was a start button press and that the puzzle has been fully initialized
        if event.id == EventType.TELEGRAPH_BUTTON_PRESS and self.state == 'initialized':

            # Get the amount of time the button was held down for (convert from ms to s)
            time_closed = float(event.data) / 1000
            # Calculate the amount of time the switch was open between presses
            time_open = ((datetime.now() - self.last_press_time).total_seconds()) - time_closed

            print('Current position: ', self.current_pos)
            print('Open time: ', time_open)
            print('Open closed: ', time_closed)

            input_valid = True  # Set to False when there is a mistake

            if time_open > self.RESET_TIME:
                self.current_correct_count = 0
                self.current_pos = 0
                print('Resetting')
                # TODO Do this with an asynchronous timer
            else:  # See if button was left open for the correct amount of time
                if self.input_matches(time_open, self.CORRECT_SEQUENCE_OPEN[self.current_pos]):
                    self.current_correct_count += 1
                    print('Open matched')
                else:
                    print('Open did not match')
                    self.current_pos = 0
                    self.current_correct_count = 0
                    input_valid = False

            if input_valid and self.input_matches(time_closed, self.CORRECT_SEQUENCE_CLOSED[self.current_pos]):
                print('Closed matched')
                self.current_correct_count += 1
                self.current_pos += 1
                if self.current_correct_count == self.SLOT_COUNT - 1:  # Puzzle has been answered correctly
                    print('Puzzle completed successfully')
                    self.eink.set_text(self.CORRECT_RESPONSE)
                    self.rotary_state.set_mode(RotaryState.CLOSED)
                    self.update_io_state(ARDUINO1, self.rotary_state)
                    self.pause(5)
                    self.report_attempt(self.PUZZLE_ID)
                    self.is_solved = True  # Exits this puzzle
                self.update_io_state(ARDUINO1, self.start_button_correct_blink)

            else:  # Answer has not been built up properly -- tell player they're wrong and restart
                print('Closed did not match ({} instead of {})'.format(time_closed,
                                                                       self.CORRECT_SEQUENCE_CLOSED[self.current_pos]))
                self.update_io_state(ARDUINO1, self.start_button_incorrect_blink)
                # if self.attempt_count < len(self.RESPONSES):
                #     text = self.RESPONSES[self.attempt_count]
                #     self.attempt_count += 1
                # else:
                #     text = "Wrong"
                # self.eink.set_text(text)
                self.current_correct_count = 0
                self.current_pos = 0
                # self.pause(5)
                # self.eink.clear()
                # self.eink.set_text(self.PROMPT)

            self.last_press_time = datetime.now()
            print('Correct count: ', self.current_correct_count)
            print('')

    def input_matches(self, reading, correct_value):
        """
        Determines whether or not a given input matches the correct timing. (Global THRESHOLD used.)
        :param reading: the user's input value (in seconds)
        :param correct_value: the correct value (in seconds)
        :return: True if the input matches, False otherwise
        """
        return math.fabs(reading - correct_value) < self.THRESHOLD
