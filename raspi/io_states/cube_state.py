from raspi.io_states.io_state import IOState


class CubeState(IOState):

    CLOSED = 'C'
    TELEGRAPH = 'T'
    TRELLIS = 'A'

    def __init__(self, mode=CLOSED, old_state_json=False):
        IOState.__init__(self, old_state_json)
        self._data['mode'] = mode

    def set_mode(self, mode):
        """
        Moves the rotary to change with input (if any) is visible.
        :param mode: can be CubeState.CLOSED, CubeState.TELEGRAPH or CubeState.TRELLIS
        """
        self._data['mode'] = mode

    def get_arduino_message(self):
        return 'C' + self._data['mode']
