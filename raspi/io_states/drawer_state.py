from raspi.io_states.io_state import IOState


class DrawerState(IOState):

    def __init__(self, old_state_json=None):
        IOState.__init__(self, old_state_json)

    def open(self):
        """
        Opens the drawer so the user can (maybe) put an object in it or remove it.
        """
        self._data['open_drawer'] = 1
