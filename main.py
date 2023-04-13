import logging
import threading
import tkinter as tk

from modules.robot_arm import RobotArm
from modules.serial_io import ArduinoSerial

SERIAL_PORT = 'COM9'


def read_loop(serial: ArduinoSerial):
    while True:
        serial.read()


def ui_loop(serial: ArduinoSerial):
    def command_1():
        serial.send('command 1')

    def command_2():
        serial.send('command 2')

    def command_3():
        serial.send('command 3')

    # Create the main window and set its properties
    root = tk.Tk()
    root.title("My GUI")

    # Create the buttons and assign the functions to be called when clicked
    button1 = tk.Button(root, text="Command 1", command=command_1)
    button2 = tk.Button(root, text="Command 2", command=command_2)
    button3 = tk.Button(root, text="Command 3", command=command_3)

    # Pack the buttons into the window
    button1.pack()
    button2.pack()
    button3.pack()

    # Run the main loop
    root.mainloop()


if __name__ == '__main__':
    serial = ArduinoSerial(SERIAL_PORT)

    # Create and run reader thread
    read_thread = threading.Thread(target=read_loop, args=(serial,))
    read_thread.start()

    # Create and run UI thread
    ui_thread = threading.Thread(target=ui_loop, args=(serial,))
    ui_thread.start()
