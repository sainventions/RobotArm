import logging
import threading
import tkinter as tk
import time

from modules.robot_arm import RobotArm
from modules.serial_io import ArduinoSerial

# SERIAL_PORT = 'usb:0/140000/0/8/4'
SERIAL_PORT = 'COM10'
# SERIAL_PORT = '/dev/serial/by-id/usb-Teensyduino_USB_Serial_1400000-if00'


def read_loop(serial: ArduinoSerial):
    """Reads data from the serial buffer and prints it to the console"""
    while True:
        serial.read()
        time.sleep(0.1)  # Sleep to prevent the loop from running too fast

        # Execute any commands received
        # TODO: Implement command execution


def ui_loop(serial: ArduinoSerial, robot_arm: RobotArm):
    """Creates the GUI and runs the main loop"""

    def home_all():
        """Homes all axes"""
        robot_arm.home()

    def test_output():
        serial.send('TESTOUTPUT')

    # Create the main window and set its properties
    root = tk.Tk()
    root.title("My GUI")

    # Create the buttons and assign the functions to be called when clicked
    button1 = tk.Button(root, text='Home Axes', command=home_all)
    button2 = tk.Button(root, text='Test Output', command=test_output)

    # Pack the buttons into the window
    button1.pack()
    button2.pack()

    # Run the main loop
    root.mainloop()


if __name__ == '__main__':
    # Create serial connection
    serial = ArduinoSerial(SERIAL_PORT)
    print('Serial connection established')

    # Create and initialize robot arm
    robot_arm = RobotArm(serial)
    print('Robot arm initialized')

    # Create and run reader thread
    read_thread = threading.Thread(target=read_loop, args=(serial,))
    read_thread.start()
    print('Reader thread started')

    time.sleep(1)  # Sleep to allow the reader thread to start

    # Create and run UI thread
    ui_thread = threading.Thread(target=ui_loop, args=(serial, robot_arm))
    ui_thread.start()
    print('UI thread started')

    # Wait for the threads to finish
    read_thread.join()
    ui_thread.join()

    # Close the serial connection
    serial.close()
