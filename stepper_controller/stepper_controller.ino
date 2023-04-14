#include <AccelStepper.h>

// Define the stepper motor and the pins that is connected to
class JointStepper
{
public:
    // Static variables
    int driverType;
    int stepPin;
    int dirPin;
    int limitPinA;
    int limitPinB; // -1 if not used
    int dir;
    int homeDir;
    int stepResolution; // Steps per revolution
    float ratio;        // Gear ratio
    float maxPosition;  // Degrees
    float minPosition;  // Degrees
    float homePosition; // Degrees

    // Dynamic variables
    bool homed;
    float position; // Degrees

    // Stepper object
    AccelStepper stepper;

    // Constructor

    /*
    Crea
    */
    JointStepper(
        int stepPin, int dirPin, int limitPinA, int limitPinB,
        int dir, int homeDir,
        int stepResolution, float ratio,
        float maxPosition, float minPosition)
    {
        this->driverType = 1;                  // Type 1; with 2 pins
        this->stepPin = stepPin;               // Always used
        this->dirPin = dirPin;                 // Always used
        this->limitPinA = limitPinA;           // -1 if not used
        this->limitPinB = limitPinB;           // Always used
        this->dir = dir;                       // 0 = CW, 1 = CCW
        this->homeDir = homeDir;               // 0 = CW, 1 = CCW
        this->stepResolution = stepResolution; // Steps per revolution
        this->ratio = ratio;                   // Gear ratio
        this->maxPosition = maxPosition;       // Min Degrees
        this->minPosition = minPosition;       // Max Degrees
        this->homed = false;                   // Homed flag
        this->stepper = AccelStepper(driverType, stepPin, dirPin);
    }

    // Homing Method
    void home()
    {
        // Move to the limit switch
        stepper.setSpeed(100);
        while (digitalRead(limitPinA) == LOW)
        {
            stepper.runSpeed();
        }
        // Serial.println("Home Sensor Pressed");
        stepper.stop();

        position = 0; // Home position is always 0
        homed = true;
    }

    void test_360()
    {
        stepper.setAcceleration(1000);
        stepper.setCurrentPosition(0);

        stepper.setMaxSpeed(1000);
        stepper.move(stepResolution * 16);
        stepper.runToPosition();
    }
};

JointStepper dof1(34, 33, 14, -1, 0, 0, 200, 1, 0, 0);
JointStepper dof2(36, 35, 16, 15, 0, 0, 200, 1, 0, 0);
JointStepper dof3(38, 37, 18, 17, 0, 0, 200, 1, 0, 0);
JointStepper dof4(28, 27, 22, -1, 0, 0, 200, 1, 0, 0);
JointStepper dof5(30, 29, 21, 20, 0, 0, 200, 1, 0, 0);
JointStepper dof6(32, 31, 19, -1, 0, 0, 200, 1, 0, 0);

void setup()
{
    Serial.begin(9600);
    // stepPin, dirPin, limitPinA, limitPinB, dir, homeDir, stepResolution, ratio, maxPosition, minPosition
    /*
        |        | **Step** | **Dir** | **Limit_A** | **Limit_B** |
        |--------|----------|---------|-------------|-------------|
        | **S1** | 34       | 33      | 14          | -1          |
        | **S2** | 36       | 35      | 16          | 15          |
        | **S3** | 38       | 37      | 18          | 17          |
        | **S4** | 28       | 27      | 22          | -1          |
        | **S5** | 30       | 29      | 21          | 20          |
        | **S6** | 32       | 31      | 19          | -1          |
    */
}

void loop()
{
    // check if data is available
    if (Serial.available())
    {
        // read data
        String data = Serial.readStringUntil('\n');

        // Execute data command
        if (data == "HOME")
        {
            // Home all joints
            // dof1.home();
            // dof2.home();
            // dof3.home();
            // dof4.home();
            // dof5.home();
            // dof6.home();

            // Send confirmation
            Serial.println("HOMED");
        }
        else if (data == "TEST360")
        {
            dof1.test_360();
            Serial.println("TEST360 DONE");
        }
    }
}