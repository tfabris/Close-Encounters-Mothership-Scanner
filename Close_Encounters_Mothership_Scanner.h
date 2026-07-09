#ifndef Close_Encounters_Mothership_Scanner_h
#define Close_Encounters_Mothership_Scanner_h

// ---------------------------------------------------------------------------
// Close Encounters Mothership Scanner by Tony Fabris
//
// https://github.com/tfabris/Close-Encounters-Mothership-Scanner
//
// A lighting effect for WS2812 LED strips (aka "neopixels") using FastLED.
// This code is intended to run on an Arduino Mega, but it could be adapted
// for other processors.
//
// This is an attempt to simulate the "Scanner ring" effect from the mothership
// from the Steven Spielberg film "Close Encounters of the Third Kind". When
// the mothership extends its airlock hatch, it emerges from a circular opening
// which has a ring of white fiber-optic lights which are moving in an
// interesting pattern. This code attempts to translate that physical
// long-exposure effect into addressable LED strips.
//
// My goal is to perform an accurate simulation of the effect, rather than just
// an imitation. Full details of how the original effect was done:
//
//                    https://bit.ly/CE3K-Scanner
//
// Special note: This code is intended for use with CRGBW LED strips. Note
// the "W" at the end of the "CRGBW". This means that the strips have red,
// green, and blue LEDS, but also include a white LED too. I am using "BTF"
// brand SK6812 RGBW strips (https://www.amazon.com/dp/B079ZW1265), but any
// brand will work. I'm deliberately taking advantage of the separation of RGB
// from W in my code, but it should be relatively simple to refactor this code
// to work with regular CRGB strips, if that's what you're using.
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Set up variables
// ---------------------------------------------------------------------------

// These variables control the brightness value of the white bars in the idle
// portion of the scanner effect, and the V of the HSV values of the flashing
// color conversation lights which appear atop the scanner. To mute either of
// the animations, for example, muting the color flashes so that you see only
// the white scanner bars, or muting the scanner bars to that you only see the
// color flashes, then set the corresponding value to 0. The scanner brightness
// can be set to a number between 0 and 255. Recommend adjusting the scanner
// brightness to a medium number to keep the color flash animations from being
// overwhelmed by the brilliance of the white bars. Due to the way the HSV
// calculations are coded, the conversation brightness value will not change
// the LED brightness much, so set conversation brightness to just 0 or 255.
#define SCANNER_BRIGHTNESS             150
#define CONVERSATION_BRIGHTNESS        255

// Number of milliseconds between "frames" of the scanner light animation. For
// reference, a 60fps video game displays at about 16ms per frame.
#define SCANNER_ANIMATION_SPEED        15

// Minimum and maximum random start positions for the color conversation lights
// along the LED strand. Use these values if you want the color flashes to be
// constrained to a certain subsection of your LED strand. In my case, my
// strand lights up some shelving units, and I might want to constrain the
// color flashes to a single shelf unit (in my case the third shelf runs from
// 206 to 313). You can make these values from 0 to 32767. If a given color bar
// would exceed NUM_LEDS, the values will be reined in, so that it doesn't write
// past the end of the strand array.
#define CONVERSATION_START_POINT_MIN   206
#define CONVERSATION_START_POINT_MAX   313

// Parameters which control the speed of the occasional flashing color
// conversation lights overlaid atop the white scanner bars. Some of these seem
// illogical at first glance. For example, why would I limit each frame's speed
// to a minimum number of milliseconds, but then, skip frames to speed it up
// again? Because I want this animation to work on different-length LED
// strands, and the entire routine takes different amounts of milliseconds to
// run depending on the strand length. These variables allow you to adjust for
// different strand lengths and processor speeds.
#define CONVERSATION_FLASH_MIN_FRAMES  5   // Minimum random width of color bar for each color flash animation (pixels). 
#define CONVERSATION_FLASH_MAX_FRAMES  25  // Max is actually this plus the Minumum, for the total maximum.
#define CONVERSATION_EXTRA_DWELL_MAX   40  // The color flashes dwell a random amount of frames at full extension during held notes (max random frames).
#define CONVERSATION_FLASH_SPEED       13  // Minimum number of milliseconds before a new color flash animation frame can be played. If this is lower than the number of milliseconds the entire routine takes to run, it makes no difference.
#define CONVERSATION_FLASH_FRAMESKIP   3   // If the color flash animation swells too slowly, skip more frames to make it faster.
#define CONVERSATION_FLASH_FREQUENCY   900 // Each blank frame, a random number of 0-1000 must exceed this number to start a new color flash (higher is less likely).

// Values I'm using for some speed optimizations to avoid expensive
// floating-point and division operations during the main loop. 
const uint8_t MAX_SUBPIXELS = 40;       // Highest number of subpixels that we will scroll any of the patterns.
uint8_t div256Table[MAX_SUBPIXELS];     // Speed-optimization lookup table for weighting of each sub-scroll of the pattern.
uint16_t slitCopyFullRepeatsCount = 0;  // How many full, un-truncated slit pattern strips fit inside the total LED strip.
uint16_t slitCopyRemainingLeds = 0;     // Number of leftover pixels need to be copied for the last remainder to fill the LED strip.

// ---------------------------------------------------------------------------
// Pixel array definitions
// ---------------------------------------------------------------------------
// The pixel arrays below are intended to simulate Robert Swarthe's cutout
// patterns. As this program gets improved, expect these pixel arrays to get
// updated with improved versions. In the CE3K film, there are several
// different sets of lighting patterns used. The long term goal is to
// eventually simulate all the patterns seen in the film.
//
// Up to two pixel arrays can be used at a time. If two arrays are used, then
// they will be displayed as if they were overlaid atop each other on the light
// box. If using two arrays, they can be different heights, but they must be
// the same width. If only one array is needed, then use arrayBlank for the
// second array.
// 
// For best results, design each array so that it wraps around smoothly at the
// top/bottom and sides. The code allows arrays of different heights in order
// to help with being able to design wrap-around images.
//
// If new arrays have are added here, remember to also update the place at the
// beginning of the ce3kScanner() function which defines the data structure of
// each pattern, and also update the NUM_CE3K_PATTERNS and WIDEST_ARRAY
// values.
// ---------------------------------------------------------------------------

// Work in progress: I'm working on GitHub issue #8 - trying to allow for larger
// patterns with proper antialiasing. So far: I have converted the arrays to
// use PROGMEM instead of standard memory, and also they have been changed from
// BOOL arrays to CHAR arrays. Refs:
// https://www.e-tinkers.com/2020/05/do-you-know-arduino-progmem-demystified/
// https://www.arduino.cc/reference/en/language/variables/utilities/progmem/
//
// Before, with arrays in standard memory:
// Sketch uses 33010 bytes (12%) of program storage space. Maximum is 253952 bytes.
// Global variables use 5495 bytes (67%) of dynamic memory, leaving 2697 bytes for local variables. Maximum is 8192 bytes.
//
// After changing arrays to PROGMEM:
// Sketch uses 32924 bytes (12%) of program storage space. Maximum is 253952 bytes.
// Global variables use 1915 bytes (23%) of dynamic memory, leaving 6277 bytes for local variables. Maximum is 8192 bytes.

// Define the special empty array "arrayBlank". The purpose of this array is so
// that if, in a given pattern, you are using only one array instead of both
// arrays, then supply this as the second array.
const char PROGMEM arrayBlank[] = {};
int  arrayBlankSize = sizeof(arrayBlank);
int  arrayBlankWidth = 0;

// Arrays Tony01 and Tony02, when combined together, create a particularly nice
// and interesting pattern. Reminiscent of the film, but not the same as any of
// the ones seen in the film.
const char PROGMEM arrayTony01[] = 
{
  0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,
  1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,
  1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,
  1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,
  1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,
  1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};
int arrayTony01Size = sizeof(arrayTony01);
int arrayTony01Width = 44;

const char PROGMEM arrayTony02[] = 
{
  1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,
  0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
  0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
  0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
  0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
  0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,
};
int arrayTony02Size = sizeof(arrayTony02);
int arrayTony02Width = 44;

// Attempt to reproduce the counter-rotating pairs of lights, used for most of
// the conversation scene, which seem to merge and split. This is a single
// array, not intended to be used with a second overlaid array.
const char PROGMEM arrayConversationPairs[] = 
{
  0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,
  0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,
  0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,
  0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,
  0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,
  0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,
  0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,
  1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
  1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,
  1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,
  1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,
  1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
  0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,
  0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,
  0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,
  0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,
  0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,
  0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,
  0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,
};
int arrayConversationPairsSize = sizeof(arrayConversationPairs);
int arrayConversationPairsWidth = 32;

// Another set of two overlaid patterns of my own (not in the film).
const char PROGMEM arrayTony03[] = 
{
  0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,
  0,0,0,0,1,1,1,1,1,1,0,0,1,1,1,1,1,1,0,0,
  1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
  1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
  1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,
  1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,
  1,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,
  1,1,1,1,1,1,0,0,0,0,1,1,0,0,0,0,1,1,1,1,
  0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,
  0,0,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,
};
int arrayTony03Size = sizeof(arrayTony03);
int arrayTony03Width = 20;

const char PROGMEM arrayTony04[] = 
{
  0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,
  0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,
  0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,
  1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,
  1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,
  1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,
  1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,
  1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,
  1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,
  0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,
  0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,
  0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,
  1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,
  1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,
  1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,
  1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,
  1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,
  1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,
  0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,
  0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,
  0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,
  0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,
};
int arrayTony04Size = sizeof(arrayTony04);
int arrayTony04Width = 20;

// Define the widest array width that is expected to be used in the code. It
// should be the widest of the arrays defined above.
#define WIDEST_ARRAY 44

// This is an array of values that represents the "slit" view of the zigzag
// patterns. Note: This code was written for CRGBW strip hardware; if you are
// using CRGB hardware, you'll need to refactor some parts of this code.
CRGBW zigzagSlit[WIDEST_ARRAY];

// Define a data structure called "CE3Kpattern", which holds the collected
// information for one of the currently-running patterns. Note that a running
// pattern can be a combination of up to two of the image arrays defined
// above.
// 
// A note about the SubpixelResolution parameter: This code will scroll the view
// of a "slit" down the zigzag pattern images. It will do so in units which are
// smaller than one line, in order to get sub-pixel resolution of the movement
// of the image. This variable controls the number of "steps" which will occur
// between each line of the image. The finer the subpixel resolution (the larger
// this variable is), then the greater number of steps, so the slower that the
// simulation will scroll that pattern. In this way, SubpixelResolution is what
// controls the relative "speed" of the white bars during each pattern. Note
// that this code is only doing subpixel resolution in the vertical dimension,
// not horizontal, so all antialiasing occurs vertically line-to-line, not
// sideways pixel-to-pixel.
typedef struct
{
  char* ArrayA;             // Can use up to two overlapping arrays if desired.
  char* ArrayB;             // If using only one array, set this to arrayBlank.
  int   SizeA;  
  int   SizeB;    
  int   Width;              // If using two arrays, they must both be the same width. 
  int   SubpixelResolution; // Indirectly controls the speed of the white bars.
} CE3Kpattern;

// Variable which indicates how many total runnable patterns are going to be
// used in this program. Note: here, we are talking about the sets of data
// structures which contain the runnable patterns, not the pixel arrays defined
// above. If changing the number of patterns in the program, first update the
// definitions at the beginning of the ce3kScanner() function which update the
// pattern data structures, then update this number to match.
#define NUM_CE3K_PATTERNS 3

// Define an array to hold all of the data structures of all of the runnable
// patterns. At the start of the main loop, values will be assigned to the data
// structures in this array. Then we will be able to cycle through all of the
// patterns.
CE3Kpattern CE3Kpatterns[NUM_CE3K_PATTERNS];

// Number of milliseconds between pattern changes.
#define CE3K_PATTERN_CHANGE_INTERVAL 15000


// ---------------------------------------------------------------------------
// Function to grab one pixel out of the zigzag pixel array(s), and turn it
// into a brightness value that can be applied to the LEDs.
// ---------------------------------------------------------------------------
int pixelValue(long arrayPosition, char firstArray[], int firstArraySize, char secondArray[], int secondArraySize)
{
  // Work in progress: I'm working on GitHub issue #8 - trying to allow for
  // larger patterns with proper antialiasing. Originally the pixel arrays were
  // coded as bool, either 0 or 1, to make it easier to type patterns into the
  // arrays. This is in-process of being changed, so that each pixel in the
  // pattern could be an 8 bit level which can be antialiased.
  // 
  // Current state: If you are using two arrays, this code ANDs the two arrays
  // together so that any black stripes are preserved. This simulates the
  // original fiber optic effect, where the photosensor controlling the fiber
  // optic light is blocked by the pattern(s).  
  static long lastPosition = -1;
  static int index1 = 0;
  static int index2 = 0;

  // Speed optimization: Handle the first run or frame reset (Position 0).
  // Previously it did an expensive modulo calculation ("%") every time this
  // function was called. Now it only does the modulo on the first pixel of a
  // frame, improving the overall speed. 
  //
  // Each array will be smaller than the arrayPosition variable, but by using
  // the modulo operator (%) we can dig into the arrays at the correct
  // position points without needing complicated math. Basically, the
  // arrayPosition cycles continuously through a very large range of numbers,
  // and the "%" operator is the math that finds out what the array position
  // would have been if it had started within this small array. This allows
  // the code to support zigzag arrays of any size.
  //
  // The speed optimization is to only calculate the modulo at certain points
  // where it's needed, and use normal integer addition and subtraction
  // most of the rest of the time.
  long difference = arrayPosition - lastPosition;
  if (lastPosition == -1 || arrayPosition == 0 || difference == arrayPosition) 
  {
    // Also cast the modulo operations to unsigned ints to speed those up.     
    index1 = (unsigned long)arrayPosition % (unsigned int)firstArraySize;
    if (secondArraySize > 0)
    {
      index2 = (unsigned long)arrayPosition % (unsigned int)secondArraySize;
    }
  }
  else 
  {
    // For all subsequent pixels in the row, use addition/subtraction instead of
    // expensive modulo operations.

    // Update tracking for the first array.
    index1 += (int)difference;
    while (index1 >= firstArraySize) index1 -= firstArraySize;
    while (index1 < 0)               index1 += firstArraySize;
    
    // Update tracking for the second array (if a second array is used).
    if (secondArraySize > 0)
    {
      index2 += (int)difference;
      while (index2 >= secondArraySize) index2 -= secondArraySize;
      while (index2 < 0)               index2 += secondArraySize;
    }
  }

  // Save the master position for the next function call.
  lastPosition = arrayPosition;

  // If only one array is used, then the second array will be empty and will
  // have a size of zero. Detect this and use only the first array.
  bool blackOrWhitePixel;
  if (secondArraySize > 0)
  {
    // This is where the code ANDs the two arrays together so that any black
    // stripes are preserved, simulating the original fiber optic effect, where
    // the photosensor controlling the fiber optic light is blocked by black.
    blackOrWhitePixel = pgm_read_byte(&firstArray[index1]) & pgm_read_byte(&secondArray[index2]);
  }
  else
  {
    // If only one array is being used, then just return that byte in the array.
    blackOrWhitePixel = pgm_read_byte(&firstArray[index1]);
  }

  // Work in progress: I'm working on GitHub issue #8 - trying to allow for
  // larger patterns with proper antialiasing. At the moment this is still
  // converting  the binary 0 or 1 value into a pixel darkness value, i.e., 0
  // becomes 0, 1 becomes full brightness. I will be working on making shades
  // of gray possible later.
  // 
  // Note: I originally had this line:
  //      return blackOrWhitePixel * SCANNER_BRIGHTNESS;
  //
  // Which was intended to work if True was 1 and False was 0. But I was finding
  // that True was not casting to 1, it was casting to a larger number. Instead
  // do a test on blackOrWhitePixel and then multiply either the 1 or the 0
  // times the scanner brightness value. This is going to be rewritten when the
  // arrays become shades of gray.
  return (blackOrWhitePixel ? 1 : 0) * SCANNER_BRIGHTNESS;
}


// ---------------------------------------------------------------------------
// Subroutine to add the colored flashing "conversation" lights, atop the moving
// white "idle" animation bars. The original colored lights in the film were
// hand-animated by Robert Swarthe, this attempts to simulate their style.
// ---------------------------------------------------------------------------
void CE3Kconversation()
{
  // Each color flash has to go through several frames of animation for the
  // entire flash. Keep track of both things with the variable "flashStage":
  // flashStage 0 means no flash is occurring, flashStage nonzero means that we
  // are in the middle of a color flash animation. Each time a frame of the
  // animation is played, it increments or decrements the flashStage variable
  // (incrementing when the color bar is swelling up to its full length,
  // decrementing when it's unswelling down to nothing). A separate variable,
  // flashFrames, keeps track of the number of swelling frames in this
  // animation (each flash will have a different size and thus a different
  // number of frames). Then if flashStage reaches flashFrames, that means it's
  // the last frame of the swell-up animation and it then dwells for a bit,
  // then switches direction and decrements down to zero. Once it hits zero,
  // then the animation is done and it waits until the next color flash is
  // triggered.
  //
  // TO DO: In the film, there are some moments in the scene where multiple
  // color flashes occur at the same time. This code only does one flash at a
  // time. Investigate refactoring the code to keep track of multiple flashes.
  static int  flashStage      = 0;
  static int  flashFrames     = 0;
  static int  flashDwell      = 0;
  static bool flashIncreasing = false;

  // Initialize the variables which will be used to control the width, position,
  // and color of the color flash bar. These remain the same across multiple
  // loops, so they are declared as static, and they only get changed when a
  // new flash is triggered.
  static int  colorBarWidth      = 0;
  static int  colorBarTempWidth  = 0;
  static int  halfWayMark        = 0;
  static int  colorBarStartPoint = 0;
  static int  colorBarHue        = 0;
  static CRGB colorBarColor      = (0,0,0);
  uint8_t     onePixelBrightness = 0;

  // Each frame of the color flash animation happens at this speed.
  EVERY_N_MILLISECONDS ( CONVERSATION_FLASH_SPEED )
  {
    // If we're currently not in the middle of a flash animation, decide
    // whether this frame will begin a new flash animation.
    if (flashStage < 1)
    {
      // Flash animation decision is random, governed by this threshold.
      // Generate a random number between 0 and 1000, and if the number is
      // larger than the threshold, trigger a new color flash animation.
      if (random16(1000) > CONVERSATION_FLASH_FREQUENCY)
      {
        // Begin the color flash animation.
        flashStage         = 1;   
        flashIncreasing    = true;

        // Randomize the size and position of the color bar flash. 
        colorBarWidth      = random16(CONVERSATION_FLASH_MAX_FRAMES) + CONVERSATION_FLASH_MIN_FRAMES;
        flashDwell         = random16(CONVERSATION_EXTRA_DWELL_MAX); // Color bar can dwell at its widest point for a certain number of frames.
        colorBarStartPoint = random16(CONVERSATION_START_POINT_MAX - CONVERSATION_START_POINT_MIN - colorBarWidth) + CONVERSATION_START_POINT_MIN; 
        colorBarHue        = random8();                              // Random hue for each color light flash.

        // Ensure we do not overwrite memory by making sure the start/end points
        // don't exceed the start or end of the LED strand. This should only be
        // needed when running on a small test strip where the width of the
        // color bars could become greater than the length of the strand.
        if (colorBarStartPoint < 0) { colorBarStartPoint = 0; }
        if (colorBarStartPoint >= NUM_LEDS) { colorBarStartPoint = random16(NUM_LEDS - colorBarWidth); }
        if (colorBarStartPoint + colorBarWidth >= NUM_LEDS) { colorBarWidth = NUM_LEDS - colorBarStartPoint - 1; }

        // Half way mark that defines the center of the color flash.
        //   halfWayMark = colorBarWidth / 2;
        // Speed optimization: Do a right bitwise shift by one position, which is
        // a faster way to do a divide-by-2 with integer math on this chipset.
        halfWayMark = colorBarWidth >> 1; 

        // Number of animation frames is the same as the color bar width.
        flashFrames = colorBarWidth;
 
        // Create an HSV value based on the random hue, then convert HSV to RGB so
        // that I can paint *just* the RGB values into the RGBW strip but *not*
        // the W, which is responsible for the idle scanner effect.
        CHSV colorBarhsv(colorBarHue, 255, CONVERSATION_BRIGHTNESS);
        hsv2rgb_rainbow( colorBarhsv, colorBarColor);  
      }
    }

    // Increment or decrement the animation frame, depending on the current
    // stage of the flash animation we're in right now. This must be done
    // within the "EVERY_N_MILLISECONDS" section so that the timing of the
    // animation is preserved.
    if (flashStage > 0)
    {
      // Increment or decrement the color flash animation frame we're in, which
      // will control whether the color bar is swelling or unswelling.
      if (flashIncreasing)
      {
        // The color bar swells to full width twice as quickly as it unswells.
        // Since each color flash represents a musical note, this is like
        // giving the note a sharper attack than the note's decay. This seems
        // to look best. I asked Robert Swarthe if that's the way he did it,
        // and he doesn't remember, but said that it sounds like something he'd
        // probably do.
        flashStage += CONVERSATION_FLASH_FRAMESKIP;
        flashStage += CONVERSATION_FLASH_FRAMESKIP;
      }
      else
      {
        flashStage -= CONVERSATION_FLASH_FRAMESKIP;
      }

      // The distance from the center of the color flash that the pixels extend.
      // This controls the swell size of the color bar. Basically the color bar
      // is divided down the center, and it swells from that center point
      // leftward towards the 0 mark of the color bar, and from the center
      // point rightward toward the max mark of the color bar. This value
      // controls how far from the center point it swells on this frame of the
      // animation.
      //   colorBarTempWidth = flashStage / 2;
      // Speed optimization: Do a right bitwise shift by one position, which is
      // a faster way to do a divide-by-2 with integer math on this chipset.
      colorBarTempWidth = flashStage >> 1; 
 
      // If we have reached widest point of the color flash animation, either
      // dwell at the widest point for a bit (like the sustain portion of a
      // musical note), or switch direction so that it decreases the bar's
      // width (like the decay portion of a musical note).
      if (flashStage > flashFrames)
      {
        if (flashDwell > 0)
        {
          // During the dwell period, flashStage will keep getting
          // double-incremented in an earlier step. By decrementing it here
          // during the dwell, we keep it bumping up against the "wall" of the
          // fully-extended bar width.
          flashStage -= CONVERSATION_FLASH_FRAMESKIP; 
          flashStage -= CONVERSATION_FLASH_FRAMESKIP; 
          flashDwell -= CONVERSATION_FLASH_FRAMESKIP; // Count down the dwell towards zero.
        }
        else
        {
          // If there's no dwell, or if we're done with the dwell, then start
          // decreasing the bar width instead of increasing it.
          flashIncreasing = false;  
        }
      }
    }
  }

  // Paint the current color flash bar onto the LED strand. Note: Must do this
  // outside the "EVERY_N_MILLISECONDS" section so that the color flash pixels
  // are applied to the strand on every loop. This prevents the color bars from
  // rapidly flickering each loop.
  if (flashStage > 0)
  {
    // Cycle through all pixels of the color bar from left to right.
    for (int c = colorBarStartPoint; c <= colorBarStartPoint + colorBarWidth; c++)
    {
      // Determine if each color bar pixel should be lit or dark, based on our
      // position through the loop. Note that some of these "if" statements
      // make a color decision about one of the pixels, and then a later
      // statement makes a different decision about the same pixel. By design.
      if (c > colorBarStartPoint + halfWayMark - colorBarTempWidth)   
      {
        // Left half of the color bar, lit pixels.
        onePixelBrightness = 255;
      }
      else
      {
        // Left half of the color bar, dark pixels. 
        onePixelBrightness = 0;
      }
      if (c > colorBarStartPoint + halfWayMark  )
      {
        // Right half of the color bar, lit pixels.
        onePixelBrightness = 255;
      }
      if (c > colorBarStartPoint + halfWayMark + colorBarTempWidth  ) 
      {
        // Right half of the color bar, dark pixels. 
        onePixelBrightness = 0;
      }  

      // Apply the color flash to the LED strand. Modify only the RGB values and
      // do not touch the W value in the CRGBW strand. The W value is already
      // being used for scrolling the black and white "idle scanner" animation
      // running in parallel to these color flashes.
      //
      // If you're using CRGB LEDs instead of CRGBW LEDs, and refactoring this
      // code to use them, then here is where you'll have to blend the
      // conversation colors with the white idle scanner pixels that are
      // already there.
      leds[c].red = colorBarColor.red * onePixelBrightness; 
      leds[c].green = colorBarColor.green * onePixelBrightness;
      leds[c].blue = colorBarColor.blue * onePixelBrightness;
    }
  }
}

// ---------------------------------------------------------------------------
// Create/update a lookup table of the current pattern subpixel resolution and
// other variables, so that every time through the animation loop, it doesn't
// run a costly floating point division operation to get the antialiasing
// blend-weights and other values. Only do these costly division operations
// once, each time a new pattern is activated. Note: Hard coding the first
// entry in the table(index[0]) as a 0, instead of trying to calculate that
// first entry, in order to prevent divide by zero errors in this function.
// ---------------------------------------------------------------------------
void updateDivTable(int currentPatternIndex)
{
  int subpixelResolution = CE3Kpatterns[currentPatternIndex].SubpixelResolution;
  div256Table[0] = 0; 
  for (uint8_t tableIndex = 1; tableIndex < subpixelResolution; tableIndex++)
  {
    div256Table[tableIndex] = ((float)1 / subpixelResolution) * tableIndex * 256;
  }

  // Precalculate additional division variables. These variables control the
  // operation which copies each "slit" view of the patterns onto the full size
  // of the entire LED strip. 
  uint16_t patternWidth =  CE3Kpatterns[currentPatternIndex].Width;
  slitCopyFullRepeatsCount = NUM_LEDS / patternWidth;   // How many full, un-truncated blocks fit inside the strip.
  slitCopyRemainingLeds = NUM_LEDS % patternWidth;      // Number of leftover pixels need to be copied for the last remainder.
}

// ---------------------------------------------------------------------------
// Main loop of the CE3K scanner effect. This routine should be called once
// per "loop()" of the main Arduino code. This routine is responsible for
// generating the moving white "idle" bars of the scanner ring animation.
// ---------------------------------------------------------------------------
void ce3kScanner()
{
  static long imageOffset = 0;        // Which line of the zigzag arrays are we on?
  static int subPixelOffset = 0;      // Move through the arrays slowly while antialiasing.
  static CE3Kpattern currentPattern;  // Which pattern is currently running.
  static int currentPatternIndex;     // Which index in the array of pattern data structures is the curernt pattern.
  static bool firstTime = true;       // Keep track of code which only needs to be run the first time through the loop.
  
  // Only animate the scanner lights at a certain frame rate.
  EVERY_N_MILLISECONDS ( SCANNER_ANIMATION_SPEED )
  {
    // Perform actions on the first run of this function only.
    if (firstTime)
    {
      firstTime = false;

      // Initialize the slit view array to black.
      fill_solid( zigzagSlit, WIDEST_ARRAY, CRGBW(0,0,0,0) );

      // Initialize the data in all of the pattern data structures. Make sure to
      // update the variable definition NUM_CE3K_PATTERNS at the top of the code
      // too. Since this is something that I might tend to forget myself, I'm
      // using a "trick" here with the checkPatternIndex variable, to force
      // myself to ensure that NUM_CE3K_PATTERNS equals the number of definitions
      // here.
      int checkPatternIndex = -1;

      checkPatternIndex++;
      CE3Kpatterns[checkPatternIndex].ArrayA = arrayTony01;
      CE3Kpatterns[checkPatternIndex].ArrayB = arrayTony02;
      CE3Kpatterns[checkPatternIndex].SizeA  = arrayTony01Size;  
      CE3Kpatterns[checkPatternIndex].SizeB  = arrayTony02Size;
      CE3Kpatterns[checkPatternIndex].Width  = arrayTony01Width;
      CE3Kpatterns[checkPatternIndex].SubpixelResolution = 5;

      checkPatternIndex++;
      CE3Kpatterns[checkPatternIndex].ArrayA = arrayTony03;
      CE3Kpatterns[checkPatternIndex].ArrayB = arrayTony04;
      CE3Kpatterns[checkPatternIndex].SizeA  = arrayTony03Size;  
      CE3Kpatterns[checkPatternIndex].SizeB  = arrayTony04Size;
      CE3Kpatterns[checkPatternIndex].Width  = arrayTony03Width;
      CE3Kpatterns[checkPatternIndex].SubpixelResolution = 15;   // Do not go above MAX_SUBPIXELS

      checkPatternIndex++;
      CE3Kpatterns[checkPatternIndex].ArrayA = arrayConversationPairs;
      CE3Kpatterns[checkPatternIndex].ArrayB = arrayBlank;
      CE3Kpatterns[checkPatternIndex].SizeA  = arrayConversationPairsSize;  
      CE3Kpatterns[checkPatternIndex].SizeB  = arrayBlankSize;
      CE3Kpatterns[checkPatternIndex].Width  = arrayConversationPairsWidth;   
      CE3Kpatterns[checkPatternIndex].SubpixelResolution = 5;

      // Done with defining patterns. Make sure that we defined them correctly.
      checkPatternIndex++; // Because of zero-indexing, the count is one higher than the index.
      if (checkPatternIndex != NUM_CE3K_PATTERNS)
      {
        while(true)
        {
          // Yell at myself if I don't update the code correctly.
          Serial.println( F("-------------------------------------------------------------------------"));
          Serial.println( F("Error - CE3K Scanner: NUM_CE3K_PATTERNS differs from pattern definitions."));
          Serial.println( F("-------------------------------------------------------------------------"));
          delay(5000);
        }
      }

      // Decide which of the patterns we'll be starting on, and prep it.
      currentPatternIndex = 0;
      currentPattern = CE3Kpatterns[currentPatternIndex];
      updateDivTable(currentPatternIndex);
    }

    // Cycle to the next scanner pattern at intervals.
    EVERY_N_MILLISECONDS(CE3K_PATTERN_CHANGE_INTERVAL)
    {
      if (colorCyclingIsOn)   // This variable globally toggles animations on and off.
      {
        imageOffset = 0;      // Must reset these variables when changing patterns
        subPixelOffset = 0;   // in order to prevent positioning and indexing bugs.
        currentPatternIndex ++;
        if (currentPatternIndex >= NUM_CE3K_PATTERNS) { currentPatternIndex = 0; }
        currentPattern = CE3Kpatterns[currentPatternIndex];    
        updateDivTable(currentPatternIndex);
      }
    }

    // Sanity check that I remembered to set WIDEST_ARRAY correctly.
    if (currentPattern.Width > WIDEST_ARRAY)
    {
      while(true)
      {
        // Yell at myself if I didn't update WIDEST_ARRAY correctly.
        Serial.println( F("-------------------------------------------------------------------------"));
        Serial.println( F("Error - CE3K Scanner: currentPattern width is wider than WIDEST_ARRAY."));
        Serial.println( F("-------------------------------------------------------------------------"));
        delay(5000);
      }
    }  

    // The "weight unit" is a percentage number representing the "thickness" of 1
    // unit of subpixel resolution. For example if the subpixel resolution is set
    // to 5, then each one of these units would be 0.20. The "weight" is this
    // times the subpixel offset, so that the "weight" of each anti-aliased step
    // is a smooth linear brightness transition between each line: 0.00, 0.20,
    // 0.40, 0.60, 0.80. Note that 1.00 is skipped in the loops because that ends
    // up being the same as the 0.00 for the following iteration. If it actually
    // did both 0.00 and 1.00, it would work, but it would cause a short "pause"
    // in the animation where the two similar frames met. The blend is based on:
    // http://www.designimage.co.uk/quick-tip-the-maths-to-blend-between-two-values/
    //    float oneSubPixelWeightUnit = ((float)1 / (float)currentPattern.SubpixelResolution);
    //    float blendWeight = subPixelOffset * oneSubPixelWeightUnit;

    // Speed optimization: Changing the blending code to a non-floating-point and
    // non-division version of the blending weight code. Intended to make the
    // loops run slightly faster by using pure integer math in the main loop.
    // Instead of the weighting being done by a floating point percentage, it's
    // now an integer value that is represented by a number from 0-255. For
    // instance, if the subpixel resolution is at 5 for this particular pattern,
    // then the first level of the five subpixel weighting values will be 51
    // instead of 0.20. These values are stored in a lookup table which is
    // recalculated once at the beginning of each pattern display. This way, the
    // division operation is replaced by this quick div256Table lookup, saving
    // dozens of CPU cycles per loop.
    uint8_t blendWeight = div256Table[subPixelOffset];

    // TO DO: The blend above (regardless of whether it's done with the floating
    // point or the lookup table) is a purely linear blend. Unfortunately the
    // LEDs do not have a purely linear brightness based on the numbers pumped
    // into them. The lowest brightness level of an LED is significantly brighter
    // than when the LED is just "off". This makes the antialiased pixels kind
    // of "pop" from fully dark to dimly-lit in the final animation. This causes
    // the transition points between black background and the white bars to seem
    // to "caterpillar" across the strand instead of flowing perfectly smoothly.
    // It would be nice if I could come up with a nonlinear blend to make it seem
    // more smooth.

    // Assemble the current slit view into the slit array. This loop is using
    // a speed optimization where decrementing the uint16_t is faster than
    // incrementing and testing an int with a For loop.
    uint16_t x = currentPattern.Width;
    if (currentPattern.Width <= 0) return;
    while (x--)
    {
      // Obtain the current pixel location, and also the pixels on the prior row
      // and the following row (for antialiasing).
      long thisPixelPosition = x+imageOffset;
      long nextPixelPosition = thisPixelPosition+currentPattern.Width;

      // Get the value of the pixels of the image arrays. 
      int thisPixelDarkness = pixelValue(thisPixelPosition, currentPattern.ArrayA, currentPattern.SizeA, currentPattern.ArrayB, currentPattern.SizeB);
      int nextPixelDarkness = pixelValue(nextPixelPosition, currentPattern.ArrayA, currentPattern.SizeA, currentPattern.ArrayB, currentPattern.SizeB);

      // Blend the next and previous line's pixels into the current line's pixel.
      // I tried using FastLED's "blend8" function, but it did not produce the
      // results I wanted. This is my own blend math, based on this:
      // http://www.designimage.co.uk/quick-tip-the-maths-to-blend-between-two-values/
      //    int blendedDarkness = (nextPixelDarkness*blendWeight)+(thisPixelDarkness*(1-blendWeight));

      // Speed optimization: Integer-math version of the floating point blend
      // above. Multiply by the blend weight as described above, but multiply it
      // into a 16-bit integer and then bitshift it back down to 8 bits.
      uint8_t blendedDarkness = (((uint16_t)nextPixelDarkness*blendWeight) >> 8)+(((uint16_t)thisPixelDarkness*(256-blendWeight)) >> 8);

      // Apply the final values to the array that represents the slit. I'm using
      // only the White LED in the CRGBW array here, so the colored conversation
      // lights can be painted separately without having to blend them with the
      // white LEDs. If you are using CRGB LEDs, you'll have to refactor this.
      zigzagSlit[x].white = blendedDarkness;
    }

    // Copy the slit array onto the entire LED strand. If the current width is
    // less than the total number of LEDs, then it will copy it multiple times.
    // If the current width is larger than the total number of LEDs, it will
    // copy only the relevant subsection. More details and example found here:
    // https://github.com/marmilicious/FastLED_examples/blob/master/memmove8_pattern_copy.ino

    // This code has had some speed optimizations made. Originally it was an
    // incrementing for/next loop, and in the middle of the loop there was
    // an "if" test every time to determine if the copied data would exceed the
    // end of the strand. Instead, here it has been changed to a descending
    // while loop so that its test condition runs faster. Also the variables
    // have been changed to uints instead of ints to make things faster. And
    // the "if" test no longer runs every loop, instead it copies leftover
    // pixels at the end of the strand when the loop is done. It knows whether
    // there are leftover pixels to copy, because the costly division
    // operations have been pre-calculated in the "updateDivTable" routine,
    // instead of being done every time through the loop.
    uint16_t patternWidth = (uint16_t)currentPattern.Width;
    uint16_t n = 0;
    uint16_t copiedPatterns = slitCopyFullRepeatsCount;
    while (copiedPatterns--)
    {
      // Copy the slit array to the LED strand. Syntax of this command is:    
      // memmove8( &destination[start position], &source[start position], size of pixel data )      
      // If you want to see just the color flashes and not the white scanner
      // lights, either comment out the memmove8 lines, or set SCANNER_BRIGHTNESS 0.
      memmove8(&leds[n], &zigzagSlit[0], patternWidth * sizeof(CRGBW));
      n += patternWidth;
    }
    if (slitCopyRemainingLeds > 0)
    {
      // Handle the final leftover slice (if the pattern doesn't divide evenly).
      memmove8(&leds[n], &zigzagSlit[0], slitCopyRemainingLeds * sizeof(CRGBW));
    }

    // Increment to the next line in the image (by fractional sub-pixel).
    // First check the variable which globally toggles animations on and off.
    if (colorCyclingIsOn)
    {
      subPixelOffset ++;
    }

    // This comparison must be ">=" rather than just ">", in order to prevent a
    // small pause in the animation. The pause occurs if the subpixel offset
    // number can be at the maximum value in one frame and then also be 0 in the
    // next frame. For instance with a subpixel resolution of 5, it would go
    // 012345 012345 012345, making the blend values for each animation go 0%,
    // 20%, 40%, 60%, 80%, 100% each cycle. The 100% blend value in that last
    // frame would match the 0% in the next cycle, and so the brightness of the
    // pixel would be the same in both the "0" and the "5" frames because the
    // blend results would be the same in both frames, and the animation would
    // not seem to advance during that frame. This causes a noticeable "frame
    // judder". Instead, it must skip the top value. For instance, with subpixel
    // resolution of 5, it must go 01234 01234. That makes the blend values wrap
    // around correctly at the ends of each cycle.
    if (subPixelOffset >= currentPattern.SubpixelResolution)
    {
      // If we have progressed through all of the subpixels, then it's time to
      // move on to the next line in the image.
      subPixelOffset = 0;
      imageOffset += currentPattern.Width; 

      // The imageOffset is a long int, which goes up to 2147483647, which will
      // take a long long time for this particular code. In any case, we still
      // need to ensure this never crashes by resetting imageOffset back to 0 if
      // it is allowed to get large (for example, if the code is configured to
      // run a single pattern forever and never changes). The reset does not look
      // smooth, but it will happen super-rarely. Still, it must get reset at
      // some point, so that its value never wraps to negative. Note that the
      // comparison can't be ">= 2147483647" because the variable will not ever
      // reach that number before the variable wraps to negative (since it's
      // incremented by pattern width rather than incremented by one). So give
      // the variable some headroom by resetting the value well before it reaches
      // that number.

      //    if ( (imageOffset + currentPattern.Width) >= 1600 )    // Test-Debug a small reset point.
      if ( (imageOffset + currentPattern.Width) >= 2147480000 )    // Reset before it reaches too close to the Long Int limit.
      {
        imageOffset = 0;
        subPixelOffset = 0;
        // Serial.println (F("CE3K animation has wrapped around."));    // Test-Debug message to be notified of the reset point.
      }
    }   
  }  // This bracket ends the "EVERY_N_MILLISECONDS" for the scanner animation frames.

  // Check the variable which globally toggles animations on and off.
  if (colorCyclingIsOn)
  {
    // Call the subroutine to add the color conversation flashes. If you wish to
    // see only the scanner lines and not the color flashes, then comment out
    // this line, or set CONVERSATION_BRIGHTNESS 0.
    CE3Kconversation();
  }
}

#endif