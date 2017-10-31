class ArduinoCommEvent:

    def __init__(self, event_id, data, options=None):
        self.id = event_id
        self.data = data
        self.options = options


class ArduinoCommEventType:
    NUMERIC_KEYPAD_PRESS = 0
    LED_KEYPAD_PRESS = 1
    PIANO_KEYBOARD_CHANGE = 2
    DRAWER_STATE_CHANGE = 3
    TELEGRAPH_TAP = 4
    RESISTOR_ARRAY_CHANGE = 5
