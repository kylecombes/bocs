from raspi.io_states.io_state import IOState


class LCDState(IOState):

    # These need to be strings for the Arduino's JSON parser to work
    TOP_LINE = '0'
    BOTTOM_LINE = '1'
    BACKLIGHT = 'bl'

    def __init__(self, line1=None, line2=None, backlight_on=True, old_state_json=None):
        """
        Creates a new instance of LCDState.
        :param line1: the text to show on the first line
        :param line2: the text to show on the second line
        :param backlight_on: (optional) whether or not to turn the backlight on immediately (can be turned on manually
            later by calling set_backlight()). Defaults to True.
        :param old_state_json: (optional) a JSON-encoded state to load into the current object. Non-None values passed
        as arguments to this constructor will take precedent (i.e. overwrite) values loaded from this state object.
        """
        # Call parent class (IOState) constructor
        IOState.__init__(self, old_state_json)
        if self.set_backlight is not None:
            self._data[self.BACKLIGHT] = '1' if backlight_on else '0'
        if line1:
            self._data[self.TOP_LINE] = line1
        if line2:
            self._data[self.BOTTOM_LINE] = line2

    def set_line(self, line_number, string):
        """
        Sets a line on the LCD.
        :param line_number: the zero-indexed line number
        :param string: the message to display (max length: 200 characters)
        """
        if not line_number == self.TOP_LINE and not line_number == self.BOTTOM_LINE:
            raise ValueError('{} is not a valid line number. Expected {} (top line) or {} (bottom line).'
                             .format(line_number, self.TOP_LINE, self.BOTTOM_LINE))
        self._data[line_number] = string

    def get_line(self, line_number):
        """
        Gets the string being displayed on a particular line. Note: It is the entire string, not just the portion
        currently displayed on the LCD.
        :param line_number: the zero-indexed line number
        :return: the string set for the given line number
        """
        if not line_number == self.TOP_LINE or not line_number == self.BOTTOM_LINE:
            return False  # Invalid line position
        return self._data[line_number]

    def set_backlight(self, is_on):
        """
        Controls whether or not the backlight is on or off.
        :param is_on: True if the backlight should be on, False otherwise
        """
        self._data[self.BACKLIGHT] = '1' if is_on else '0'
