import threading
import tkinter as tk
import time
import pyautogui

from modules.robot_arm import RobotArm
from modules.serial_io import ArduinoSerial
from modules.ik2d import ikSolver2D

SERIAL_PORT = 'COM14'


def read_loop(serial: ArduinoSerial):
    '''Reads data from the serial buffer and prints it to the console'''
    while True:
        serial.read_str()
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
        'sh all',
        'ss 1 8000;sa 1 2000;ss 2 8000;sa 2 2000',
        'st 1 0;st 2 0',
        'st 1 90;st 2 90',
        'st 1 180;st 2 180'
    ]

    def create_command(command):
        def run_command():
            serial.send_str(command)
        return run_command

    for command in commands:
        button = tk.Button(root, text=command, command=create_command(command))
        button.pack()

    # create an input field
    command_input = tk.Entry(root)
    command_input.pack()

    def process_command(event=None):
        command = command_input.get()
        serial.send_str(command)
        command_input.delete(0, tk.END)

    command_input.bind('<Return>', process_command)

    # center the window on the screen
    screen_width = root.winfo_screenwidth()
    screen_height = root.winfo_screenheight()
    window_width = 300
    window_height = 36*len(commands)+25
    x_coord = (screen_width - window_width) // 2
    y_coord = (screen_height - window_height) // 2
    root.geometry('{}x{}+{}+{}'.format(window_width,
                  window_height, x_coord, y_coord))

    # Run the main loop
    root.mainloop()


def direct_control_loop(serial: ArduinoSerial, robot_arm: RobotArm):
    '''Runs the direct control loop'''

    serial.send_str('sh all')
    serial.send_str('setspeed 1 8000;setaccel 1 150')
    serial.send_str('setspeed 2 8000;setaccel 2 150')

    # create ik solver
    ik = ikSolver2D(96, 47)
    

    last_x = 0
    last_y = 0

    while True:
        x, y = pyautogui.position()

        if (x != last_x or y != last_y) and (x < 3840 and y < 2160) and (x > 0 and y > 0):
            # map x to -230 to 230
            # map y to 0 to 129
            mapped_x = int((x / 3840) * 250 - 125)
            mapped_y = int(((2160-y) / 2160) * 129 + 50 + 1)
            print(x, y, mapped_x, mapped_y)

            a1, a2 = ik.solve((mapped_x, mapped_y))

            if not (a1 is None or a2 is None):
                serial.send_str(f'st 1 {a1};st 2 {a2}')

        last_x = x
        last_y = y

        time.sleep(0.1)


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

    mode = input('Enter mode: ')

    if mode == 'ui':
        # Create and run UI thread
        ui_thread = threading.Thread(target=ui_loop, args=(serial, robot_arm))
        ui_thread.start()
        print('UI thread started')
    else:
        # Create and run direct control loop
        direct_control_thread = threading.Thread(
            target=direct_control_loop, args=(serial, robot_arm))
        direct_control_thread.start()
        print('Direct control thread started')
