from autobahn.twisted.websocket import WebSocketServerProtocol, WebSocketServerFactory
from playsound import playsound
from os import path, environ
import json

MEDIA_FOLDER_PATH = environ['BOCS_MEDIA']


class AudioServerProtocol(WebSocketServerProtocol):

    def __init__(self):
        WebSocketServerProtocol.__init__(self)

    def onConnect(self, request):
        print('Client connecting: {}'.format(request.peer))

    def onOpen(self):
        print('WebSocket connection open')

    def onMessage(self, payload, isBinary):
        json_str = payload.decode()
        if not isBinary:
            try:
                # Decode JSON
                data = json.loads(payload.decode())
            except json.JSONDecodeError:
                print('Error parsing message:\n{}'.format(json_str))
                return

            print('Successfully decoded payload:\n{}'.format(data))

            # Pull out the info that was sent over
            command = data.get('command', None)
            filename = data.get('filename', None)

            if command == 'play':  # We should play a sound file
                # Determine the full path to the audio file
                full_path = path.join(MEDIA_FOLDER_PATH, filename)

                # If the sound file exists, play it
                if path.isfile(full_path):
                    print('Playing file "{}"'.format(full_path))
                    playsound(full_path)
                else:
                    print('Could not find file "{}"'.format(full_path))

    def onClose(self, wasClean, code, reason):
        print("WebSocket connection closed: {}".format(reason))


if __name__ == '__main__':

    from twisted.internet import reactor

    factory = WebSocketServerFactory(u'ws://127.0.0.1:2222')
    factory.protocol = AudioServerProtocol

    reactor.listenTCP(2222, factory)
    reactor.run()
