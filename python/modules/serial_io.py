import serial
import struct


class ArduinoSerial:
    """Class for communicating with an Arduino over a serial connection"""

    def __init__(self, port: str, commands: dict = {}, baudrate: int = 9600):
        try:
            self.ser = serial.Serial(port, baudrate)
        except serial.SerialException:
            print(
                f'\u001b[31mFailed to connect to serial port \'{port}\'\u001b[0m')

        self.commands = commands

    def send_str(self, data: str):
        """Sends string to the serial port"""

        # Send data to the serial port
        self.ser.write(data.encode()+b'\n')
        # Print the data sent to the console
        print(f'\u001b[32m<< \'{data}\'\u001b[0m')

    def read_str(self) -> list[str]:
        """Reads data from the serial buffer and returns it as a list of strings for each line"""

        # Check if there is data in the serial buffer
        if self.ser.in_waiting > 0:
            # Read the data from the serial buffer
            data = self.ser.read(self.ser.in_waiting)
            data = data.strip().decode().split('\n')
            lines = [line.strip() for line in data]
            # Print the data received to the console
            for line in lines:
                print(f'\u001b[94m>> \'{line}\'\u001b[0m')
            return lines
        else:
            # Return an empty list if there is no data in the serial buffer
            return []

    def send_command(self, command_name: str, args: tuple):
        """Sends a command to the Arduino"""

        # Check if the command exists
        if command_name not in self.commands.keys():
            raise ValueError(f'Command \'{command_name}\' does not exist')

        command_bytes = self.commands[command_name]

        for arg in args:
            if type(arg) == int:
                command_bytes += self.__long_to_bytes(arg)
            elif type(arg) == float:
                command_bytes += self.__float_to_bytes(arg)
            else:
                raise TypeError(f'Argument \'{arg}\' is not a valid type')

        command_bytes = len(command_bytes).to_bytes(2, 'big') + command_bytes

        print(command_bytes, len(command_bytes), command_bytes.hex(), sep='\n')

    def _send_bytes(self, data: bytes):
        """Sends bytes to the serial port"""

        # Send data to the serial port
        self.ser.write(data)
        # Print the data sent to the console
        print(f'\u001b[32m<< \'{data}\'\u001b[0m')

    def _long_to_bytes(self, val: int) -> bytes:
        """Converts a long int to bytes"""

        # check if the value is an integer
        if type(val) != int:
            raise TypeError('Value is not an integer')
        # check if the value is within the range of a 32-bit signed integer
        if (val > 2147483647 or val < -2147483648):
            raise ValueError('Value is too large or too small')
        else:
            return struct.pack('>l', val)

    def _bytes_to_long(self, val: bytes) -> int:
        """Converts bytes to a long int"""

        return struct.unpack('>l', val)[0]

    def _float_to_bytes(self, val: float) -> bytes:
        """Converts a float to bytes (4 bytes)"""

        # check if the value is a float
        if type(val) != float:
            raise TypeError('Value is not a float')
        else:
            return struct.pack('>f', val)

    def _bytes_to_float(self, val: bytes) -> float:
        """Converts bytes to a float"""

        return struct.unpack('>f', val)[0]

    def close(self):
        """Closes the serial connection"""

        self.ser.close()
        print('Serial connection closed')
