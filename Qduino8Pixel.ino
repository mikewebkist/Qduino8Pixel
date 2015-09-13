#include "Arduino.h"
#include "Wire.h"
#include "Qduino.h"
#include <Adafruit_NeoPixel.h>

// Where the NeoPixels are connected
#define NEOPIXEL_PIN 6
#define NEOPIXEL_LEDS 8
#define BUTTON_PIN 8

fuelGauge battery;  // initialize the library

qduino q;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEOPIXEL_LEDS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup()
{  
  Wire.begin(); 
  Serial.begin(9600);  // start Serial port
  pinMode(BUTTON_PIN, INPUT);

  battery.begin();
  q.begin();
  strip.begin();
  q.ledOff();

  delay(100);
  wipeIn();
  showChargeBinary();
  delay(1000);
  wipeOut();
  wipeIn();
  showChargeLevel();
  delay(1000);
  wipeOut();
}

void wipeIn() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, 32, 32, 32);
      strip.show();
      delay(25);
      strip.setPixelColor(i, 0, 0, 0);
      delay(25);
      strip.show();
  }
}

void showChargeLevel() {
  int charge = battery.chargePercentage();
  battery.reset();

  for(uint16_t i=0; i<strip.numPixels(); i++) {
      if(i < (strip.numPixels() * charge / 100)) {
	  strip.setPixelColor(i, 64, 0, 0);
      } else {
	  strip.setPixelColor(i, 0, 0, 0);
      }
  }
  strip.show();
}

void showChargeBinary() {
  int charge = battery.chargePercentage();
  battery.reset();

  for(uint16_t i=0; i<strip.numPixels(); i++) {
      if((charge >> i) & 1) {
	  strip.setPixelColor(i, 64, 0, 0);
      } else {
	  strip.setPixelColor(i, 0, 0, 0);
      }
  }
  strip.show();
}

void wipeOut() {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(strip.numPixels() - i - 1, 32, 32, 32);
      strip.show();
      delay(10);
      strip.setPixelColor(strip.numPixels() - i - 1, 0, 0, 0);
      delay(10);
      strip.show();
  }
}


long pressTime = 0;
int buttonStatePrev = LOW;

void loop()
{
    int buttonState = digitalRead(BUTTON_PIN);
    if(buttonStatePrev == LOW && buttonState == HIGH) {
	pressTime = millis();
	buttonStatePrev = buttonState;
    }

    for(int i=0; i<strip.numPixels(); i++) {
	strip.setPixelColor(i, 0, 0, 0);
    }

    if (buttonState == HIGH) {
	long leds = (millis() - pressTime) / 500 + 1;
	if(leds > strip.numPixels()) { leds = strip.numPixels(); }
	for(int i=0; i<leds; i++) {
	    strip.setPixelColor(i, 32, 32, 32);
	}
	if(((millis() - pressTime) / 500) > 10) {
	    wipeIn();
	    showChargeBinary();
	    delay(1000);
	    wipeOut();
	    wipeIn();
	    showChargeLevel();
	    delay(1000);
	    wipeOut();
	}
    } else {
	buttonStatePrev = buttonState;
    }
    strip.show();
    delay(100);
}
