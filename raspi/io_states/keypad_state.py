from raspi.io_states.io_state import IOState


class KeypadState(IOState):

    def __init__(self, old_state_json=False):
        IOState.__init__(self, old_state_json)

    def set_visible(self, is_visible):
        """
        Shows and hides the input to/from the player.
        :param is_visible: True to make the input visible, False to hide it
        """
        self._data['keypad_visible'] = 1 if is_visible else 0
