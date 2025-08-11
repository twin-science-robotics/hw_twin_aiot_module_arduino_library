/* 
  Blink_RGB

  Turns ON the RGB LED on the Twin AIoT module for one second, then OFF for one second, repeatedly.

  modified 11 Aug 2025
  by TwinScience

*/


#include <FastLED.h> // include the RGB led library, install it from Library Manager if it is not installed

#define RGB_PIN 15   // The pin number the RGB led connected to 
#define NUM_LEDS 1   // Number of RGB led on the module

CRGB leds[NUM_LEDS]; // Create an RGB led object


// the setup function runs once when you press reset or power the board
void setup() {
  FastLED.addLeds<WS2812B,RGB_PIN, GRB>(leds, NUM_LEDS);  // initialize the RGB led 
  FastLED.setBrightness(100);                             // set its brightness between 0-255
}

// the loop function runs over and over again forever
void loop() {
  leds[0] = CRGB(0,255,0);      // set RGB values, green color as an example
  FastLED.show();               // show RGB values on the led

  delay(1000);                  // wait for a second

  leds[0] = CRGB(0,0,0);        // set RGB values all zero to turn OFF the led
  FastLED.show();               // show RGB values on the led

  delay(1000);                  // wait for a second
}
