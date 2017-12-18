from raspi.puzzles.puzzle import BOCSPuzzle
from raspi.arduino_comm import ArduinoCommEventType as EventType
from raspi.io_states.keypad_state import KeypadState
from raspi.io_states.start_button_state import StartButtonState
from raspi.available_io import *
from datetime import datetime
import math


class FortyTwoPuzzle(BOCSPuzzle):

    PUZZLE_ID = 'What is 6x9?'  # A unique ID (can be anything) to use when reporting puzzle stats to the server

    ANSWER = '42'

    HINTS = ['']

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
        "And here I thought you had potential.\nPerhaps you are simply pestilential.\n\nTry again.",
        "That is really too bad.\nMaybe it is not intelligence they said you humans had.",
        "I am starting to grow weary of this.\nI think something has gone amiss.",
        "Now I am very much perturbed.\nI'll grant you another turn.",
        "You have exhausted my patience and my creativity. Unless you give me the correct answer, I will say nothing "
        "more than \"wrong\"."
    ]
    CORRECT_RESPONSE = "Marvelous!\nYou have discovered the truth of 42.\nPerhaps not so much is beyond you."

    attempt_count = 0
    guess = ''

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

        self.hint_pos = 0
        self.state = 'initialized'
        print('42 puzzled initialized')

        # Show keypad
        self.keypad_state = KeypadState(visible=True)
        self.update_io_state(ARDUINO1, self.keypad_state)

    def user_input_event_received(self, event):
        """
        This function is called whenever the user triggers an input event on the BOCS. The majority of your puzzle logic
        should probably be here (or be in another function called from here), as this will run every time the user does
        something to the box.
        :param event: an object with attributes `id` (which event happened), `data` (what was the primary data
        associated with the event, e.g. which key/number) and `options` (extra data, usually empty)
        """
        if event.id == EventType.NUMERIC_KEYPAD_PRESS:
            key = event.data
            if key == '#':  # Submit
                if self.guess == self.ANSWER:
                    self.keypad_state.set_visible(False)
                    self.update_io_state(ARDUINO1, self.keypad_state)
                    self.eink.set_text('Correct!')
                    self.report_attempt(self.PUZZLE_ID)
                    self.is_solved = True
                else:
                    self.eink.set_text('{}\n\n{}'.format(self.PROMPT, "Sorry, that's incorrect"))
                    self.report_attempt(self.PUZZLE_ID, self.guess)
                    self.pause(5)
                    self.eink.set_text(self.PROMPT)
            else:
                if key == '*':  # Backspace
                    self.guess = self.guess[0:-1]
                else:  # Digit entry
                    self.guess += str(key)

                self.eink.set_text('{}\n\n{}{}'.format(self.PROMPT, 'Input: ', self.guess))


