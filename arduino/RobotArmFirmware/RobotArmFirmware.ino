#include <Arduino.h>
#include <Encoder.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display
#define SCREEN_WIDTH 128 // OLED display width
#define SCREEN_HEIGHT 64 // OLED display height
#define SCREEN_ADDRESS 0x3C
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire2, OLED_RESET);

// TODO: create a SERIAL pointer that points to the serial port

// Rotary encoder
#define ROTARY_ENCODER_PIN_A 40  // CLK
#define ROTARY_ENCODER_PIN_B 41  // DT
#define ROTARY_ENCODER_PIN_SW 39 // SW
int knobPos = 0;
Encoder knob(ROTARY_ENCODER_PIN_A, ROTARY_ENCODER_PIN_B);

void setup()
{
    // Serial port
    Serial.begin(9600);
    Serial.println("Serial port initialized");

    // Rotary encoder
    pinMode(ROTARY_ENCODER_PIN_A, INPUT);
    pinMode(ROTARY_ENCODER_PIN_B, INPUT);
    pinMode(ROTARY_ENCODER_PIN_SW, INPUT);
    Serial.println("Rotary encoder initialized");

    // OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println("SSD1306 allocation failed");
        for (;;)
            ;
    }
    Serial.println("OLED display initialized");

    display.clearDisplay();

    Serial.println("Setup complete");
}

void loop()
{
    int pos = knob.read()/-2;
    if (pos != knobPos)
    {
        knobPos = pos;
        Serial.println(knobPos);
    }

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 28);
    display.println("Knob: " + String(knobPos));
    display.display();
    delay(100);
    display.clearDisplay();
}