import serial
import logging


class ArduinoSerial:
    """Class for communicating with an Arduino over a serial connection

    Args:
        port (str): The serial port to connect to
        baudrate (int): The baudrate to use (default: 9600)

    Methods:
        send(data: str): Sends data to the serial port
        read(): Reads data from the serial buffer and returns it as a list of strings for each line
        close(): Closes the serial connection
    """

    def __init__(self, port: str, baudrate: int = 9600):
        self.ser = serial.Serial(port, baudrate)

    def send(self, data: str):
        """Sends data to the serial port"""

        # Send data to the serial port
        self.ser.write(data.encode()+b'\n')
        # Print the data sent to the console
        print(f'\u001b[32m>> \'{data}\'\u001b[0m')

    def read(self) -> list[str]:
        """Reads data from the serial buffer and returns it as a list of strings for each line"""

        # Check if there is data in the serial buffer
        if self.ser.in_waiting > 0:
            # Read the data from the serial buffer
            data = self.ser.readline().decode().strip().split('\n')
            # Print the data received to the console
            for line in data:
                print(f'\u001b[94m<< \'{line}\'\u001b[0m')
            return data
        else:
            # Return an empty list if there is no data in the serial buffer
            return []

    def close(self):
        """Closes the serial connection"""

        self.ser.close()
        print('Serial connection closed')
