Close-Encounters-Mothership-Scanner
==============================================================================
&copy; 2023 by Tony Fabris

https://github.com/tfabris/Close-Encounters-Mothership-Scanner

Arduino code for the FastLED library using WS2812 LED strips, also known as
Neopixels. This code is intended to run on an Arduino Mega, but it could be
adapted for other processors.

This is an attempt to simulate the "Scanner ring" effect from the mothership
from the Steven Spielberg film *Close Encounters of the Third Kind*. When
the mothership extends its airlock hatch, it emerges from a circular opening
which has a ring of white fiber-optic lights which are moving in an
interesting pattern. This code attempts to translate that physical
long-exposure effect into addressable LED strips.

Videos:
- This code running on my LED shelves: https://youtu.be/tTZtArMXk0Y
- Part of the original scene in the film: https://youtu.be/pu-W292TsRM?t=223
- Part of the original scene in the film: https://youtu.be/S4PYI6TzqYk

Description of the original effect, from the book *[Close
Encounters of the Third Kind - The Making of Steven Spielberg's
Classic Film](https://www.amazon.com/dp/1557837104)* by Ray Morton:

> "Another important lighting effect on the underbelly was the scanner- a
>  series of white lights that rotate around the circular cutout from which
>  the hatch emerges. To create the scanner, a piece of fiber-optic cable was
>  attached to a tiny motorized arm that circled around the cutout at the rate
>  of one revolution per frame. The cable was threaded down through the
>  underbelly and attached to a motorized grip that moved back and forth on a
>  track set up in front of a very bright light source. Robert Swarthe cut
>  several zigzag patterns out of pieces of shiny black paper, which were then
>  mounted in glass and placed between the light source and the motorized
>  grip. During filming, the grip would move past the patterns. The black
>  paper would block the light coming from the source, but when the grip moved
>  over an open area in the pattern, the light would shine through and the
>  fiber optic would transmit it to the scanner arm. This on-again, off-again
>  pattern broke the scanner line into segments. The long exposures employed
>  to shoot each frame caused the lights to streak a bit. As a result, some of
>  the scanner lights appeared to expand and contract, while others appeared
>  to merge with and separate from one another. Like the other light patterns,
>  the scanner effect was filmed in its own separate pass."

I've had detailed conversations with Robert Swarthe and gotten more information
about the zigzag patterns, with the intention of making the simulation more
accurate. I'm also writing up a full detailed document about the original
effect which will be added to this code repository when it's ready.

Special note: This code is intended for use with CRGBW LED strips. Note the "W"
at the end of the "CRGBW". This means that the strips have red, green, and blue
LEDS, but also include a white LED too. I'm deliberately taking advantage of
the separation of RGB from W in my code, but it should be relatively simple to
refactor this code to work with regular CRGB strips, if that's what you're
using.