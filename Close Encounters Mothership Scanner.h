#ifndef Close_Encounters_Mothership_Scanner_h
#define Close_Encounters_Mothership_Scanner_h

// ---------------------------------------------------------------------------
// Close Encounters Mothership Scanner by Tony Fabris
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
// There are some other existing pieces of prior art which attempt to do the
// same effect. But those versions are not trying to exactly duplicate the
// method that was used for creating the original visual effect in the movie.
// My version attempts to recreate it more closely, by trying to perform a more
// direct simulation of the original effect. 
//
// Prior art examples (I am not using these):
//
// - https://gist.github.com/jasoncoon/d4c935566ec82731fece
// - https://youtu.be/aEVh9WNl57A
//
// Description of the original effect, from the book "Close Encounters of the
// Third Kind - The Making of Steven Spielberg's Classic Film" by Ray Morton:
//
//  "Another important lighting effect on the underbelly was the scanner- a
//   series of white lights that rotate around the circular cutout from which
//   the hatch emerges. To create the scanner, a piece of fiber-optic cable was
//   attached to a tiny motorized arm that circled around the cutout at the rate
//   of one revolution per frame. The cable was threaded down through the
//   underbelly and attached to a motorized grip that moved back and forth on a
//   track set up in front of a very bright light source. Robert Swarthe cut
//   several zigzag patterns out of pieces of shiny black paper, which were then
//   mounted in glass and placed between the light source and the motorized
//   grip. During filming, the grip would move past the patterns. The black
//   paper would block the light coming from the source, but when the grip moved
//   over an open area in the pattern, the light would shine through and the
//   fiber optic would transmit it to the scanner arm. This on-again, off-again
//   pattern broke the scanner line into segments. The long exposures employed
//   to shoot each frame caused the lights to streak a bit. As a result, some of
//   the scanner lights appeared to expand and contract, while others appeared
//   to merge with and separate from one another. Like the other light patterns,
//   the scanner effect was filmed in its own separate pass."
//
// I'm currently trying to contact Robert Swarthe for more details. I'm
// interested in finding old photos of the fiber optic rig, and in particular,
// I'm hoping that photos exist of the original transparencies he created for
// the effect. The simulation could be potentially very accurate if we could
// find out what the original zigzag patterns actually looked like. If anyone
// can help me find his contact information, please let me know at
// tfabris@gmail.com .
//
// Special note: This code is intended for use with CRGBW LED strips. Note
// the "W" at the end of the "CRGBW". This means that the strips have red,
// green, and blue LEDS, but also include a white LED too. I am using "BTF"
// brand SK6812 RGBW strips (https://www.amazon.com/dp/B079ZW1265), but any
// brand will work. I'm deliberately taking advantage of the separation of RGB
// from W in my code, but it should be relatively simple to refactor this code
// to work with regular CRGB strips, if that's what you're using.
// ---------------------------------------------------------------------------

// Set up variables

// This variable controls the width of a "slit" view of the zigzag patterns.
// This must be the same as the X width of the zigzag pattern arrays.
#define ZIGZAG_SLIT_WIDTH 44

// This is an array of values that represents the "slit" view of the zigzag
// patterns. Note: This code was written for CRGBW strip hardware; if you are
// using CRGB hardware, you'll need to refactor some parts of this code.
CRGBW zigzagSlit[ZIGZAG_SLIT_WIDTH];

// This code will scroll the view of the "slit" up and down the zigzag pattern
// image. It will do so in units which are smaller than each line, in order to
// get sub-pixel resolution of the movement of the image. This variable
// controls the number of "steps" which will occur between each line of the
// image. The finer the subpixel resolution (the larger this variable is), then
// the greater number of steps, so the slower that this simulation will scroll.
// In this way, this variable is what controls the "speed" of the simulation.
// Note that this code is only doing subpixel resolution in the vertical
// dimension, not horizontal, so all antialiasing occurs vertically
// line-to-line, not sideways pixel-to-pixel.
#define SUBPIXEL_RESOLUTION 8   // This controls the speed of the white bars.

// These variables control the brightness value of the white bars in the idle
// portion of the scanner effect, and the V of the HSV values of the flashing
// color conversation lights which appear atop the scanner. These can be a
// number between 0 and 255. Adjust the scanner brightness down from 255 in
// order to keep the color conversation flashes from being overwhelmed by the
// brilliance of the white bars. Due to the way the HSV calculations work,
// there's little difference in the brightness of the color flashes when you
// adjust the CONVERSATION_BRIGHTNESS value between 1 and 255. To "mute" one or
// the other of these animations, set the corresponding value to zero.
#define SCANNER_BRIGHTNESS             190
#define CONVERSATION_BRIGHTNESS        255

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
#define CONVERSATION_FLASH_FREQUENCY   990 // Each blank frame, a random number of 0-1000 must exceed this number to start a new color flash (higher is less likely).

// These pixel arrays are supposed to imitate Robert Swarthe's zigzag patterns,
// and will be overlaid atop each other. Make the width of the array the same
// as ZIGZAG_SLIT_WIDTH and the height of each array can be any height you
// want. Each array can be a different *height*, but all arrays must be the
// same *width*. For best results, design each image in such a way that it
// wraps around smoothly at the top/bottom and sides. The code allows arrays of
// different heights in order to help with being able to design wrap-around
// images, so different arrays can wrap at different points in the time
// dimension.
//
// To experiment with different patterns, look in in the same directory as this
// file. There should be another file called "Alternate Zigzag Patterns.txt"
// which will have some other options. Try some of those patterns too.
//
// TO DO: In the CE3K film, there are several different sets of lighting
// patterns which appear at different times in the scene. If we can learn the
// correct zigzag patterns to replicate them all, try to code a way to
// occasionally transition between them. 
bool zigzagImage01[] = 
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

bool zigzagImage02[] = 
{
  1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,
  1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,
  1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,
  1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,
  1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
  0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
  0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,
  0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,
  0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,
  0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,
  0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,
  0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,
};

// ---------------------------------------------------------------------------
// Function to grab one pixel out of the overlaid zigzag arrays, and turn it
// into a brightness value that can be applied to the LEDs.
// ---------------------------------------------------------------------------
int pixelValue (long arrayPosition)
{
  // The arrays are coded as bool, either 0 or 1, to make it easier to type into
  // the zigzagImage arrays. Then this ANDs or ORs the multiple arrays together
  // so that any black stripes or white stripes are preserved. (Using AND
  // preserves black stripes, using OR preserves white stripes.) This simulates
  // the original fiber-optic effect where multiple transparencies were
  // overlaid atop each other, blocking the light source from the fiber optic
  // strand.
  bool blackOrWhitePixel =
  (
    // Each array will be smaller than the arrayPosition variable, but by using
    // the modulo operator (%) we can dig into the arrays at the correct
    // position points without needing complicated math. Basically, the
    // arrayPosition cycles continuously through a very large range of numbers,
    // and the "%" operator is the math that finds out what the array position
    // would have been if it had started within this small array. This allows
    // the code to support zigzag arrays of any size.
    //
    // For different patterns, experiment with using and "or" ("|") below, which
    // will preserve white pixels, instead of using "and" ("&") below, which
    // preserves black pixels.
    zigzagImage01[arrayPosition % sizeof(zigzagImage01)] &
    zigzagImage02[arrayPosition % sizeof(zigzagImage02)]
  );

  // Convert the binary 0 or 1 value into a pixel darkness value, i.e., 0
  // becomes 0, 1 becomes full brightness. (No shades of gray at this point;
  // the code antialiases elsewhere.)
  return blackOrWhitePixel * SCANNER_BRIGHTNESS;
}

// ---------------------------------------------------------------------------
// Subroutine to add the colored flashing "conversation" lights, atop the 
// moving white "idle" animation bars.
// ---------------------------------------------------------------------------
void CE3Kconversation()
{
  // Only one color flash will appear at a time, and also it has to go through
  // several frames of animation for the entire flash. Keep track of both
  // things with the variable "flashStage": flashStage 0 means no flash is
  // occurring, flashStage nonzero means that we are in the middle of a color
  // flash animation. Each time a frame of the animation is played, it
  // increments or decrements flashStage (incrementing when the color bar is
  // swelling up to its full length, decrementing when it's unswelling down to
  // nothing). A separate variable, flashFrames, keeps track of the number of
  // swelling frames in this animation (each flash will have a different size
  // and thus a different number of frames). Then if flashStage reaches
  // flashFrames, that means it's the last frame of the swell-up animation and
  // it then dwells for a bit, then switches direction and decrements down to
  // zero. Once it hits zero, then the animation is done and it waits until the
  // next color flash is triggered. reset until the next one is triggered.
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
  float       onePixelBrightness = 0.0;

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
        halfWayMark = colorBarWidth / 2;

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
        // giving the note a sharper attack than the note's decay.
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
      colorBarTempWidth = flashStage / 2;
 
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
// Main loop of the CE3K scanner effect. This routine should be called once
// per "loop()" of the main Arduino code. This routine is responsible for
// generating the moving white "idle" bars of the scanner ring animation.
// ---------------------------------------------------------------------------
void ce3kScanner()
{
  static long imageOffset = 0;   // Which line of the zigzag arrays are we on?
  static int subPixelOffset = 0; // Move through the arrays slowly while antialiasing.

  // Perform actions which are only needed on the first run of this function.
  static bool firstTime = true;
  if (firstTime)
  {
    // The first time this is called, initialize the slit view array to black.
    fill_solid( zigzagSlit, ZIGZAG_SLIT_WIDTH, CRGBW(0,0,0,0) );
    firstTime = false;
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
  static float oneSubPixelWeightUnit = ((float)1 / (float)SUBPIXEL_RESOLUTION);
  float blendWeight = subPixelOffset * oneSubPixelWeightUnit;

  // TO DO: The blend above is a purely linear blend. Unfortunately the LEDs do
  // not have a purely linear brightness based on the numbers pumped into them.
  // The lowest brightness level of an LED is significantly brighter than when
  // the LED is just "off". This makes the antialiased pixels kind of "pop"
  // from fully dark to dimly-lit in the final animation. This causes the
  // transition points between black and the white pixels to seem to sort
  // of "caterpillar" across the strand instead of flowing perfectly smoothly.
  // It would be nice if I could come up with a nonlinear blend to make it
  // seem more smooth.

  // TO DO: The floating point calculations are a bit slow. Idea: Use a table of
  // blend values instead of doing the FP math each time, to improve the speed
  // of the animation. I could pre-calculate the blend values the first time
  // through the loop, and store them in a table, instead of realtime
  // calculating the blend value with floating point math each time through the
  // loop. Not sure how much this would improve the speed.

  // Assemble the current slit view into the slit array.
  for (int x=0; x<ZIGZAG_SLIT_WIDTH; x++ )
  {
    // Obtain the current pixel location, and also the pixels on the prior row
    // and the following row (for antialiasing).
    long thisPixelPosition = x+imageOffset;
    long nextPixelPosition = thisPixelPosition+ZIGZAG_SLIT_WIDTH;
    long lastPixelPosition = thisPixelPosition-ZIGZAG_SLIT_WIDTH;

    // Get the value of the pixels of the image arrays. 
    int lastPixelDarkness = pixelValue(lastPixelPosition);
    int thisPixelDarkness = pixelValue(thisPixelPosition);
    int nextPixelDarkness = pixelValue(nextPixelPosition);

    // Blend the next and previous line's pixels into the current line's pixel.
    // There is a blend feature in FastLED to do this for us, which does
    // essentially the same thing, but it works on three values (r,g,b) and I
    // only need one value since the scanner is in black and white. I could use
    // FastLED's function but, it might be slower since it has to do the math
    // three times as much. So I'm rolling my own blend math, based on this:
    // http://www.designimage.co.uk/quick-tip-the-maths-to-blend-between-two-values/
    // TO DO: Refactor to use FastLED's blend routine and see if it's faster.
    float lastBlend = (thisPixelDarkness*blendWeight)+(lastPixelDarkness*(1-blendWeight));
    float nextBlend = (nextPixelDarkness*blendWeight)+(thisPixelDarkness*(1-blendWeight));
    int blendedDarkness = (lastBlend+nextBlend)/2;

    // Apply the final values to the array that represents the slit. I'm using
    // only the White LED in the CRGBW array here, so the colored conversation
    // lights can be painted separately without having to blend them with the
    // white LEDs. If you are using CRGB LEDs, you'll have to refactor this.
    zigzagSlit[x].white = (int)blendedDarkness;

    // // ---------------------------------------------------------------------------
    // // Debugging printouts.
    // FastLED.show();
    // Serial.println("");
    // Serial.println("x = " + String(x));
    // Serial.println("sizeof(zigzagImage01) = " + String(sizeof(zigzagImage01)));
    // Serial.println("imageOffset = " + String(imageOffset));
    // Serial.println("oneSubPixelWeightUnit = " + String(oneSubPixelWeightUnit));
    // Serial.println("blendWeight = " + String(blendWeight));
    // Serial.println("subPixelOffset = " + String(subPixelOffset));
    // Serial.println("lastPixelPosition = " + String(lastPixelPosition));
    // Serial.println("thisPixelPosition = " + String(thisPixelPosition));
    // Serial.println("nextPixelPosition = " + String(nextPixelPosition));
    // Serial.println("lastPixelDarkness = " + String(lastPixelDarkness));
    // Serial.println("thisPixelDarkness = " + String(thisPixelDarkness));
    // Serial.println("nextPixelDarkness = " + String(nextPixelDarkness));
    // Serial.println("lastBlend = " + String(lastBlend));
    // Serial.println("nextBlend = " + String(nextBlend));
    // Serial.println("blendedDarkness = " + String(blendedDarkness));
    // Serial.println("");
    // // Wait for keypress
    // while (Serial.available() < 1)
    // {
    //   delay(10);
    // }
    // while (Serial.available() > 0)
    // {
    //   byte dummyread = Serial.read();
    // }
    // // ---------------------------------------------------------------------------
  }

  // Copy the slit array onto the entire LED strand. If ZIGZAG_SLIT_WIDTH is
  // less than the total number of LEDs, then it will copy it multiple times.
  // If ZIGZAG_SLIT_WIDTH is larger than the total number of LEDs, it will
  // copy only the relevant subsection. More details and example found here:
  // https://github.com/marmilicious/FastLED_examples/blob/master/memmove8_pattern_copy.ino
  for (int n=0; n<NUM_LEDS; n+=ZIGZAG_SLIT_WIDTH)
  {
    // Precalculate a number to prevent myself from writing past the end of
    // the CRGBW array. This variable will be the same as ZIGZAG_SLIT_WIDTH
    // for most of the copy operations, and then on the last copy operation,
    // it will be less than ZIGZAG_SLIT_WIDTH because it is copying only up
    // to the end of the LEDs array.
    int numLedsToCopy = ZIGZAG_SLIT_WIDTH;
    if (n+ZIGZAG_SLIT_WIDTH >= NUM_LEDS) 
    {
      numLedsToCopy = NUM_LEDS-n;
    }

    // Copy the slit array to the LED strand. Syntax of this command is:    
    // memmove8( &destination[start position], &source[start position], size of pixel data )
    //
    // NOTE: In my LED array I'm using CRGBW strand hardware, but if yours is
    // CRGB, you'll need to refactor the code to accept CRGB.
    //
    // If you want to see just the color flashes and not the white scanner
    // lights, either comment out this line or set SCANNER_BRIGHTNESS 0.
    memmove8(&leds[n], &zigzagSlit[0], numLedsToCopy*sizeof(CRGBW));
  }

  // Call the subroutine to add the color conversation flashes. If you wish to
  // see only the scanner lines and not the color flashes, then comment out
  // this line, or set CONVERSATION_BRIGHTNESS 0.
  CE3Kconversation();

  // Increment to the next line in the image (by fractional sub-pixel).
  subPixelOffset ++;

  // This comparison must be ">=" rather than just ">", in order to prevent a
  // small pause in the animation. The pause occurs if the subpixel offset
  // number can be at the maximum value in one frame and then also be 0 in the
  // next frame. For instance with a subpixel resolution of 5, it would go
  // 012345 012345 012345, making the blend values for each animation go 0.00,
  // 0.20, 0.40, 0.60, 0.80, 1.00 each cycle. The 1.00 blend value in that last
  // frame would match the 0.00 in the next cycle, and so the brightness of the
  // pixel would be the same in both the "0" and the "5" frames because the
  // blend results would be the same in both frames, and the animation would
  // not seem to advance during that frame. This causes a noticeable "frame
  // judder". Instead, it must skip the top value. For instance, with subpixel
  // resolution of 5, it must go 01234 01234. That makes the blend values wrap
  // around correctly at the ends of each cycle.
  if (subPixelOffset >= SUBPIXEL_RESOLUTION)
  {
    // If we have progressed through all of the subpixels, then it's time to
    // move on to the next line in the image.
    subPixelOffset = 0;
    imageOffset += ZIGZAG_SLIT_WIDTH; 

    // The imageOffset is a long int, which goes up to 2147483647, which will
    // take a long long time for this particular code. In any case, we still
    // need to ensure this never crashes by resetting imageOffset back to 0
    // occasionally. The reset does not look smooth, but it will happen
    // super-rarely. Still, it must get reset at some point, so that its value
    // never wraps to negative. For example, the comparison can't be ">=
    // 2147483647" because the variable will not ever reach that number before
    // the variable wraps to negative. In the final code, give the variable
    // some headroom by resetting the value well before it reaches that number.

    // TO DO: In the original CE3K film, there are some interesting moments in
    // the animation where the lights appear to briefly flicker in random
    // patterns. My guess is that this occurred when the "source" end of the
    // fiber optic rig hit the endpoint of its travel, or ran up against
    // (or past) the edge of one of the glass plates, or some other unexpected
    // malfunction of the rig. Normally this kind of footage wouldn't be used,
    // but my guess is that the team thought it looked interesting and
    // specifically lampshaded it in the final cut; a classic "happy accident"
    // moment. Someday I intend to simulate that light pattern as well, as a
    // special case, and this reset point could be modified so that
    // special "flickering" simulation is triggered whenever this reset point
    // wraps around. Then we could adjust the reset point so that the speical
    // animation happens at regular intervals.

    // For testing and debugging, you can temporarily change the reset point a
    // smaller number, so the reset will be visible in just a few loops. 
    //    if ( (imageOffset + ZIGZAG_SLIT_WIDTH) >= 1600 )    // Test-Debug a small reset point.
    if ( (imageOffset + ZIGZAG_SLIT_WIDTH) >= 2147480000 )    // Reset before it reaches too close to the Long Int limit.
    {
      imageOffset = 0;
      subPixelOffset = 0;
      // Serial.println (F("CE3K animation has wrapped around."));    // Test-Debug message to be notified of the reset point.
    }
  }       
}

#endif