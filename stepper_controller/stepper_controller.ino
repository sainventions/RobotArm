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
    float minPosition;  // Min Degrees
    float maxPosition;  // Max Degrees
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
        float minPosition, float maxPosition)
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
        this->minPosition = minPosition;
        this->maxPosition = maxPosition;
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
        halt(); // Stop the motor
        // TODO: move back a bit

        position = 0; // Home position is always 0
        homed = true;
        Serial.println("INFO: " + name + " Homed");
    }

    /// @brief Skip homing the joint
    void skipHome()
    {
        homed = true;
        Serial.println("WARN: " + name + " Homing skipped");
    }

    /// @brief  set the current position of the AccelStepper object to 0 and update the position variable to the current position converted from steps to degrees
    void updatePos()
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

        // Check if the target is within the limits
        if (target > maxPosition || target < minPosition)
        {
            Serial.println("ERROR: " + name + " Out of limits");
            return;
        }

        // Calculate the steps as a float
        long steps = static_cast<long>((target - position) / 360 * stepsFullRot * ratio + 0.5);

        // set the target position
        stepper.move(steps);
    }

    /// @brief Set the speed of the stepper in degrees per second
    // TODO: make this a float
    void setSpeed(float floatSpeed)
    {
        // convert degrees per second to steps per second
        long speed = static_cast<long>(floatSpeed / 360 * stepsFullRot * ratio + 0.5);
        stepper.setSpeed(speed);
    }

    /// @brief Set the acceleration of the stepper in degrees per second per second
    // TODO: make this a float
    void setAcceleration(float floatAcceleration)
    {
        long acceleration = static_cast<long>(floatAcceleration / 360 * stepsFullRot * ratio + 0.5);
        stepper.setAcceleration(acceleration);
    }

    /// @brief Halts the stepper to its current position instantly
    void halt()
    {
        long currentPos = stepper.currentPosition();
        stepper.move(currentPos); // set target position to current position
    }

    /// @brief Run the stepper in the event loop. This just passes through to the AccelStepper run() method and updates the position variable
    void run()
    {
        // Update the position variable
        position = static_cast<float>(stepper.currentPosition()) / stepsFullRot * 360 / ratio;

        // Run the stepper
        stepper.run();
    }

    /// @brief Test the joint by rotating 16 full rotations. This method is blocking; only use it for testing
    void test()
    {
        stepper.setCurrentPosition(0);
        stepper.setAcceleration(2000);
        stepper.setMaxSpeed(24000);

        stepper.move(stepsFullRot * 16);
        stepper.runToPosition();
    }
};

/// @brief Extended String class to add split() method
class CommandString : public String
{
public:
    CommandString() : String() {}
    CommandString(String str) : String(str) {}

    std::vector<CommandString> split(char delimiter)
    {
        std::vector<CommandString> substrings;
        int start = 0;
        int target = indexOf(delimiter, start);
        while (target != -1)
        {
            substrings.push_back(substring(start, target));
            start = target + 1;
            target = indexOf(delimiter, start);
        }
        substrings.push_back(substring(start));
        return substrings;
    }
};

/* Motor Connections
    stepPin, dirPin, limitPinA, limitPinB, dir, homeDir, stepResolution, microstep, ratio, minPosition, maxPosition

    |    | Step | Dir | LimitA | LimitB |
    |----|------|-----|--------|--------|
    | S1 | 34   | 33  | 14     | -1     |
    | S2 | 36   | 35  | 16     | 15     |
    | S3 | 38   | 37  | 18     | 17     |
    | S4 | 28   | 27  | 22     | -1     |
    | S5 | 30   | 29  | 21     | 20     |
    | S6 | 32   | 31  | 19     | -1     |
*/

JointStepper DOF1("DOF1", 34, 33, 14, -1, 0, 0, 200, 8, 1, 0, 360.0);
JointStepper DOF2("DOF2", 36, 35, 16, 15, 0, 0, 200, 8, 1, 0, 360.0);
JointStepper DOF3("DOF3", 38, 37, 18, 17, 0, 0, 200, 8, 1, 0, 360.0);
JointStepper DOF4("DOF4", 28, 27, 22, -1, 0, 0, 200, 8, 1, 0, 360.0);
JointStepper DOF5("DOF5", 30, 29, 21, 20, 0, 0, 200, 8, 1, 0, 360.0);
JointStepper DOF6("DOF6", 32, 31, 19, -1, 0, 0, 200, 8, 1, 0, 360.0);

JointStepper *DOFs[7] = {nullptr, &DOF1, &DOF2, &DOF3, &DOF4, &DOF5, &DOF6};

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
    // Prepare the arguments
    String commandName = arguments[0];
    arguments.erase(arguments.begin());
    int argumentCount = static_cast<int>(arguments.size());

    // test
    if (commandName == "test")
    {
        Serial.println("INFO: Test\n" + String(argumentCount) + " arguments:");
        for (int i = 0; i < argumentCount; i++)
        {
            Serial.println(arguments[i]);
        }
    }

    // home
    else if ((commandName == "home" || commandName == "h") && argumentCount >= 1 && argumentCount <= 6)
    {
        if (argumentCount == 1 && arguments[0] == "all")
        {
            for (int i = 1; i <= 6; i++)
            {
                DOFs[i]->home();
            }
        }
        else
        {
            for (int i = 0; i < argumentCount; i++)
            {
                if (String("123456").indexOf(arguments[i].charAt(0)) != -1)
                {
                    DOFs[arguments[i].toInt()]->home();
                }
            }
        }
    }

    // skiphome
    else if ((commandName == "skiphome" || commandName == "sh") && argumentCount >= 1 && argumentCount <= 6)
    {
        if (argumentCount == 1 && arguments[0] == "all")
        {
            for (int i = 1; i <= 6; i++)
            {
                DOFs[i]->skipHome();
            }
        }
        else
        {
            for (int i = 0; i < argumentCount; i++)
            {
                if (String("123456").indexOf(arguments[i].charAt(0)) != -1)
                {
                    DOFs[arguments[i].toInt()]->skipHome();
                }
            }
        }
    }

    // settarget
    else if ((commandName == "settarget" || commandName == "st") && argumentCount == 2)
    {
        if (String("123456").indexOf(arguments[0].charAt(0)) != -1)
        {
            DOFs[arguments[0].toInt()]->setTarget(arguments[1].toFloat());
            Serial.println("INFO: Set target of " + arguments[0] + " to " + arguments[1]);
        }
        else{
            Serial.println("ERROR: Invalid DOF");
        }
    }

    // setspeed
    else if ((commandName == "setspeed" || commandName == "ss") && argumentCount == 2)
    {
        if (String("123456").indexOf(arguments[0].charAt(0)) != -1)
        {
            DOFs[arguments[0].toInt()]->setSpeed(arguments[1].toFloat());
        }
    }

    // setaccel
    else if ((commandName == "setaccel" || commandName == "sa") && argumentCount == 2)
    {
        if (String("123456").indexOf(arguments[0].charAt(0)) != -1)
        {
            DOFs[arguments[0].toInt()]->setAcceleration(arguments[1].toFloat());
        }
    }

    // halt
    else if (commandName == "halt" && argumentCount == 1)
    {
        if (argumentCount == 1 && arguments[0] == "all")
        {
            for (int i = 1; i <= 6; i++)
            {
                DOFs[i]->halt();
            }
        }
        else
        {
            for (int i = 0; i < argumentCount; i++)
            {
                if (String("123456").indexOf(arguments[i].charAt(0)) != -1)
                {
                    DOFs[arguments[i].toInt()]->halt();
                }
            }
        }
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

    DOF1.run();
}