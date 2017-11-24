from playsound import playsound
from raspi.e_ink_controller import EInkController
from datetime import datetime


class BOCSPuzzle:

    is_solved = False

    def __init__(self, stat_server, update_io_state):
        # Save the I/O update callback for use later
        self.update_io_state = update_io_state

        # Initialize a state for the e-ink display
        self.eink = EInkController()

        # Save the connection to the statistic and monitoring server for reporting attempts later
        self.server = stat_server

        # Save the start time
        self.start_time = datetime.now()

    def play_sound(self, path):
        """
        Plays a sound over the speakers.
        :param path: the filesystem path or web URL to the sound file
        """
        playsound(path)

    def get_elapsed_seconds(self):
        """
        Get the number of seconds elapsed since the puzzle first appeared on the screen.
        :return: the number of elapsed seconds
        """
        return (datetime.now() - self.start_time).total_seconds()

    def report_attempt(self, puzzle_id, guess=None, name=None):
        """
        Sends a puzzle answer attempt to the stat server.
        :param puzzle_id: the ID of the puzzle (as defined by its creator)
        :param guess: (optional) the player's guess. Leave empty if the correct answer was entered.
        :param name: (optional) the player's name (or pseudonym)
        """
        data = {
            'msg_type': 'attempt',
            'puzzle_id': puzzle_id,
            'elapsed_time': self.get_elapsed_seconds()
        }

        if guess:
            data['guess'] = guess

        if name:
            data['name'] = name

        if self.server:
            self.server.send_data(data)
