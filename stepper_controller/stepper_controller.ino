#include <AccelStepper.h>

/// @brief  Class for controlling a stepper motor for a joint
class JointStepper
{
public:
    // Static variables
    String name;        // Name of the joint
    int driverType;     // Type 1; with 2 pins
    int stepPin;        // Always used
    int dirPin;         // Always used
    int limitPinA;      // -1 if not used
    int limitPinB;      // Always used
    int dir;            // 0 = CW, 1 = CCW
    int homeDir;        // 0 = CW, 1 = CCW
    int stepResolution; // Steps per revolution
    int microstep;      // Microstep resolution (1, 2, 4, 8, 16)
    float ratio;        // Gear ratio output/input; ratio of 2 means the output shaft rotates twice as much as the input shaft
    float maxPosition;  // Max Degrees
    float minPosition;  // Min Degrees
    int stepsFullRot;   // Steps to complete a full rotation of the motor

    // Dynamic variables
    bool homed;
    float position; // last recorded position of output shaft in degrees

    // Stepper object
    AccelStepper stepper;

    /// @brief Create a JointStepper object
    JointStepper(
        String name,
        int stepPin, int dirPin, int limitPinA, int limitPinB,
        int dir, int homeDir,
        int stepResolution, int microstep, float ratio,
        float maxPosition, float minPosition)
    {
        this->name = name;
        this->driverType = 1;
        this->stepPin = stepPin;
        this->dirPin = dirPin;
        this->limitPinA = limitPinA;
        this->limitPinB = limitPinB;
        this->dir = dir;
        this->homeDir = homeDir;
        this->stepResolution = stepResolution;
        this->microstep = microstep;
        this->ratio = ratio;
        this->maxPosition = maxPosition;
        this->minPosition = minPosition;
        this->homed = false;
        this->stepsFullRot = stepResolution * microstep;
        // gives steps to complete a full rotation of the output shaft

        // Create the AccelStepper object
        this->stepper = AccelStepper(driverType, stepPin, dirPin);
    }

    /// @brief get the current position of the joint in degrees
    /// @return position in degrees
    float get_position()
    {
        return position;
    }

    /// @brief Home the joint by moving to the limit switch and then back a bit
    void home()
    {
        // Move to the limit switch
        stepper.setSpeed(100);
        while (digitalRead(limitPinA) == LOW)
        {
            stepper.runSpeed();
        }
        Serial.println("Home Sensor Pressed");
        halt(); // Stop the motor
        // move back a bit

        position = 0; // Home position is always 0
        homed = true;
    }

    /// @brief Skip homing the joint
    void skip_home()
    {
        homed = true;
        Serial.println("WARN: " + name + " Homing skipped");
    }

    /// @brief  set the current position of the AccelStepper object to 0 and update the position variable to the current position converted from steps to degrees
    void record_position()
    {
        position += static_cast<float>(stepper.currentPosition()) / stepsFullRot * 360 * ratio;
        stepper.setCurrentPosition(0);
    }

    void setTarget(float target)
    {
        // Check if homed
        if (!homed)
        {
            Serial.println("ERROR: " + name + " Not homed");
            return;
        }

        // Check if the position is within the limits
        if (target > maxPosition || target < minPosition)
        {
            Serial.println("ERROR: " + name + " Out of limits");
            return;
        }

        // Calculate the steps
        float steps = (target - position) * stepsFullRot / 360 * ratio;

        // Move the stepper
        record_position(); // Record the current position
        stepper.move(steps);
    }

    /// @brief Halts the stepper to its current position instantly
    void halt()
    {
        record_position(); // Record the current position
        stepper.move(0);   // set target position to current position
    }

    void stop()
    {
        stepper.stop();
    }

    /// @brief Run the stepper in the event loop. This just passes through to the AccelStepper run() method
    void run()
    {
        stepper.run();
    }

    /// @brief Test the joint by rotating 16 full rotations. This method is blocking
    void test()
    {
        stepper.setCurrentPosition(0);
        stepper.setAcceleration(2000);
        stepper.setMaxSpeed(24000);

        stepper.move(stepsFullRot * 16);
        stepper.runToPosition();
    }

    /// @brief Test 2 steppers at once by rotating them 16 rotations. This will set the speed and acceleration of the stepper and must be used in conjunction with the run() method in an event loop. This method is non-blocking
    void test2()
    {
        stepper.setCurrentPosition(0);
        stepper.setAcceleration(2000);

        stepper.setMaxSpeed(24000);
        stepper.move(stepsFullRot * 16);
    }
};

void parse_command(String command)
{
        if (command == "HOME")
        {
            // Home all joints
            // dof1.home();
            // dof2.home();
            // dof3.home();
            // dof4.home();
            // dof5.home();
            // dof6.home();

            // Send confirmation
            Serial.println("INFO: HOMED");
        }
        else if (command == "TEST")
        {
            dof1.test();
            Serial.println("INFO: TEST DONE");
        }
        else if (command == "TEST2")
        {
            dof1.test2();
            dof2.test2();
            Serial.println("INFO: TEST2 DONE");
        }
        else if (command == "STOP")
        {
            dof1.halt();
            dof2.halt();
            Serial.println("INFO: STOP DONE");
        }
        else
        {
            // Send error
            Serial.println("ERROR: INVALID COMMAND");
        }
}

// stepPin, dirPin, limitPinA, limitPinB, dir, homeDir, stepResolution, microstep, ratio, maxPosition, minPosition
/*
    |    | Step | Dir | Limit_A | Limit_B |
    |----|------|-----|---------|---------|
    | S1 | 34   | 33  | 14      | -1      |
    | S2 | 36   | 35  | 16      | 15      |
    | S3 | 38   | 37  | 18      | 17      |
    | S4 | 28   | 27  | 22      | -1      |
    | S5 | 30   | 29  | 21      | 20      |
    | S6 | 32   | 31  | 19      | -1      |
*/

JointStepper dof1("DOF1", 34, 33, 14, -1, 0, 0, 200, 8, 1, 0, 0);
JointStepper dof2("DOF2", 36, 35, 16, 15, 0, 0, 200, 8, 1, 0, 0);
JointStepper dof3("DOF3", 38, 37, 18, 17, 0, 0, 200, 8, 1, 0, 0);
JointStepper dof4("DOF4", 28, 27, 22, -1, 0, 0, 200, 8, 1, 0, 0);
JointStepper dof5("DOF5", 30, 29, 21, 20, 0, 0, 200, 8, 1, 0, 0);
JointStepper dof6("DOF6", 32, 31, 19, -1, 0, 0, 200, 8, 1, 0, 0);

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    // check if data is available
    if (Serial.available())
    {
        // read data
        String data = Serial.readStringUntil('\n');

        
    }

    dof1.run();
    dof2.run();
}