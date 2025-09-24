#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>
//Service and characteristic UUIDs
#define SERVICE_UUID        "FFE0" // short format
#define CHARACTERISTIC_UUID "FFE1" 

#include <FastLED.h>

#define RGB_PIN 15  
#define NUM_LEDS 1

extern CRGB leds[NUM_LEDS];

// Firmware version 2.4.2
#define FIRMWARE_VERSION_MAJOR 2
#define FIRMWARE_VERSION_MINOR 4


#define SYNC_BYTE1 0xAA
#define SYNC_BYTE2 0x44
#define SYNC_BYTE3 0x1C

#define DIGITAL 1  
#define ANALOG 0

#define QUEUE_LENGTH 256

#define D6_output_pin 35
#define D9_output_pin 36
#define D10_output_pin 10

#define SERVO_CHANNEL_6  0
#define SERVO_CHANNEL_9  1  
#define SERVO_CHANNEL_10 2
#define TONE_CHANNEL 3
#define PWM_CHANNEL_1 4
#define PWM_CHANNEL_2 5
#define PWM_CHANNEL_3 6

#define D_IN_4 8
#define D_IN_8  11
#define D_IN_12  9

#define AN_IN_4 17
#define AN_IN_8  16
#define AN_IN_12  7

#define S1pin 37  
#define S2pin 38
#define S3pin 14 

#define TOUCHPAD_THRESHOLD 22000
#define TOUCH_PIN 13

#define PIANO_THRESHOLD 950

#define TRIG_PIN 5 // GPIO connected to HC-SR04 TRIG
#define ECHO_PIN 6 // GPIO connected to HC-SR04 ECHO

#define latchPin 34
#define clockPin 47
#define dataPin 48


const uint8_t data_length = 15;  // Adjust based on your command size
const uint8_t header_length = 5; // SYNC_BYTE1, SYNC_BYTE2, SYNC_BYTE3, msgID, msglength
const uint8_t packet_size = 20;  // header_length + data_length

// setting PWM properties
const int freq = 490;
const int resolution = 12;

const char outputpins[3] = {D6_output_pin, D9_output_pin, D10_output_pin};


// Message IDs
enum MessageID {
    DIGITAL_WRITE = 0x01,
    DIGITAL_READ = 0x02,
    ANALOG_WRITE = 0x03,
    ANALOG_READ = 0x04,
    PIANO_BUTTON = 0x05,
    SERVO = 0x0B,
    HCSR04 = 0x0C,
    RGB_LED = 0x0D,
    INIT_TWIN = 0x0E,
    GETFIRMWAREVERSION = 0x14,
    PLAYNOTE = 0x15,
    SINGLE_DC = 0x16,
    TEMPRATUREHUMIDITY = 0x18,
    DOUBLE_DC = 0x19,
    TOUCH_PAD = 0x1A,
    LED_MATRIX = 0x1B   
};


struct LEDMatrixLetter {
  char letter;
  uint8_t pattern[8];
};

const LEDMatrixLetter ledMatrixLetters[] = {
  { 'R', {0x7C, 0x42, 0x42, 0x7C, 0x48, 0x44, 0x42, 0x00} },
  { 'G', {0x3C, 0x42, 0x40, 0x40, 0x4E, 0x42, 0x3C, 0x00} },
  { 'B', {0x7C, 0x42, 0x42, 0x7C, 0x42, 0x42, 0x7C, 0x00} },
  { 'W', {0x42, 0x42, 0x42, 0x42, 0x5A, 0x66, 0x42, 0x00} },
  { 'O', {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00} },
  { 'P', {0x7C, 0x42, 0x42, 0x7C, 0x40, 0x40, 0x40, 0x00} },
  
  { 'T', {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00} },
  { 'w', {0x00, 0x00, 0x41, 0x41, 0x49, 0x55, 0x22, 0x00}}, //{ 'w', {0x00, 0x00, 0x63, 0x63, 0x6B, 0x6B, 0x36, 0x00} },
  { 'i', {0x18, 0x00, 0x38, 0x18, 0x18, 0x18, 0x3C, 0x00} },
  { 'n', {0x00, 0x00, 0x5C, 0x62, 0x42, 0x42, 0x42, 0x00}},//{ 'n', {0x00, 0x00, 0x5E, 0x63, 0x63, 0x63, 0x63, 0x00} },
  { ' ', {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} }
};

const size_t numLetters = sizeof(ledMatrixLetters) / sizeof(LEDMatrixLetter);


#endif // CONSTANTS_H