Close-Encounters-Mothership-Scanner
==============================================================================
&copy; 2023 by Tony Fabris

https://github.com/tfabris/Close-Encounters-Mothership-Scanner

Arduino code for the FastLED library using WS2812 LED strips, also known as
Neopixels. This code is intended to run on an Arduino Mega, but it could be
adapted for other processors. This code is trying to accurately simulate this:

<center><p style="text-align: center;">

![CE3KMovieStill](CE3K%20Movie%20Still%20Small.jpg)

</p></center>

This code tries to reproduce the "Scanner ring" effect from the mothership from
the Steven Spielberg film *Close Encounters of the Third Kind*. When the
mothership extends its airlock hatch, it emerges from a circular opening which
has a ring of white fiber-optic lights which are moving in an interesting
pattern. This code attempts to translate that physical long-exposure effect
into addressable LED strips. My goal is to perform an accurate simulation of
the effect, rather than just an imitation.

### Original Effect:

This is an [article with full details](https://bit.ly/CE3K-Scanner) of how the
original effect was done, including behind the scenes photos from some of the
legendary effects artists who created the original effect:

<center><p style="text-align: center;">

***https://bit.ly/CE3K-Scanner***

[![EffectArticleThumbnail](Effect%20Article%20Thumbnail.jpg)](https://bit.ly/CE3K-Scanner)

</p></center>

### Videos:
- This code running on my LED shelves: https://youtu.be/tTZtArMXk0Y
- Part of the original scene in the film: https://youtu.be/pu-W292TsRM?t=223
- Part of the original scene in the film: https://youtu.be/S4PYI6TzqYk

### LED Source code:
- https://github.com/tfabris/Close-Encounters-Mothership-Scanner

### Notes:
- This code is a work in progress, it doesn't fully simulate the original
  effect... yet. See the [issues page](../../issues) if you want to know what's
  still to-do.
- Most of the explanation of the LED code details can be found in the file
  [Close Encounters Mothership Scanner.h](Close%20Encounters%20Mothership%20Scanner.h)
- This code is intended for use with CRGBW LED strips. Note the "W" at the end
  of the "CRGBW". This means that the strips have red, green, and blue LEDS,
  but also include a white LED too. I'm deliberately taking advantage of the
  separation of RGB from W in my code, but it should be relatively simple to
  refactor this code to work with regular CRGB strips, if that's what you're
  using.
