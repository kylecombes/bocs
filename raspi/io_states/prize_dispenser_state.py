from raspi.io_states.io_state import IOState


class PrizeDispenserState(IOState):

    def __init__(self, old_json_state=None):
        IOState.__init__(self, old_state_json=old_json_state)
        self._data['mode'] = ''

    def dispense_candy(self):
        self._data['mode'] = 'C'

    def get_arduino_message(self):
        return 'P{}'.format(self._data['mode'])
