from os import path
from raspi.io_states.io_state import IOState


class EInkState(IOState):

    image_path = None
    text = None

    def __init__(self, old_state_json=None):
        """
        Creates a new EInkState to keep track of the state attributes of the e-ink display. Currently, only an image or
        some text can be displayed. Setting both will result in only the image being displayed.
        :param old_state_json: (not currently implemented) a JSON-encoded old state to load
        """
        IOState.__init__(self, old_state_json)

    def set_image(self, file_path, width, height, xpos=0, ypos=0):
        """
        Sets the image to display on the screen.
        :param file_path: the path to the image (relative to the raspi directory)
        :param width: the width of the image (in pixels)
        :param height: the height of the image (in pixels)
        :param xpos: the x-offset for the top-left corner of the image
        :param ypos: the y-offset for the top-left corner of the image
        """

        if not file_path or width < 1 or height < 1:
            raise ValueError('File path must be defined and width and height must be greater than 0.')

        if not path.exists(file_path):  # File not found
            ValueError('Could not find "{}". Please check that the file exists.'.format(file_path))

        self._data['img'] = file_path
        self._data['img_width'] = width
        self._data['img_height'] = height
        self._data['img_xpos'] = xpos
        self._data['img_ypos'] = ypos

    def get_image(self):
        """
        Gets the image to display along with the coordinates to display it at
        :return: a tuple of the form (img_path, xpos, ypos) if the image has been set, (False, False, False, False,
            False) otherwise
        """
        if 'img' not in self._data:
            return False, False, False, False, False  # TODO Probably a better way of doing this

        return self._data['img'], self._data['img_width'], self._data['img_height'], self._data['img_xpos'], \
               self._data['img_ypos']

    def set_text(self, text, xpos=0, ypos=0):
        """
        Sets the text to display on the screen. Use newlines (\n) to break into multiple lines.
        :param text: the text to display
        :param xpos: the x-offset for the top-left corner of the text
        :param ypos: the y-offset for the top-left corner of the text
        """
        self._data['text'] = text
        self._data['text_xpos'] = xpos
        self._data['text_ypos'] = ypos

    def get_text(self):
        """
        Gets the text to display along with the coordinates to display it at
        :return: a tuple of the form (text, xpos, ypos) if the text has been set, (False, False, False) otherwise
        """
        if 'text' not in self._data:
            return False, False, False

        return self._data['text'], self._data['text_xpos'], self._data['text_ypos']
