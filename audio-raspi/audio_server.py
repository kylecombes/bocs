from autobahn.twisted.websocket import WebSocketServerProtocol, WebSocketServerFactory
from playsound import playsound
from os import path, environ
import subprocess
import json

MEDIA_FOLDER_PATH = environ.get('BOCS_MEDIA', False)
SHUTDOWN_PASSWORD = environ.get('BOCS_PASSWORD', False)


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

            elif command == 'shutdown' or command == 'reboot':  # Powering off RasPi, if password defined
                if not SHUTDOWN_PASSWORD:
                    print('Attempted to shut down Raspberry Pi, but no password set. Aborting...')
                    return

                # -h flag halts (shuts down) while -r restarts
                do_reboot = command == 'reboot'
                command = '/usr/bin/sudo /sbin/shutdown {} now'.format('-r' if do_reboot else '-h')

                if data.get('password', None) == SHUTDOWN_PASSWORD:
                    print("Attempting to {} Raspberry Pi (will only work if 'sudo shutdown' doesn't require "
                          "password)'...".format('reboot' if do_reboot else 'shut down'))
                    process = subprocess.Popen(command.split(), stdout=subprocess.PIPE)
                    output = process.communicate()[0]
                    print(output)
                else:
                    print('Power control authentication failed')

    def onClose(self, wasClean, code, reason):
        print("WebSocket connection closed: {}".format(reason))


if __name__ == '__main__':

    from twisted.internet import reactor

    factory = WebSocketServerFactory(u'ws://127.0.0.1:2222')
    factory.protocol = AudioServerProtocol

    reactor.listenTCP(2222, factory)
    print('Starting BOCS audio server...')
    reactor.run()
