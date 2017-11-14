from raspi.io_states.io_state import IOState


class TrellisState(IOState):

    LIGHTS_KEY = 'tLEDs'  # Key used when sending LED statuses to Arduino (encoded as JSON)

    def __init__(self, old_state=None):
        IOState.__init__(self, old_state)

        self._data[self.LIGHTS_KEY] = 0b0  # Binary encoding of which lights are on

    def set_led_state(self, id, is_on):
        """
        Turns an LED on or off.
        :param id: the zero-indexed id of the LED to change
        :param is_on: True to turn the light on, False to turn off
        """
        if is_on:
            self._data[self.LIGHTS_KEY] |= 1 << id  # Set id-th bit to 1
        else:
            self._data[self.LIGHTS_KEY] &= ~(1 << id)  # Set id-th bit to 0

