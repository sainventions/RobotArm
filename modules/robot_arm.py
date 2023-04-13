from modules.serial_io import ArduinoSerial


class RobotArm:
    def __init__(self, serial: ArduinoSerial):
        self.serial = serial

    def home(self, axes: list = [1, 2, 3, 4, 5, 6]):
        command = ' '.join(['HOME'] + [str(axis) for axis in axes])
        self.serial.send(command)
        self.serial.read()
        print('Homed')


if __name__ == '__main__':
    robot_arm = RobotArm('COM9')
    robot_arm.home()
    robot_arm.home(axes=[1, 3, 5])
    print('Done')
