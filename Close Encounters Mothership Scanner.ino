// ---------------------------------------------------------------------------
// Close Encounters Mothership Scanner by Tony Fabris
//
// A lighting effect for WS2812 LED strips (aka "neopixels") using FastLED.
// This code is intended to run on an Arduino Mega, but it could be adapted
// for other processors.
//
// This is a demonstration ".ino" wrapper file. It is intended to run a simple
// main LED setup and then make a call to the actual animation loop. For the
// actual animation code, please view all the complicated code details in the
// included file:
//   "Close Encounters Mothership Scanner.h"
//
// This code runs on an Arduino Mega 2560, either full or small version:
//   https://www.amazon.com/dp/B0046AMGW0
//   https://www.amazon.com/dp/B07TGF9VMQ
//
// Special note: This code is intended for use with CRGBW LED strips. Note
// the "W" at the end of the "CRGBW". This means that the strips have red,
// green, and blue LEDS, but also include a white LED too. I am using "BTF"
// brand SK6812 RGBW strips (https://www.amazon.com/dp/B079ZW1265), but any
// brand will work. I'm deliberately taking advantage of the separation of RGB
// from W in my code, but it should be relatively simple to refactor this code
// to work with regular CRGB strips, if that's what you're using.
// ---------------------------------------------------------------------------

// Include LED header files.
#include <FastLED.h>         // https://fastled.io/
#include "FastLED_RGBW_2.h"  // https://www.partsnotincluded.com/fastled-rgbw-neopixels-sk6812/

// Global definitions for setting up the LED strips.
#define LED_TYPE                WS2812B
#define DATA_PIN                48
#define NUM_LEDS                40     // This code looks best on larger LED strands, 100-400 or more.
#define COLOR_ORDER             RGB
#define MAX_POWER_MILLIAMPS     500    // Set to appropriate power for your LED power supply.
#define SERIAL_PORT_SPEED       115200
#define BRIGHTNESS              255

// Prepare the arrays to hold the LED data. This is a special version of the
// definitions, which is needed for the special CRGBW strips with the extra
// white LED. See this web site for more details about coding for CRGBW:
// https://www.partsnotincluded.com/fastled-rgbw-neopixels-sk6812/
CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];

// Main code for the animations, file is in the same folder as this one.
#include "Close Encounters Mothership Scanner.h"

// Arduino setup routine, runs once when the Arduino powers up.
void setup()
{
  // Initialize serial port for debugging sessions.
  Serial.begin(SERIAL_PORT_SPEED);

  // Initialize FastLED, modified for use with RGBW light strips - Details at
  // https://www.partsnotincluded.com/fastled-rgbw-neopixels-sk6812/
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(ledsRGB, getRGBWsize(NUM_LEDS));
  FastLED.setMaxPowerInVoltsAndMilliamps( 5, MAX_POWER_MILLIAMPS);
  FastLED.setBrightness(BRIGHTNESS);
}

// Arduino main loop, runs continuously after the setup routine is done.
void loop()
{
  // Perform one runthrough (one frame of animation) of the main effect loop
  // located in the included file "Close Encounters Mothership Scanner.h"
  ce3kScanner();

  // Paint the final LED array values onto the LED strip for this frame.
  FastLED.show();
}

