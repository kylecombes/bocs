from raspi.io_states.e_ink_state import EInkState
try:
    from papirus import PapirusComposite
    eink_present = True
except ImportError:
    eink_present = False


class EInkController:

    def __init__(self, rotation=0):

        if eink_present:
            # Initialize the screen such that it does not update until WriteAll() is called
            self.rotation = rotation

        self.state = EInkState()

    def set_image(self, path, width=264, height=176, xpos=0, ypos=0):
        """
        Sets the image to display on the screen.
        :param file_path: the path to the image (relative to the raspi directory)
        :param width: the width of the image (in pixels) - defaults to full width
        :param height: the height of the image (in pixels) - defaults to full height
        :param xpos: the x-offset for the top-left corner of the image
        :param ypos: the y-offset for the top-left corner of the image
        """
        self.state.set_image(path, width, height, xpos, ypos)
        self.update_display()

    def set_text(self, text, xpos=0, ypos=0):
        """
        Sets the text to display on the screen. Use newlines (\n) to break into multiple lines.
        :param text: the text to display
        :param xpos: the x-offset for the top-left corner of the text
        :param ypos: the y-offset for the top-left corner of the text
        """
        self.state.set_text(text, xpos, ypos)
        self.update_display()

    def update_state(self, new_state, full_refresh=True):
        self.state = new_state

    def update_display(self):
        """
        Writes the image and text (if any) stored in the state to the display.
        """

        # Get the set text and image (if any)
        img, img_width, img_height, img_xpos, img_ypos = self.state.get_image()
        text, text_xpos, text_ypos = self.state.get_text()

        if eink_present:  # There is an e-ink display connected
            screen = PapirusComposite(False, self.rotation)
            if img:  # There is an image to display
                screen.AddImg(img, img_xpos, img_ypos, (img_width, img_height))

            if text:
                screen.AddText(text, text_xpos, text_ypos)

            # Write the updates to the display
            screen.WriteAll()

        else:  # There is no e-ink display connected, so just print to the console
            if img:
                print('Displaying image: {}'.format(img))

            if text:  # There is text to display
                print('Displaying text: {}'.format(text))

    def clear(self):
        self.state = EInkState()
