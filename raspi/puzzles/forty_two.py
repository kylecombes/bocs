from raspi.puzzles.puzzle import BOCSPuzzle
from raspi.arduino_comm import ArduinoCommEventType as EventType
from raspi.io_states.rotary_state import RotaryState
from raspi.available_io import *
from datetime import datetime


class FortyTwoPuzzle(BOCSPuzzle):

    PUZZLE_ID = 'forty_two!'  # A unique ID (can be anything) to use when reporting puzzle stats to the server

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
        "And here I thought you had potential.\Perhaps you are simply pestulential.\n\nTry again.",
        "That is really too bad.\nMaybe it is not intelligence they said you humans had.",
        "I am starting to grow weary of this.\nI think something has gone amiss.",
        "Now I am very much perturbed.\nI'll grant you another turn.",
        "You have exhausted my patience and my creativity. Unless you give me the correct answer, I will say nothing " \
        "more than \"wrong\"."
    ]
    CORRECT_RESPONSE = "Marvelous!\nYou have discovered the truth of 42.\nPerhaps not so much is beyond you."
    attempt_count = 0
    scale = 1
    error = 200  # +||- in milliseconds
    count1 = 0
    count2 = 0
    # the array of input time values the switch is open and closed for
    model = scale*[[1000, 1000],[1000, 1000],[1000, 1000],[1000, 1000],[3000, 1000],[1000, 1000],[3000, 1000],[3000, 1000],[3000, 1000],[3000, 1000]]
    user_input = [[0 for x in range(9)] for y in range(2)]
    # the array of model time values to compare the input to
    is_solved = False  # Set this to True when you want the BOCS to progress to the next puzzle

    def __init__(self, init_bundle, register_callback):
        """
        Runs once, when the puzzle is first started.
        :param update_io_state: a callback function to update the state of an I/O device
        :param register_callback: a callback function to register the function that should be called anytime a BOCS
            user input event occurs
        """
        # Perform some standard initialization defined by the BOCSPuzzle base class
        BOCSPuzzle.__init__(self, init_bundle)

        # Register our `event_received` function to be called whenever there is a BOCS input event (e.g. key press)
        register_callback(self.user_input_event_received)

        self.reset()

        self.rotary_state = RotaryState()
        self.rotary_state.set_mode(RotaryState.TELEGRAPH)
        self.update_io_state(ARDUINO1, self.rotary_state)
        self.last_press_time = datetime.now()

    def reset(self):
        self.count1 = 0
        self.count2 = 0
        self.user_input = [[0 for x in range(self.w)] for y in range(self.h)]

        self.eink.set_text(self.PROMPT)

    def user_input_event_received(self, event):
        """
        This function is called whenever the user triggers an input event on the BOCS. The majority of your puzzle logic
        should probably be here (or be in another function called from here), as this will run every time the user does
        something to the box.
        :param event: an object with attributes `id` (which event happened), `data` (what was the primary data
        associated with the event, e.g. which key/number) and `options` (extra data, usually empty)
        """

        if event.id == EventType.TELEGRAPH_BUTTON_PRESS:  # See if the event was a start button press
            duration = event.data  # The amount of time the button was held down for (in ms)
            time_open = ((datetime.now() - self.last_press_time).total_seconds())/1000-float(duration)
            if time_open>self.scale*(3000+self.error):
                user_input = [[0 for x in range(self.w)] for y in range(self.h)]
                self.count1 = 0
            self.user_input[self.count1][0]=duration
            self.user_input[self.count1][1]=time_open
            self.count1+=1
            self.last_press_time = datetime.now()

            if self.count1==9:
                for x in range (0,9):
                    for y in range (0,1):
                        if self.model[x][y]-self.error <= self.user_input[x][y] <= self.model[x][y]+self.error:
                            self.count2+=1
                #input is complete sweep and compare two arrays
                #increment count2 if input value within model value

            if self.count2 >= 18: #arrays match within tolerances
                self.eink.set_text(self.CORRECT_RESPONSE)
                self.pause(5)
                self.rotary_state.set_mode(RotaryState.CLOSED)
                self.update_io_state(ARDUINO1, self.rotary_state)
                self.is_solved = True  # Exits this puzzle
            else:
                if self.attempt_count < len(self.RESPONSES):
                    text = self.RESPONSES[self.attempt_count]
                    self.attempt_count += 1
                else:
                    text = "Wrong"
                self.eink.set_text(text)
                self.pause(5)
                self.reset()