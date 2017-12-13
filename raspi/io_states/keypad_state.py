from raspi.io_states.io_state import IOState


class KeypadState(IOState):

    def __init__(self, visible=False, old_state_json=False):
        IOState.__init__(self, old_state_json)
        self._data['visible'] = visible

    def set_visible(self, is_visible):
        """
        Shows and hides the input to/from the player.
        :param is_visible: True to make the input visible, False to hide it
        """
        self._data['visible'] = is_visible

    def get_arduino_message(self):
        return 'k1' if self._data['visible'] else 'k0'
