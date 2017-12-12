from raspi.puzzles.puzzle import BOCSPuzzle
from raspi.io_states.piano_state import PianoState
from raspi.arduino_comm import ArduinoCommEventType as EventType
from raspi.available_io import *
from random import *


class FrequencyPuzzle(BOCSPuzzle):

    PUZZLE_ID = 'frequency_puzzle'  # A unique ID (can be anything) to use when reporting puzzle stats to the server

    is_solved = False  # Set this to True when you want the BOCS to progress to the next puzzle

    def __init__(self, init_bundle, register_callback):
        """
        Runs once, when the puzzle is first started.
        :param update_io_statke: a callback function to update the state of an I/O device
        :param register_callback: a callback function to register the function that should be called anytime a BOCS
            user input event occurs
        """
        # Perform some standard initialization defined by the BOCSPuzzle base class
        BOCSPuzzle.__init__(self, init_bundle)

        # Register our `event_received` function to be called whenever there is a BOCS input event (e.g. key press)
        register_callback(self.user_input_event_received)

        # Let’s display the image bocs-start.png on the e-ink display
        self.eink.set_text('392-415-349-440-392-370\n(G, G#, F, A, G, F#)')

        # Display the piano
        self.piano_state = PianoState()
        self.piano_state.set_visible(True)
        self.update_io_state(PIANO, self.piano_state)

        #Variable Setup
        self.code = '341532' #Assuming inputs are int increments from left to right
        self.code_state = 0  #Current input to look for in code

    def user_input_event_received(self, event):
        """
        This function is called whenever the user triggers an input event on the BOCS. The majority of your puzzle logic
        should probably be here (or be in another function called from here), as this will run every time the user does
        something to the box.
        :param event: an object with attributes `id` (which event happened), `data` (what was the primary data
        associated with the event, e.g. which key/number) and `options` (extra data, usually empty)
        """

        if event.data == int(self.code[self.code_state]):     #if input is = to relevant code input
            self.code_state += 1                         #yes - increment code input
            if self.code_state == 2:                      #check whether to play audio
                #TODO play correct2 audio clip      #yes - Play audio
                pass
            elif self.code_state == 4:
                #TODO play correct4 audio clip
                pass
        else:                                       #no - reset code input
            self.code_state = 0
            if random() >= 0.5:     #check whether to play audio (random)
                #TODO play random incorrect audio clip
                pass

        if self.code_state == 6:                         #if code is complete
            #TODO play finished audio clip          #yes - puzzle is finished, play audio, change screen
            self.eink.set_text('Good job!')
            self.piano_state.set_visible(False)
            self.update_io_state(PIANO, self.piano_state)
            self.pause(5)
            self.is_solved = True
            #no - do nothing
