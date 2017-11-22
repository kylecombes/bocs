import websocket


class ServerComm:
    """
    Used to report game and BOCS health statistics to the server.
    """
    CONNECTING = 1
    CONNECTED = 2
    CLOSED = 3

    def __init__(self, uri, debug=False):
        """
        Initializes a new connection to the BOCS statistics server.
        :param uri: the URI of the server (e.g. ws://localhost:8000)
        """
        websocket.enableTrace(debug)

        self.ws = websocket.WebSocketApp(uri, on_message=self.on_message, on_error=self.on_error, on_close=self.on_close)
        self.state = ServerComm.CONNECTING
        self.ws.on_open = self.on_open
        self.ws.run_forever()

    def on_open(self, *args):
        self.state = ServerComm.CONNECTED

    def on_message(self, ws, message):
        print(message)

    def on_error(self, ws, error):
        self.state = ServerComm.CLOSED
        print('Error: \n' + error)

    def on_close(self, message):
        self.state = ServerComm.CLOSED
        print('Closed')

    def send_puzzle_stat(self, stat):
        """
        Sends a statistic packet to the server.
        :param stat: a Statistic
        :return: True if sent successfully, False otherwise
        """
        if self.state != ServerComm.CONNECTED:
            return False

        # TODO Serialize stat
        # TODO Send serialized stat
        return True

    def send_health_update(self, health):
        """
        Sends a BOCS health update to the server.
        :param health: a BOCSHealth update
        :return: True if sent successfully, False otherwise
        """
        if self.state != ServerComm.CONNECTED:
            return False

        # TODO Serialize health state
        # TODO Send serialized state
        return True

if __name__ == "__main__":
    sc = ServerComm('ws://localhost:2492')
