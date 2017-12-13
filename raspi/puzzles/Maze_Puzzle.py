from raspi.puzzles.puzzle import BOCSPuzzle
from raspi.arduino_comm import ArduinoCommEventType as EventType
from raspi.available_io import *
from random import *


class MazePuzzle(BOCSPuzzle):

    PUZZLE_ID = 'maze_puzzle'  # A unique ID (can be anything) to use when reporting puzzle stats to the server

    is_solved = False  # Set this to True when you want the BOCS to progress to the next puzzle

    def __init__(self, stat_server, update_io_state, register_callback):
        """
        Runs once, when the puzzle is first started.
        :param update_io_statke: a callback function to update the state of an I/O device
        :param register_callback: a callback function to register the function that should be called anytime a BOCS
            user input event occurs
        """
        # Perform some standard initialization defined by the BOCSPuzzle base class
        BOCSPuzzle.__init__(self, stat_server, update_io_state)

        # Register our `event_received` function to be called whenever there is a BOCS input event (e.g. key press)
        register_callback(self.user_input_event_received)

        # Let’s display the image bocs-start.png on the e-ink display
        self.eink.set_image('maze_puzzle.png') #Does not exist yet

        #Deploy Trellis

        #Variable Setup
        #Define end point
        #Define start point
        #Define current position (start point)
        #Define obstacle points

        #Set all LED's to on
        #Set end point LED (off)
        #Set current pos LED (blink)
        #Set obstacle LED's (fade)

    def user_input_event_received(self, event):
        """
        This function is called whenever the user triggers an input event on the BOCS. The majority of your puzzle logic
        should probably be here (or be in another function called from here), as this will run every time the user does
        something to the box.
        :param event: an object with attributes `id` (which event happened), `data` (what was the primary data
        associated with the event, e.g. which key/number) and `options` (extra data, usually empty)
        """

        #Set current pos LED to solid

        #if: check if button pressed is above current position #These will not always be possible (ie on edge)
            #Set current pos one below
            #Check for wrap around, adjust value accordingly
        #elif: if button pressed is below current position
            # Set current pos one above
            # Check for wrap around, adjust value accordingly
        #elif: if button pressed is right of current pos
            # Set current pos one left
            # Check for wrap around, adjust value accordingly
        #elfif: if button pressed is right of current pos
            # Set current pos one right
            # Check for wrap around, adjust value accordingly
        #else: Do nothing

        #if: check if current pos is the same as an obstacle
            #make angry noise
            #Set current pos to start point

        #if: check if current pos is the end point
            #Play winning noise
            self.is_solved = True

        #Set current pos LED to blink

        if self.code_state == 6:                         #if code is complete
            #TODO play finished audio clip          #yes - puzzle is finished, play audio, change screen
            self.eink.set_image('frequency_puzzle_solved.png')  # Does not exist yet
            self.is_solved = True
            #no - do nothing
