from modules.serial_io import ArduinoSerial


class RobotArm:
    """Class for controlling the robot arm

    Args: 
        serial (ArduinoSerial): The serial connection to the Arduino

    Methods:
        home(axes: list): Homes the specified axes
    """

    def __init__(self, serial: ArduinoSerial):
        """Initializes the robot arm"""
        self.serial = serial

    def home(self, axes: list = [1, 2, 3, 4, 5, 6]):
        """Homes the specified axes

        Args:
            axes (list): The axes to home (default: [1, 2, 3, 4, 5, 6])
        """
        # command = ' '.join(['HOME'] + [str(axis) for axis in axes])
        command = 'HOME'
        self.serial.send_str(command)
        #print(f'Axes [{", ".join(str(axis) for axis in axes)}] Homed')
