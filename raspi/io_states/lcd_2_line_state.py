from raspi.io_states.io_state import IOState


class LCD2LineState(IOState):

    # These need to be strings for the Arduino's JSON parser to work
    TOP_LINE = '0'
    BOTTOM_LINE = '1'
    BACKLIGHT = 'bl'

    # Private member - do not access directly from outside this class
    _data = {}

    def __init__(self, line1="", line2="", backlight_on=True, old_state_json=None):
        # Call parent class (IOState) constructor
        IOState.__init__(self, old_state_json)

        self._data[self.BACKLIGHT] = 1 if backlight_on else 0
        self._data[self.TOP_LINE] = line1
        self._data[self.BOTTOM_LINE] = line2

    def set_line(self, line_number, string):
        if not line_number == self.TOP_LINE and not line_number == self.BOTTOM_LINE:
            raise ValueError('{} is not a valid line number. Expected {} (top line) or {} (bottom line).'
                             .format(line_number, self.TOP_LINE, self.BOTTOM_LINE))
        self._data[line_number] = string

    def get_line(self, line_number):
        if not line_number == self.TOP_LINE or not line_number == self.BOTTOM_LINE:
            return False  # Invalid line position
        return self._data[line_number]

    def set_backlight(self, is_on):
        self._data[self.BACKLIGHT] = 1 if is_on else 0
