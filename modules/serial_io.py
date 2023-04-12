import serial


class ArduinoSerial:
    def __init__(self, port: str, baudrate: int = 9600):
        self.ser = serial.Serial(port, baudrate)

    def send(self, data: str):
        self.ser.write(data.encode())
        print(f'\u001b[32m>> \'{data}\'\u001b[0m')

    def read(self) -> str:
        data = self.ser.readline().decode().strip()
        print(f'\u001b[94m<< \'{data}\'\u001b[0m')
        print('read ')
        return data

    def close(self):
        self.ser.close()
        print('Serial connection closed')


if __name__ == '__main__':
    arduino = ArduinoSerial('COM9')
    while True:
        data = input('Enter data: ')
        arduino.send(data)
        arduino.read()
