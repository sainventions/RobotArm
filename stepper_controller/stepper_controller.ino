// yo copilot /// @breif goes before a function or class
#include <AccelStepper.h>
#include <vector>

/// @brief  Class for controlling a stepper motor for a joint
class JointStepper
{
public: // TODO: make private and add getters and setters for all variables
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
    void skipHome()
    {
        homed = true;
        Serial.println("WARN: " + name + " Homing skipped");
    }

    /// @brief  set the current position of the AccelStepper object to 0 and update the position variable to the current position converted from steps to degrees
    void recordPos()
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
        recordPos(); // Record the current position
        stepper.move(steps);
    }

    /// @brief Halts the stepper to its current position instantly
    void halt()
    {
        recordPos();     // Record the current position
        stepper.move(0); // set target position to current position
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

/// @brief Extended String class to add command processing
class CommandString : public String
{
public:
    CommandString() : String() {}
    CommandString(String str) : String(str) {}

    // Split the string into substrings using the given delimiter
    std::vector<CommandString> split(char delimiter)
    {
        std::vector<CommandString> substrings;
        int start = 0;
        int end = indexOf(delimiter, start);
        while (end >= 0)
        {
            substrings.push_back(substring(start, end - start));
            start = end + 1;
            end = indexOf(delimiter, start);
        }
        substrings.push_back(substring(start));
        return substrings;
    }
};

// Motor Connections
// stepPin, dirPin, limitPinA, limitPinB, dir, homeDir, stepResolution, microstep, ratio, maxPosition, minPosition
/*
    |    | Step | Dir | LimitA | LimitB |
    |----|------|-----|--------|--------|
    | S1 | 34   | 33  | 14     | -1     |
    | S2 | 36   | 35  | 16     | 15     |
    | S3 | 38   | 37  | 18     | 17     |
    | S4 | 28   | 27  | 22     | -1     |
    | S5 | 30   | 29  | 21     | 20     |
    | S6 | 32   | 31  | 19     | -1     |
*/

JointStepper dof1("DOF1", 34, 33, 14, -1, 0, 0, 200, 8, 1, 0, 0);
JointStepper dof2("DOF2", 36, 35, 16, 15, 0, 0, 200, 8, 1, 0, 0);
JointStepper dof3("DOF3", 38, 37, 18, 17, 0, 0, 200, 8, 1, 0, 0);
JointStepper dof4("DOF4", 28, 27, 22, -1, 0, 0, 200, 8, 1, 0, 0);
JointStepper dof5("DOF5", 30, 29, 21, 20, 0, 0, 200, 8, 1, 0, 0);
JointStepper dof6("DOF6", 32, 31, 19, -1, 0, 0, 200, 8, 1, 0, 0);

/// @brief parse and execute a command with passed through values based on the command string
/// @param rawCommand This is the string command that will be parsed
void parseCommand(String rawCommand)
{
    // Split the command into commands seperated by ";"
    CommandString command(rawCommand);
    command.trim();
    std::vector<CommandString> commands = command.split(';');

    // Loop through each command
    for (int i = 0; i < static_cast<int>(commands.size()); i++)
    {
        commands[i].trim();
        // Split the command into the command name and the arguments
        std::vector<CommandString> arguments = commands[i].split(' ');

        // Execute the command
        executeCommand(arguments);
    }
}

/// @brief execute a command based on the command string
/// @param arguments This is the vector of arguments that will be passed to the command; format: [commandName, arg1, arg2, ...]
void executeCommand(std::vector<CommandString> arguments)
{
    // Seperate the command name from the arguments
    String commandName = arguments[0];
    arguments.erase(arguments.begin());

    // select the command to execute
    if (commandName == "test")
    {
        dof1.test();
        Serial.println("INFO: Test Recieved");
    }
    else if (commandName == "test2")
    {
        dof1.test2();
        dof2.test2();
        Serial.println("INFO: Test2 Recieved");
    }
    else if (commandName == "home")
    {
    }
    else if (commandName == "skipHome")
    {
    }
    else if (commandName == "setTarget")
    {
    }
    else if (commandName == "halt")
    {
        dof1.halt();
        dof2.halt();
        Serial.println("Halt Recieved");
    }
    else if (commandName == "stop")
    {
        dof1.stop();
        dof2.stop();
        Serial.println("Stop Recieved");
    }
    else if (commandName == "run")
    {
    }
    else
    {
        Serial.println("ERROR: Command not found");
    }
}

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
        parseCommand(data);
    }

    dof1.run();
    dof2.run();
}