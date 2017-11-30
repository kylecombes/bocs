from raspi.puzzles.puzzle import BOCSPuzzle
from raspi.arduino_comm import ArduinoCommEventType as EventType


class NothingPuzzle(BOCSPuzzle):

    PROMPT = "\"Poor people have it, rich people need it and, if you eat it, you die.\"\n\nThe drawer wants to know."

    def __init__(self, init_bundle, register_callback):
        BOCSPuzzle.__init__(self, init_bundle)

        self.reset_puzzle()

        # Subscribe to input events
        register_callback(self.event_received)

    def reset_puzzle(self):
        self.eink.set_text(self.PROMPT)

    def event_received(self, event):
        if event.id == EventType.DRAWER_STATE_CHANGE:
            if event.data == '0':  # Drawer closed
                if event.extra:
                    self.eink.set_text("Sorry, that's incorrect.")
                    self.pause(5)
                    self.reset_puzzle()
                else:
                    self.eink.set_text('Correct!')
                    self.pause(5)
                    self.is_solved = True
