#include <FastLED.h>
#define RGB_PIN 15  
#define NUM_LEDS 1


CRGB leds[NUM_LEDS];


void setup() {
  // put your setup code here, to run once:
FastLED.addLeds<WS2812B,RGB_PIN , RGB>(leds, 1);
FastLED.setBrightness(255);
}

void loop() {
  // put your main code here, to run repeatedly:

  leds[0] = CRGB(255,0,0);
  FastLED.show();
  delay(1000);
  leds[0] = CRGB(0,0,0);
  FastLED.show();
  delay(1000);
}
