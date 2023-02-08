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

A brief description of the original effect is found on pages 231-232 of Ray
Morton's excellent book, *[Close Encounters of the Third Kind - The Making of
Steven Spielberg's Classic Film](https://www.amazon.com/dp/1557837104)*. I've
had detailed conversations with Robert Swarthe who worked on the original
effect, and I'm writing up a full detailed document about it, which will be
added to this code repository when it's ready.

Special note: This code is intended for use with CRGBW LED strips. Note the "W"
at the end of the "CRGBW". This means that the strips have red, green, and blue
LEDS, but also include a white LED too. I'm deliberately taking advantage of
the separation of RGB from W in my code, but it should be relatively simple to
refactor this code to work with regular CRGB strips, if that's what you're
using.