import threading
import tkinter as tk
import time
import pyautogui

from modules.robot_arm import RobotArm
from modules.serial_io import ArduinoSerial

SERIAL_PORT = 'COM10'


def read_loop(serial: ArduinoSerial):
    '''Reads data from the serial buffer and prints it to the console'''
    while True:
        serial.read()
        time.sleep(0.1)  # Sleep to prevent the loop from running too fast

        # Execute any commands received
        # TODO: Implement command execution


def ui_loop(serial: ArduinoSerial, robot_arm: RobotArm):
    '''Creates the GUI and runs the main loop'''

    # Create the main window and set its properties
    root = tk.Tk()
    root.overrideredirect(True)
    root.wm_attributes('-topmost', True)

    commands = [
        'test',
        'halt',
        'skiphome all',
        'setspeed 1 8000;setacceleration 1 8000',
        'settarget 360',
        'settarget 0'
    ]

    def create_command(command):
        def run_command():
            serial.send(command)
        return run_command

    for command in commands:
        button = tk.Button(root, text=command, command=create_command(command))
        button.pack()

    # create an input field
    command_input = tk.Entry(root)
    command_input.pack()

    def process_command(event=None):
        command = command_input.get()
        serial.send(command)
        command_input.delete(0, tk.END)

    command_input.bind('<Return>', process_command)

    # center the window on the screen
    screen_width = root.winfo_screenwidth()
    screen_height = root.winfo_screenheight()
    window_width = 200
    window_height = 25*len(commands)+25
    x_coord = (screen_width - window_width) // 2
    y_coord = (screen_height - window_height) // 2
    root.geometry('{}x{}+{}+{}'.format(window_width,
                  window_height, x_coord, y_coord))

    # Run the main loop
    root.mainloop()


def direct_control_loop(serial: ArduinoSerial, robot_arm: RobotArm):
    '''Runs the direct control loop'''

    last_x = 0
    while True:
        x = pyautogui.position()[0]
        mapped_x = round((x / 3839) * 360, 4)

        if mapped_x != last_x and mapped_x >= 0 and mapped_x <= 360:
            serial.send(f'st 1 {mapped_x}')
            last_x = mapped_x
            time.sleep(0.5)


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
    '''
    # Create and run UI thread
    ui_thread = threading.Thread(target=ui_loop, args=(serial, robot_arm))
    ui_thread.start()
    print('UI thread started')
    '''
    # Create and run direct control loop
    serial.send('sh all')
    serial.send('setspeed 1 8000;setacceleration 1 8000')
    direct_control_thread = threading.Thread(target=direct_control_loop, args=(serial, robot_arm))
    direct_control_thread.start()
    print('Direct control thread started')
