from raspi.arduino_comm import ArduinoComm


class BOCSMain:

    do_run = True

    def __init__(self):
        self.ac = ArduinoComm(self.event_callback)

    def event_callback(self, event):
        print('Message received')
        if event.data:
            print(event.data)
        if event.id == 0:
            print('Exiting...')
            self.ac.stop_listening()


if __name__ == '__main__':
    BOCSMain()
