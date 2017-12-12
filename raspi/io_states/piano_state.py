from raspi.io_states.io_state import IOState


class PianoState(IOState):

    def __init__(self, old_state_json=None):
        IOState.__init__(self, old_state_json)

    def set_visible(self, is_visible):
        """
        Opens the drawer so the user can (maybe) put an object in it or remove it.
        """
        self._data['is_visible'] = 1 if is_visible else 0

    def get_arduino_message(self):
        return self._data['is_visible']
