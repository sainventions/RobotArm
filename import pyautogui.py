import pyautogui

last_x = 0
while True:
        x, y = pyautogui.position()
        mapped_x = round((x / 3839) * 360, 4)

        if mapped_x != last_x:
                print(mapped_x)
                last_x = mapped_x