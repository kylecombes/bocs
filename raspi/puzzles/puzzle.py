import os
from playsound import playsound
from raspi.e_ink_controller import EInkController
from datetime import datetime
import time


class BOCSPuzzle:

    is_solved = False

    def __init__(self, init_bundle):
        # Save the I/O update callback for use later
        self.update_io_state = init_bundle.get('update_io_state', None)

        # Initialize a state for the e-ink display
        self.eink = EInkController()

        # Save the connection to the statistic and monitoring server for reporting attempts later
        self.telemetry_server = init_bundle.get('telem_server', None)

        self.sound_server = init_bundle.get('sound_server', None)

        # Save the start time
        self.start_time = datetime.now()

    def play_sound(self, path):
        """
        Plays a sound on the sound server, if connected, or through any connected speakers.
        :param path: the filesystem path to the sound file
        :return True if the sound was played successfully, False otherwise
        """
        if self.sound_server:
            self.sound_server.send_data({'command': 'play', 'filename': path})
        else:
            if os.path.exists(path):
                playsound(path)
            else:
                return False
        return True

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

        if self.telemetry_server:
            self.telemetry_server.send_data(data)

    @staticmethod
    def pause(seconds):
        """
        Pauses everything for the given time
        :param seconds: the number of seconds to pause for
        """
        time.sleep(seconds)
