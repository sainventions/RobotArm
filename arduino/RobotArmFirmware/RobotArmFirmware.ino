#include <Arduino.h>
#include <Encoder.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display
#define SCREEN_WIDTH 128 // OLED display width
#define SCREEN_HEIGHT 64 // OLED display height
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3D
#define OLED_SDA 25
#define OLED_SCL 24
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, Wire2, OLED_RESET);

// TODO: create a SERIAL pointer that points to the serial port

// Rotary encoder
#define ROTARY_ENCODER_PIN_A 40  // CLK
#define ROTARY_ENCODER_PIN_B 41  // DT
#define ROTARY_ENCODER_PIN_SW 39 // SW
int knobPos = 0;
Encoder knob(ROTARY_ENCODER_PIN_A, ROTARY_ENCODER_PIN_B);

void setup()
{

    // Rotary encoder
    pinMode(ROTARY_ENCODER_PIN_A, INPUT);
    pinMode(ROTARY_ENCODER_PIN_B, INPUT);
    pinMode(ROTARY_ENCODER_PIN_SW, INPUT);
}

void loop()
{
    // int pos = knob.read()/-2;
}