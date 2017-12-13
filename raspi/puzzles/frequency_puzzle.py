from raspi.puzzles.puzzle import BOCSPuzzle
from raspi.io_states.piano_state import PianoState
from raspi.arduino_comm import ArduinoCommEventType as EventType
from raspi.available_io import *
from random import *


class FrequencyPuzzle(BOCSPuzzle):

    PUZZLE_ID = 'Musical Numbers'  # A unique ID (can be anything) to use when reporting puzzle stats to the server
    PROMPT = '392-415-349-440-392-370\n(G, G#, F, A, G, F#)\n\nInput: '
    ANSWER = '341532'
    entered_input = ''

    is_solved = False  # Set this to True when you want the BOCS to progress to the next puzzle

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

        # Let’s display the image bocs-start.png on the e-ink display
        self.eink.set_text(self.PROMPT)

        # Display the piano
        self.piano_state = PianoState()
        self.piano_state.set_visible(True)
        self.update_io_state(PIANO, self.piano_state)

    def user_input_event_received(self, event):
        """
        This function is called whenever the user triggers an input event on the BOCS. The majority of your puzzle logic
        should probably be here (or be in another function called from here), as this will run every time the user does
        something to the box.
        :param event: an object with attributes `id` (which event happened), `data` (what was the primary data
        associated with the event, e.g. which key/number) and `options` (extra data, usually empty)
        """

        if event.data == '6':  # Backspace
            self.entered_input = self.entered_input[0:-1]

        elif event.data == '7':  # Submit

            if self.entered_input == self.ANSWER:  # Correct answer input
                self.report_attempt(self.PUZZLE_ID)
                self.piano_state.set_visible(False)
                self.update_io_state(PIANO, self.piano_state)
                self.eink.set_text('Good job!')
                self.pause(3)
                self.is_solved = True

            else:  # Incorrect answer submitted
                self.report_attempt(self.PUZZLE_ID, self.entered_input)
                self.eink.set_text("Sorry, that's incorrect")
                self.pause(3)
                self.eink.set_text(self.PROMPT)
                self.entered_input = ''

        # elif event.data == self.ANSWER[self.pos]:  # Entered next number in correct sequence
        else:
            self.entered_input += event.data
        #     if self.pos == 2:                      #check whether to play audio
        #         #TODO play correct2 audio clip      #yes - Play audio
        #         pass
        #     elif self.pos == 4:
        #         #TODO play correct4 audio clip
        #         pass
        # else:                                       #no - reset code input
        #     if random() >= 0.5:     #check whether to play audio (random)
        #         #TODO play random incorrect audio clip
        #         pass

        # if self.pos == 6:                         #if code is complete
        # TODO play finished audio clip          #yes - puzzle is finished, play audio, change screen
        #no - do nothing

        self.eink.set_text(self.PROMPT + self.entered_input)
