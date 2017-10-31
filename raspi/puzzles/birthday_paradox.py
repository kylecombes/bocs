from raspi.arduino_comm_event import ArduinoCommEventType as EventType


class BirthdayParadoxPuzzle:

    guess = ""
    ANSWER = "23"
    is_solved = False

    def __init__(self, update_io_state, register_callback):
        self.updateIOState = update_io_state  # TODO Implement IOState
        register_callback(EventType.NUMERIC_KEYPAD_PRESS, self.key_pressed)

    def key_pressed(self, event):
        key = event.data
        if key == '#':
            if self.guess == self.ANSWER:
                print("Correct!")
                self.is_solved = True
            else:
                print("Sorry, that's incorrect")
        else:
            self.guess += key
