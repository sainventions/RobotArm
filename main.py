from modules.robotarm import RobotArm

def main():
    robot_arm = RobotArm('COM9')
    robot_arm.home()
    robot_arm.home(axes=[1, 3, 5])
    print('Done')
    
    


if __name__ == '__main__':
    main()

