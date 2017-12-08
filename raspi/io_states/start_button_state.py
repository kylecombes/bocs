from raspi.io_states.io_state import IOState


class StartButtonState(IOState):

    MAX_FRAMES = 20
    OUTPUT_ID = 'S'  # Identifier for Arduino to know we're talking about the start button

    def __init__(self, repeat_pattern=False, old_state=None):
        IOState.__init__(self, old_state)

        self._data['repeat'] = repeat_pattern
        self.clear_pattern()

    def set_repeating(self, repeats):
        self._data['repeat'] = repeats

    def clear_pattern(self):
        self._data['pattern'] = ''
        self._data['repeat'] = False

    def set_led_on(self, is_on):
        """
        Sets whether the start button LED is on or off.
        :param is_on: True for on, False for off
        """
        self._data['repeat'] = False
        self._data['pattern'] = 'T' if is_on else 'F'

    def add_blink_frame(self, is_on, time):
        """
        Adds a blink frame to the pattern/animation.
        :param is_on: whether or not the light should be on
        :param time: how long the frame should last
        :return: True if added successfully, False if blink frame limit exceeded
        """
        if len(self._data[self.OUTPUT_ID]) >= self.MAX_FRAMES:
            return False  # Cannot have more than MAX_FRAMES

        self._data['pattern'] += '{}{}'.format('T' if is_on else 'F', time)
        return True

    def get_arduino_message(self):
        return '{}{}{}'.format(self.OUTPUT_ID, self._data['pattern'], 'R' if self._data['repeat'] else '')
