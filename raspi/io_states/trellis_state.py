from raspi.io_states.io_state import IOState


class TrellisState(IOState):

    LIGHTS_KEY = 'T'  # Key used when sending LED statuses to Arduino
    MAX_FRAMES = 20  # Maximum number of blink frames in the pattern/animation (limited in Arduino code)

    def __init__(self, repeat_pattern=False, old_state=None):
        IOState.__init__(self, old_state)

        self._data['repeat'] = repeat_pattern
        self.clear_pattern()

    def set_repeating(self, repeats):
        self._data['repeat'] = repeats

    def clear_pattern(self):
        self._data[self.LIGHTS_KEY] = []

    def add_blink_frame(self, light_config, time):
        """
        Adds a blink frame to the pattern/animation.
        :param light_config:
        :param time:
        :return: True if successful, False otherwise
        """
        if len(self._data[self.LIGHTS_KEY]) >= self.MAX_FRAMES:
            return False  # Cannot have more than MAX_FRAMES

        self._data[self.LIGHTS_KEY].append((light_config, time))
        return True

    def get_arduino_message(self):
        msg = ""
        for frame in self._data[self.LIGHTS_KEY]:
            pattern, duration = frame
            msg += '{},{};'.format(str(pattern), duration)
        return 'T{}{}'.format(msg, 'R' if self._data['repeat'] else '')

    def get_blink_frame(self, index):
        if index > len(self._data[self.LIGHTS_KEY]):
            return False
        return self._data[self.LIGHTS_KEY][index]


class TrellisLightConfig:
    """
    A single frame in a blink animation
    """

    def __init__(self, encoding=0):
        self.lights = encoding

    def set_lights(self, binary_encoding):
        """
        Set all lights at once.
        :param binary_encoding: a binary encoding of the Trellis lights
        """
        self.lights = binary_encoding

    def set_led_on(self, id, is_on):
        """
        Turns an LED on or off.
        :param id: the zero-indexed id of the LED to change
        :param is_on: True to turn the light on, False to turn off
        """
        if is_on:
            self.lights |= 1 << id  # Set id-th bit to 1
        else:
            self.lights &= ~(1 << id)  # Set id-th bit to 0

    def __str__(self):
        return str(self.lights)
