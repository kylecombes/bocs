import json


class IOState:

    def __init__(self, old_state_json=None):
        self._data = json.loads(old_state_json) if old_state_json else {}

    def serialize(self):
        return json.dumps(self._data)
