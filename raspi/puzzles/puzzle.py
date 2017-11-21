from raspi.e_ink_controller import EInkController


class BOCSPuzzle:

    is_solved = False

    def __init__(self, update_io_state):
        # Save the I/O update callback for use later
        self.update_io_state = update_io_state

        # Initialize a state for the e-ink display
        self.eink = EInkController()
