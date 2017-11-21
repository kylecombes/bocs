from raspi.puzzles.puzzle import BOCSPuzzle
from raspi.arduino_comm_event import ArduinoCommEventType as EventType


class StartPrompt(BOCSPuzzle):

    def __init__(self, update_io_state, register_callback):
        BOCSPuzzle.__init__(self, update_io_state)
        # Show the BOCS logo and "Press Start" message on the e-ink display
        self.eink.set_image('bocs-start.png')

        # Subscribe to input events
        register_callback(self.key_pressed)

    def key_pressed(self, event):
        if event.id == EventType.START_BUTTON_PRESS:
            # TODO Shut off light
            self.is_solved = True
