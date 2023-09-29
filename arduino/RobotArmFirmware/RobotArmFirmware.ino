#include <AccelStepper.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Encoder.h>
#include <SPI.h>
#include <Wire.h>
#include <vector>

// OLED display
#define SCREEN_WIDTH 128 // OLED display width
#define SCREEN_HEIGHT 64 // OLED display height
#define SCREEN_ADDRESS 0x3C
#define OLED_RESET -1

// Rotary encoder
#define ROTARY_ENCODER_PIN_A 40  // CLK
#define ROTARY_ENCODER_PIN_B 41  // DT
#define ROTARY_ENCODER_PIN_SW 39 // SW
#define KNOB_BUTTON_ACTIVE LOW
int knobPos = 0;
boolean knobPressed = false;

// Limit switches
#define LIMIT_SWITCH_ACTIVE HIGH

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

        // Set the default stepper properties
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

    /// @brief Skip homing the joint
    void skipHome()
    {
        homed = true;
        Serial.println("WARN: " + name + " Homing skipped");
    }

    /// @brief Move the joint x steps
    void moveSteps(long steps)
    {
        stepper.move(steps);
    }

    /// @brief Set the speed of the stepper in degrees per second
    void setSpeed(float floatSpeed)
    {
        // convert degrees per second to steps per second
        long speed = static_cast<long>(floatSpeed / 360 * stepsFullRot * ratio + 0.5);
        stepper.setSpeed(speed);
    }

    /// @brief Set the acceleration of the stepper in degrees per second per second
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

    /// @brief Run the stepper
    void run()
    {
        stepper.run();
    }
};

class MappedEncoder
{
private:
    int pos;
    int min;
    int max;
    Encoder knob;

public:
    // Constructor
    MappedEncoder(int A, int B) : knob(A, B)
    {
        pos = 0;
        min = 0;
        max = 16; // 0-15
    }

    // Method to read the knob value
    int read()
    {
        return knob.read() / -2;
    }

    // Method to reset the knob value
    void reset()
    {
        knob.write(0);
    }

    // Method to set knob limits
    void setLimits(int min, int max)
    {
        this->min = min;
        this->max = max;
    }

    // method to read the knob value and map it to the limits
    int readMapped()
    {
        int value = this->read();
        return value % (max - min) + min;
    }
};

/// @brief  Class for controlling a stepper motor for a joint

JointStepper DOF1("DOF1", 34, 33, 14, -1, 1, -1, 90, 200, 8, 23.0, 0.0, 0.0);
JointStepper DOF2("DOF2", 36, 35, 16, 15, 1, 1, 0, 200, 8, 1.0, 0.0, 360.0);
JointStepper DOF3("DOF3", 38, 37, 18, 17, 1, -1, 0, 200, 8, 1.0, 0.0, 360.0);
JointStepper DOF4("DOF4", 28, 27, 22, -1, 1, 1, 0, 200, 8, 1.0, 0.0, 360.0);
JointStepper DOF5("DOF5", 30, 29, 21, 20, 1, 1, 0, 200, 8, 1.0, 0.0, 360.0);
JointStepper DOF6("DOF6", 32, 31, 19, -1, 1, 1, 0, 200, 8, 1.0, 0.0, 360.0);

JointStepper *DOFs[7] = {nullptr, &DOF1, &DOF2, &DOF3, &DOF4, &DOF5, &DOF6};

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire2, OLED_RESET);
MappedEncoder knob(ROTARY_ENCODER_PIN_A, ROTARY_ENCODER_PIN_B);

void drawFrame()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("DOF 1");
    display.display();
}

int currentDOF = 1;

void setup()
{
    // Serial port
    Serial.begin(9600);
    Serial.println("Serial port initialized");

    // OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println("SSD1306 allocation failed");
        for (;;)
            ;
    }
    Serial.println("OLED display initialized");

    display.clearDisplay();

    // Rotary encoder
    knob.setLimits(0, 6);
    knob.reset();

    Serial.println("Setup complete");
}

void loop()
{
    // Read Knob
    int knobPos = knob.readMapped();

    // Move DOF
    if (knobPos != 0)
    {
        DOFs[currentDOF]->moveSteps(knobPos * 500);
        DOFs[currentDOF]->run();
        knob.reset();
    }

    // Change DOF
    if (!knobPressed)
    {
        knobPressed = (digitalRead(ROTARY_ENCODER_PIN_SW) == KNOB_BUTTON_ACTIVE);
        if (knobPressed)
        {
            Serial.println("Changing DOF");

            currentDOF++;
            if (currentDOF > 6 || currentDOF < 1)
            {
                currentDOF = 1;
            }
        }
    }
    else
    {
        knobPressed = (digitalRead(ROTARY_ENCODER_PIN_SW) == KNOB_BUTTON_ACTIVE);
    }

    // TODO: Add GUI to display current DOF and position
}