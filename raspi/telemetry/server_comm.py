from threading import Thread

import websocket
import json


class ServerComm:
    """
    Used to report game and BOCS health statistics to the server.
    """
    CONNECTING = 1
    CONNECTED = 2
    CLOSED = 3

    def __init__(self, uri, name=None, debug=False):
        """
        Initializes a new connection to the BOCS statistics server.
        :param uri: the URI of the server (e.g. ws://localhost:8000)
        :param name: a name to identify this server connection when debugging
        """
        if debug:
            print('Connecting to WebSocket server {} at {}'.format(name, uri))
        self.connection = ServerCommThread(uri, self.message_received, name, debug)
        self.connection.start()
        self.name = name
        self.debug = debug

    def get_state(self):
        return self.connection.state

    def message_received(self, message):
        print('Messaged received by {}:'.format(self.name))
        print(message)

    def send_data(self, data):
        """
        Sends a puzzle answer attempt to the server.
        :param data: a dictionary to send (will be converted to JSON)
        :return: True if sent successfully, False otherwise
        """
        if self.debug:
            print('Transmitting {} to {}'.format(data, self.name))
        self.connection.send_data(data)


class ServerCommThread(Thread):

    def __init__(self, uri, message_received_callback, name, debug=False):
        Thread.__init__(self, name='BOCS WebSocket connection {}'.format(name))

        websocket.enableTrace(debug)

        self.message_received_callback = message_received_callback

        self.ws = websocket.WebSocketApp(uri, on_message=self.on_message, on_error=self.on_error, on_close=self.on_close)
        self.state = ServerComm.CONNECTING
        self.ws.on_open = self.on_open

    def run(self):
        self.ws.run_forever()

    def on_open(self, *args):
        self.state = ServerComm.CONNECTED

    def on_message(self, ws, message):
        if self.message_received_callback:
            self.message_received_callback(message)

    def on_error(self, ws, error):
        self.state = ServerComm.CLOSED
        print('Server {} error: {}'.format(self.name, str(error)))

    def on_close(self, message):
        self.state = ServerComm.CLOSED
        print('Server {} connection closed'.format(self.name))

    def send_data(self, data):
        """
        Sends a puzzle answer attempt to the server.
        :param data: a dictionary to send (will be converted to JSON)
        :return: True if sent successfully, False otherwise
        """
        if self.state != ServerComm.CONNECTED:
            return False

        # Convert to JSON string
        data = json.dumps(data)

        # Send to the server
        self.ws.send(data)

        return True


if __name__ == "__main__":
    sc = ServerComm('ws://localhost:2492')
