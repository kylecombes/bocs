from raspi.io_states.io_state import IOState


class StartButtonState(IOState):

    def __init__(self, old_state_json=None):
        IOState.__init__(self, old_state_json)

    def set_led_on(self, is_on):
        """
        Sets whether the start button LED is on or off.
        :param is_on: True for on, False for off
        """
        self._data['sLED'] = is_on

    def get_arduino_message(self):
        return 'S{}'.format('T' if self._data['sLED'] else 'F')
