Close Encounters of the Third Kind - Scanner Effect
==============================================================================
&copy; 2023 by Tony Fabris

https://github.com/tfabris/Close-Encounters-Mothership-Scanner/blob/master/Close%20Encounters%20of%20the%20Third%20Kind%20-%20Scanner%20Effect.md

Special thanks to Robert Swarthe and Ray Morton for providing all of the
details and information contained in this document.

## Introduction

![CE3KMovieStill](CE3K%20Movie%20Still.jpg)

Among the revolutionary special effects techniques used in the making of Steven
Spielberg's seminal 1977 film *Close Encounters of the Third Kind*, there is
one very specific effect which deserves detailed attention. This effect is
known as the *scanner*, a ring of moving white lights which surround the hatch
opening on the mothership. The lights moved in interesting patterns which seem
to slide, blend, merge, and separate. The effect is mesmerizing, mysterious,
and ever-so-slightly creepy, perfectly fitting for the scenes in the film where
it's used.

In late 2022, I was writing some computer code to control WS2812 LED lighting
strips, also known as Neopixels. I was programming the LEDs to do many
different fancy moving colored patterns. Since CE3K was an important film for
me in my childhood (I was ten years old at the time), I remembered the light
patterns which had strongly affected me all those years ago. I decided that
reproducing those patterns would be perfect for the LED lighting project.

I found some other people on the internet who were also making their LED strips
dance like the CE3K scanner lights, but none of them looked quite right to me.
The existing versions seemed to be programming fixed-size bars of moving white
lights. My guess was that these other versions didn't feel right because they
were *imitating* the results, rather than trying to directly *simulate* the
process of the original special effect. I decided that I wanted my code to be
an exact simulation if I could manage it.

Great, but... *how* exactly was it done? I realized that I had an only an
inkling of how it might be done. Suddenly I had a new obsession: I had to find
out exactly how this hypnotizing light effect was created.

Analyzing the film, I came to the conclusion that it looked like a variation on
Douglas Trumbull's famous [slit-scan](https://en.wikipedia.org/wiki/Slit-scan_photography)
effect from *2001: A Space Odyssey*. I began working on writing software code to
reproduce the effect based on this concept. However, I needed more details if I
wanted my simulation to be accurate. By the way, my code can be found on
[GitHub here](https://github.com/tfabris/Close-Encounters-Mothership-Scanner),
alongside the latest version of this document.

After a bit of poking around, I found Ray Morton's excellent book,
*[Close Encounters of the Third Kind - The Making of Steven Spielberg's Classic
Film](https://www.amazon.com/dp/1557837104)*. I highly recommend this book.

## The Effect

A general description of the original effect is found on pages 231-232 of Ray
Morton's book. The white lights are long-exposure streaks from a single
brilliant point of light emanating from the end of a fiber optic cable. The
fiber optic cable is connected to a rotating motorized arm, emitting the point
of light as it rotates slowly around the circular opening in the mothership's
underbelly dome. The other end of the fiber optic cable is threaded through the
machinery inside the dome, and off-camera nearby, the cable points at a light
source, in front of which is a piece of glass containing a set of zigzag-shaped
black cutouts made by Robert Swarthe. A mechanical grip, holding the end of the
fiber optic cable, moves horizontally across the glass pane. The black cutouts
cause the fiber optic cable to be illuminated intermittently, causing the light
pattern seen in the film.

So Ray's book has a great description which clearly gives the general idea. A
modern equivalent might be a handheld spinning LED toy, but they didn't have
those in 1977. Okay, so that's the basic concept, but to truly *simulate* the
effect accurately, I still needed even more details than what I found in the
book. 

Thanks to the magic of the internet, and the kind help of some friendly people
(and because I asked very nicely), I was able to get in touch with Robert
Swarthe, who worked on the team that created the effect and who was
specifically mentioned in Ray's book. Robert took the time to answer all my
questions and give as many details as he could remember. This document attempts
to record those details for posterity.

### Remembering the Details

The information below is based on the description in Ray Morton's book, and from
conversations with Robert Swarthe. Robert was awesome: not only did he answer
my questions and tell great stories, he even supplied the animated diagram
included below. Robert worked on several aspects of the film's effects,
alongside Richard Yuricich, Doug Trumbull, Don Trumbull, Dennis Muren, and
Scott Squires, among other SFX artists. Robert was responsible for creating the
zigzag pattern cutouts which were the main source of the scanner's light
patterns, as well hand-animating the colored light flashes which overlaid the
patterns. The other team members created the mechanical fiber optic rig and
photographed it.

Keep in mind that it's been over 40 years since the film was made, and even at
the time, Robert didn't work on every aspect of this effect. The information
below is the best we could put together. Please contact me at tfabris@gmail.com
if you also worked on this film and you have more information or corrections.

### Underbelly - Multiple Passes

The scanner effect takes place on the close-up shots of the underbelly of the
mothership. The underbelly section was built and photographed as a separate
model from the full mothership model. The full model, with its top sections
heavily [greebled](https://en.wikipedia.org/wiki/Greeble), was used for the
long shots of the mothership, and it's currently on display at the
Smithsonian's Udvar-Hazy Center in Chantilly, VA. (I made a pilgrimage once,
and got a bunch of pictures, it's amazing.) The full mothership model does not
have the moving white scanner lights. The underbelly model with the scanner
lights was just a dome painted white, with a circle cut out of its center. It
was mounted on its side in the studio to make it easier to work with during
filming. The camera was turned on its side too, so that the underbelly appeared
upright in the final film.

All of the underbelly details were done with lighting effects, each photographed
in a separate pass and recombined later with an optical printer. This included:
The tiny lights that covered most of the underbelly and which were projected
onto the dome via a front projector; the colored conversation lights which were
hand-animated using an Oxberry animation stand by Robert Swarthe; and the white
scanner lights which were created with the rotating fiber optic rig. 

### Scanner Details

The concept of the scanner effect was Douglas Trumbull's design. It indeed
worked similarly to his slit-scan effect from *2001*. And like the slit-scan
effect, long exposures were an important part of the final look.

- &#x24D8; You can see Robert cutting one of the pattern sets in the photo
    below. The patterns were drawn onto white paper first, and then cut into
    the black paper with an x-acto blade. Robert doesn't recall for sure, but
    thinks it's likely that the black paper would have been Exeter paper, a
    lightproof paper often used for mattes in special effects work. Exeter
    paper was also used as the black background for the starfields in *Close
    Encounters*. From the looks of the photo, the cut patterns were about 18
    inches long or so. (Photo credit: Unknown, photo courtesy of Robert
    Swarthe)

![BobAtWork](Bob%20at%20Work.jpg)

Each long-exposure frame was photographed individually, each exposure taking
about 15 seconds to complete.

- &#x24D8; To do: Double check the exposure length with Robert. I think that's
    what he said during our call, but I could be remembering it wrong.

During each frame, the rig would make one pass. Each pass equaled one revolution
of the "output end" of the fiber optic cable rotating around the circular
opening, while simultaneously, the motorized grip holding the "source end" of
the fiber optic cable would make one horizontal pass across the zigzag pattern
plate. The entire rig would then be reset by hand before exposing the next
frame of film.

- &#x24D8; Important detail: The motorized arm, which held the source end of the
    fiber optic cable, did not move back and forth during the frame exposure.
    It moved only in one direction, making a single horizontal pass across the
    pattern during the frame. After each frame, it was manually reset back to
    its start position. Since one pass of the cable's source end equaled
    exactly one pass of the cable's output end, it means that, for the lighting
    patterns which seem to be repeating evenly-spaced lights, each of them had
    to be carefully cut out, individually. (Animation: Robert Swarthe)

![Animation](CE3K%20Scan%20Animated.gif)

When the rig was reset for the next frame of film, before the next pass, the
source end of the fiber optic cable would get moved slightly vertically, upward
or downward in relation to pattern, just a fraction of an inch. This way, each
frame, the source end of the fiber optic cable would trace a slightly different
path across the pattern, causing the white lights to slowly move in the final
film.

### Multiple Patterns

This is where I get unnaturally obsessive. My long term goal is to accurately
recreate every one of the patterns seen in the theatrical release of the film.

Several different patterns were used which ended up in the final film. Robert
doesn't remember the details of all the patterns which were created, but with
the information above, it should be possible to reverse-engineer them. I will
try to update this section as I get more information.

- The first pattern seen in the film is quite complex. It is used as the
  mothership first descends. The complex pattern merges into large bars which
  then split into several groups of four lights each, which spread apart until
  they're evenly spaced, and then they seem to flicker chaotically.

- The next pattern is seen as the scientists are rolling the organ keyboard
  forward towards the mothership. Differing widths of bars which merge and
  separate. During the scene, if you look closely, you can see this pattern
  reverses backwards onto itself. I'm not sure if the backwards movement is cut
  into the pattern, or if it's done by reversing the direction that the fiber
  optic rig was advancing between each frame, or if the scanner's film element
  was merely looped backwards in the edit in order to extend the length of the
  shot. Any of those three techniques could have produced the same result.

- The pattern for most of the musical sequence and also re-used in some other
  shots throughout the scene, looks like small, evenly-spaced lights which
  counter-rotate, causing them to seem to merge and separate in pairs. My
  initial guess is that the cutout version of this pattern would have looked
  like symmetrical black diamond shapes, or, thinking about it from another
  perspective, it would look like a crosshatch of clear stripes on a black
  background. Another method could have been using two sets of opposing black
  sawtooth shapes, like meshing shark teeth, and then reversing the vertical
  advancement direction of the source arm each time it reaches the top or
  bottom edge of the pattern. If so, then perhaps that's the pattern that
  Robert is cutting out in the photo above?

  - &#x24D8; To do: Find out from Robert if he recalls if that's the way that
      particular pattern was done. Also, if it was done that way, like meshing
      shark teeth, was it meant as a sly tribute to *Jaws*?

- At some moments, a particularly chaotic sequence of lights is displayed,
  including the moment where a loud note causes glass to break. Robert said
  that creating those chaotic patterns involved cutting out a lot of squiggly
  lines.

- A different pattern is seen in a couple of wide shots after the hatch opened
  to disembark the time-dilated abductees. It starts with some widely spaced
  bars which expand, and then split into a repeating pattern of one long slow
  moving bar and three small fast moving bars.

- Different angles of the repeating pattern of one long slow moving bar and
  three small fast moving bars is re-used for several shots.

- A different pattern is seen in a close up shot, above the heads of Richard
  Dreyfuss and François Truffaut as the hatch opens again for the aliens to
  disembark. It starts as the previous pattern, and then moves in some
  interesting ways as Truffaut speaks to Dreyfuss.

## Conclusion

With this information, I'm working on refining my code to make my simulation
more accurate. I hope that this detailed information will be helpful to other
fans of these kinds of visual effects, and I hope that this document can be
preserved via the magic of the internet.
