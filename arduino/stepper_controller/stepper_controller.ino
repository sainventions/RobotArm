#include <AccelStepper.h>
#include <vector>

#define LIMIT_SWITCH_ACTIVE HIGH

/// @brief  Class for controlling a stepper motor for a joint
class JointStepper
{
public: // TODO: make private and add getters and setters for all variables
    // Static variables
    String name;        // Name of the joint
    int driverType;     // Type 1; with 2 pins
    int stepPin;        // Always used
    int dirPin;         // Always used
    int limitPinA;      // Always used
    int limitPinB;      // -1 if not used
    int dir;            // Direction of positive movement 1 = CW, -1 = CCW
    int homeDir;        // Direction of homing (not relative to actual dir) 1 = CW, -1 = CCW
    float homePos;      // Position of the limit switch activation
    int stepResolution; // Steps per revolution
    int microstep;      // Microstep resolution (1, 2, 4, 8, 16)
    float ratio;        // Gear ratio output/input; ratio of 2 means the output shaft rotates half as much as the input shaft (indended for reduction gearboxes)
    float minPosition;  // Min Degrees (unrestricted if both min and max are 0)
    float maxPosition;  // Max Degrees (unrestricted if both min and max are 0)
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
        int dir, int homeDir, float homePos,
        int stepResolution, int microstep, float ratio,
        float minPosition, float maxPosition)
    {
        this->name = name;

        this->driverType = AccelStepper::DRIVER;
        this->stepPin = stepPin;
        this->dirPin = dirPin;
        this->limitPinA = limitPinA;
        this->limitPinB = limitPinB;
        this->dir = dir;
        this->homeDir = homeDir;
        this->homePos = homePos;
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

        // Set the stepper properties
        stepper.setMaxSpeed(8000);
        stepper.setAcceleration(1000);
    }

    /// @brief Home the joint by moving to the limit switch and then back a bit
    void home()
    {
        // Move to the limit switch
        Serial.println("Homing " + name + "...");
        stepper.setSpeed(1000 * homeDir);
        stepper.setAcceleration(1000);

        // Move until limit switch is activated
        while (digitalRead(limitPinA) == LIMIT_SWITCH_ACTIVE)
        {
            stepper.runSpeed();
            Serial.println("Moving to limit switch");
        }

        Serial.println("Limit switch activated");

        stepper.setSpeed(0); // stop the motor
        stepper.setCurrentPosition(0);

        // home other direction if exists
        if (limitPinB != -1)
        {
            stepper.setSpeed(1000 * -homeDir);
            stepper.setAcceleration(1000);

            // Move until limit switch is activated
            while (digitalRead(limitPinB) == LIMIT_SWITCH_ACTIVE)
            {
                stepper.runSpeed();
                Serial.println("Moving to limit switch");
            }

            Serial.println("Limit switch activated");

            stepper.setSpeed(0); // stop the motor
            stepper.setCurrentPosition(0);
        }

        Serial.println("Homed");
    }

    /// @brief Move the joint x steps
    void moveSteps(long steps)
    {
        stepper.move(steps);
    }

    /// @brief Skip homing the joint
    void skipHome()
    {
        homed = true;
        Serial.println("WARN: " + name + " Homing skipped");
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
        stepper.setSpeed(0);      // set speed to 0
    }
};

#define BT_SERIAL Serial2 // Pin 7 (RX2) and 8 (TX2)

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
    Arguments:
        String name,
        int stepPin, int dirPin, int limitPinA, int limitPinB,
        int dir, int homeDir, float homePos,
        int stepResolution, int microstep, float ratio,
        float minPosition, float maxPosition

    |    | Step | Dir | LimitA | LimitB |
    |----|------|-----|--------|--------|
    | S1 | 34   | 33  | 14     | -1     |
    | S2 | 36   | 35  | 16     | 15     |
    | S3 | 38   | 37  | 18     | 17     |
    | S4 | 28   | 27  | 22     | -1     |
    | S5 | 30   | 29  | 21     | 20     |
    | S6 | 32   | 31  | 19     | -1     |
*/

JointStepper DOF1("DOF1", 34, 33, 14, -1, 1, -1, 90, 200, 8, 23.0, 0.0, 0.0);
JointStepper DOF2("DOF2", 36, 35, 16, 15, 1, 1, 0, 200, 8, 1.0, 0.0, 360.0);
JointStepper DOF3("DOF3", 38, 37, 18, 17, 1, -1, 0, 200, 8, 1.0, 0.0, 360.0);
JointStepper DOF4("DOF4", 28, 27, 22, -1, 1, 1, 0, 200, 8, 1.0, 0.0, 360.0);
JointStepper DOF5("DOF5", 30, 29, 21, 20, 1, 1, 0, 200, 8, 1.0, 0.0, 360.0);
JointStepper DOF6("DOF6", 32, 31, 19, -1, 1, 1, 0, 200, 8, 1.0, 0.0, 360.0);

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

    // home
    if ((commandName == "home" || commandName == "h") && argumentCount >= 1 && argumentCount <= 6)
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

    else if ((commandName == "move" || commandName == "m") && argumentCount == 2)
    {
        DOFs[arguments[0].toInt()]->moveSteps(arguments[1].toInt());
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
    if (Serial.available())
    {
        // read data
        String data = Serial.readStringUntil('\n');
        parseCommand(data);
    }
}