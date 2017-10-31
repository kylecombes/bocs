class ArduinoCommEvent:

    def __init__(self, event_id, data, options=None):
        self.id = event_id
        self.data = data
        self.options = options
