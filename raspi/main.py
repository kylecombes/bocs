from raspi.arduino_comm import ArduinoComm
from raspi.puzzles.birthday_paradox import BirthdayParadoxPuzzle


class BOCSMain:

    do_run = True
    event_callbacks = {}
    current_puzzle = None
    current_puzzle_index = 0

    def __init__(self):
        # Initialize stuff
        self.state = BOCSState.INITIALIZING
        self.ac = ArduinoComm(self.event_callback)
        self.puzzles = [BirthdayParadoxPuzzle]

        # Run the puzzles!
        self.state = BOCSState.RUNNING
        self.current_puzzle = self.puzzles[0](self.update_io_state, self.register_callback)
        self.run_puzzles()

    def run_puzzles(self):
        while not self.state == BOCSState.STOPPING:
            if self.current_puzzle.is_solved:
                self.state = BOCSState.LOADING_NEXT_PUZZLE
                # TODO Deregister callbacks

            if self.state == BOCSState.LOADING_NEXT_PUZZLE:
                # Increment the index to the next puzzle
                self.current_puzzle_index += 1
                # Check if the last puzzle was just finished
                if self.current_puzzle_index < len(self.puzzles):
                    puzzle_class = self.puzzles[self.current_puzzle_index]
                    self.current_puzzle = puzzle_class(self.update_io_state, self.register_callback)
                else:
                    self.state = BOCSState.STOPPING
            # Sleep?

    def update_io_state(self, new_state):
        pass  # TODO Implement IO states

    def event_callback(self, event):
        if event.id in self.event_callbacks:
            for callback in self.event_callbacks[event.id]:
                callback(event)

    def register_callback(self, event_type, callback):
        # TODO Keep track of which puzzle registered the callback for unregistering automatically later
        if event_type not in self.event_callbacks:
            self.event_callbacks[event_type] = list()

        self.event_callbacks[event_type].append(callback)

    def unregister_callback(self, event_type, callback):
        if event_type in self.event_callbacks and callback in self.event_callbacks[event_type]:
            self.event_callbacks[event_type].remove(callback)


class BOCSState:
    INITIALIZING = 0
    RUNNING = 1
    LOADING_NEXT_PUZZLE = 2
    PAUSED = 3
    STOPPING = 4


if __name__ == '__main__':
    BOCSMain()
