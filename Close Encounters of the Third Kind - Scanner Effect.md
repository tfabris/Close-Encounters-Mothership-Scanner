Close Encounters of the Third Kind - Scanner Effect
==============================================================================
&copy; 2023 by Tony Fabris

https://bit.ly/CE3K-Scanner

Special thanks to Robert Swarthe, Scott Squires, Dennis Muren, and Ray Morton for providing
all of the details, photos, and information contained in this document.

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
underbelly dome. A set of zigzag-shaped black patterns, made by Robert Swarthe,
are covering a light source. The patterns moved so that the fiber optic cable
would illuminate intermittently, causing the light patterns seen in the film.

So Ray's book has a great description which clearly gives the general idea. A
modern equivalent might be a handheld spinning LED toy, but they didn't have
those in 1977. Okay, so that's the basic concept, but to truly *simulate* the
effect accurately, I still needed even more details than what I found in the
book. 

Thanks to the magic of the internet, and the kind help of some friendly people
(and because I asked very nicely), I was able to get in touch with Robert
Swarthe, the animator who was specifically mentioned in Ray's book as the one
who made the black patterns. Robert put me in touch with Scott Squires as well,
who photographed the sequence and who worked directly with Robert.

Robert and Scott have been awesome. They supplied some of the photos below, and
Robert even made an animated diagram, also included below. They contacted
Dennis Muren, who sent along even more photographs. They took the time to
answer all my questions and gave as many details as they could remember. This
document attempts to record those details for posterity.

### Remembering the Details

The information below is based on the description in Ray Morton's book, and from
conversations with Robert Swarthe and Scott Squires. Robert and Scott worked on
several aspects of the film's effects, alongside Richard Yuricich, Doug
Trumbull, Don Trumbull, and Dennis Muren, among other SFX artists. Robert was
responsible for creating the zigzag patterns which were the main source of the
scanner's light patterns, as well hand-animating the colored light flashes
which overlaid the patterns. The mechanical fiber optic rig was likely designed
and built by Richard Yuricich, Don Trumbull, and other team members. Scott
Squires and Dennis Muren did the photography.

Keep in mind that it's been over 40 years since the film was made, and no single
person worked on every aspect of this effect. The information below is the best
we could assemble so far, from multiple different accounts. Please contact me
at tfabris@gmail.com if you also worked on this film and you have more
information or corrections.

### Underbelly - Multiple Passes

The scanner effect takes place on the close-up shots of the underbelly of the
mothership. The underbelly section was built and photographed as a separate
model from the full mothership model. The full model, with its top sections
heavily [greebled](https://en.wikipedia.org/wiki/Greeble), was used for the
long shots of the mothership, and it's currently on display at the
Smithsonian's Udvar-Hazy Center in Chantilly, VA. (I made a pilgrimage once,
and got a bunch of pictures, it's amazing.) The full mothership model does not
have the moving white scanner lights, those are only seen on the close-up
underbelly section.  The underbelly model with the scanner lights was just a
dome painted white, with a circle cut out of its center. It was mounted on its
side in the studio to make it easier to work with during filming. The camera
was turned on its side too, so that the underbelly appeared upright in the
final film.

All of the underbelly details were done with lighting effects, photographed in
separate passes. The tiny lights which formed the underbelly were created with
slowly rotating moiré patterns projected through various color filters onto the
curved white surface of the model. The white scanner lights using the rotating
fiber optic rig were photographed on the same negative. This film was developed
and sent to the optical printing department where it was combined with the
live-action footage and a matte painting. Then, the un-developed optical
internegative was sent to the animation department where the colored
conversation lights were added, hand-animated, using an Oxberry animation stand
by Robert Swarthe and Scott Squires.

### Scanner Details

The concept of the scanner effect was Douglas Trumbull's design. It indeed
worked similarly to his slit-scan effect from *2001*. And like the slit-scan
effect, long exposures were an important part of the final look.

- &#x24D8; You can see Robert Swarthe cutting one of the pattern sets in the
    photo below. The patterns were drawn onto white paper first, and then cut
    into black Exeter paper with an X-Acto blade. Exeter paper is a lightproof
    paper which was popularly used for mattes in special effects work. Exeter
    paper was also used as the black background for the starfields in *Close
    Encounters*, because the camera sees only the white stars and not the
    intense black background. From the looks of the photo, the cut patterns
    were about 18 inches long or so. Robert says that these paper cut-outs were
    probably made into high-contrast 8"x10" positive and negative film
    transparencies for use in the scanner rig. (Photo credit: Unknown. Photo
    courtesy of Robert Swarthe)
    ![BobAtWork](Bob%20at%20Work.jpg)

Each long-exposure frame was photographed individually, with each exposure
taking 30 seconds or more to complete. During each frame, the rig would make
one pass. Each pass equaled one revolution of the fiber optic cable rotating
around the circular opening, while simultaneously, the rig with the patterns
was moved in sync.

According to Scott Squires, the rig with the patterns was an X/Y animation light
table on its side, motorized by a MiniScan motion control system. In front of
the pattern was a photocell attached to a light stand. The photocell stayed
fixed, while the animation stand with the patterns moved past it. Behind the
pattern, on the opposite side of the animation stand, was a light source
opposite the photocell.

- &#x24D8; Photo of one of the scanner patterns on the animation stand, with the
    photocell. This pattern looks like it was originally created with tape
    lines and then made into a transparency. (Photo: Dennis Muren)
    ![PatternOnTheStand](Pattern%20on%20the%20Stand%201.jpg)

The fiber optic cable in the mothership dome model, which emitted the scanner
light, had its own light source and an electronic shutter, which was wired to
open and close depending on the photocell in front of the pattern. Any light
hitting the photocell would cause the light source's electronic shutter to
open, causing light to emit from the fiber optic assembly.

The pattern would make one horizontal pass across the photocell for each
rotation of the fiber optic cable. The entire rig would then reset before
exposing the next frame of film.

- &#x24D8; Important detail: The system which moved the pattern in relation to
    the photocell didn't move it back and forth during the frame exposure. It
    moved only in one direction, making a single horizontal pass during the
    frame. After each frame, it was reset back to its start position. Since one
    pass of the pattern equaled one pass of the cable's output end, it means
    that, for the lighting patterns which seem to be repeating many
    evenly-spaced lights, each of them was individually created as part of the
    pattern. (Animation: Robert Swarthe)
    ![Animation](CE3K%20Scan%20Animated.gif)

When the rig reset for the next frame of film, before the next exposure, the
pattern would get moved slightly vertically in relation to the photocell, just
a fraction of an inch. This way, each frame, the photocell would trace a
slightly different path across the pattern. According to Scott Squires, the
patterns were usually made in diagonal shapes, to cause the lights to rotate
with each vertical step of the pattern. Patterns with long tapers would cause
the the lights to seem to widen or shrink.

- &#x24D8; Test Polaroids of the "B" and "C" patterns. Note how the Polaroids
    are labeled "White Open" or "Black Open". Scott says that it's likely that
    the system could be changed between those two modes: "White Open" would
    open the electronic shutter when the photocell encountered light,
    and "Black Open" would do the opposite, closing the electronic shutter when
    the photocell encountered light. (Photos: Scott Squires)
  | ![Test Polaroid B C Front](Test%20Polaroid%20B%20C%20Front.jpeg) | ![Test Polaroid B C Back](Test%20Polaroid%20B%20C%20Back.jpeg) |
  | -----------------------------------------------------------------|----------------------------------------------------------------|
  | ![Test Polaroid C C Front](Test%20Polaroid%20C%20C%20Front.jpeg) | ![Test Polaroid C C Back](Test%20Polaroid%20C%20C%20Back.jpeg) |

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
  forward towards the mothership. The pattern is composed of differing widths
  of bars which merge and separate. During the scene, if you look closely, you
  can see this pattern seems to change direction and reverse during the shot.
  According to Scott Squires, this reversal would have been done within the
  pattern itself, which Scott says would have been like cross hatch lines.

- The pattern for most of the musical sequence and also re-used in some other
  shots throughout the scene, looks like small, evenly-spaced lights which
  counter-rotate, causing them to seem to merge and separate in pairs. This
  would have been created by lines making long diamond-shaped patterns; a photo
  above, showing the pattern on the animation stand, looks like it might be
  this pattern.

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

### Colored Flashes

The colored flashing "conversation" lights for the musical sequence were
hand-animated by Robert Swarthe, with Scott Squires assisting and operating the
Oxberry camera.

Robert came up with something he called "pivot gimmicks", an assembly of
cardboard and paper punch brads, to make the color bars open and close, to
synchronize with the music. The curve of the colored lights, the top and bottom
edges, were masked with Exeter paper, and the pivot gimmicks were used for
expanding and contracting the width of the color bars. Robert made a set of
[exposure sheets](https://en.wikipedia.org/wiki/Exposure_sheet) with
instructions for each frame, and Scott Squires followed those instructions,
hand-moving the pivot gimmicks for each frame.

In the "making of" documentary included in the 2001 DVD Collectors Edition of
the film, Robert says that the color choices of the lights were his own
personal choices, he wasn't given any directions as to what color each light
should be.

## Conclusion

With this information, I'm working on refining my code to make my simulation
more accurate. I hope that this detailed information will be helpful to other
fans of these kinds of visual effects, and I hope that this document can be
preserved via the magic of the internet.
