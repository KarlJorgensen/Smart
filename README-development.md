# Developing/Tweaking Stuff #

You probably want to customise your watch. Here are a few things to beware of.

The watch face consists of 3 parts, which are edited separately:

 * The code - see `Smart.cpp` for details - should be pretty vanilla
   for Arduino folks.

 * The Static Watch Face: This is responsible for most of the "look"
   of the watch face. It is basically a background image.
   
 * The hour hand & minute hands: Line drawings which are rotated,
   scaled and placed at run-time. Lots of lovely trigonometry
   involved.

## Static Watch Face ##

The source for this is `watchface-pixmap.svg` - a perfectly normal SVG
file which you can edit with your favourite editor (e.g. Inkscape).

This _must_ be a square SVG document, as it will result in a square
image.

This gets converted (through a couple of intermediate steps) to a
raster image which gets embedded in the code.

The resulting image will be a very good likeness of the original SVG;
you can basically use all the facilities available in SVG with
near-utter abandon.

Except: The watch has an e-ink screen which _only_ supports black and
white. It does _not_ support any shades of gray:

 * Any pixel which is at least 50% black will be completely black
 
 * Any pixel which is < 50% black will be completely white.

Which means that nice stuff line antialised lines & gradients are a
non-starter.  But if you are careful of aligning things with the pixel
grid: you can use patterns for dithering.

For convenience, `watchface-pixmap.svg` defines the SVG document in
units of pixels (200 × 200), and has a grid to match.

If you want to get a preview of the watch face (static image only),
running `make images` will generate `watchface-pixmap.xbm` which can
be viewed with many image viewers. This is useful for getting quick
feedback on whether the result will look as intended.  If your image
viewer does not support the XBM format you will be relieved to know
that `watchface-pixmap.png` is also generated and you can use that
instead.

If you want to use the Arduino IDE to upload the resulting binary to
the watch: Be sure to run `make headers` first: This will update the
`*.h` file which contains the image.

# The Hour/Minute Hands #

These are line drawings consisting of a series of connected straight
lines - in the code these are referred to as `MultiLine`.

Although they are plain SVG files, they come with a number of
limitations in Smart:

 * The SVG document should be square. Defining it in units of pixels
   is sensible, but not required.

 * The SVG document must contain a `path` element with an `id`
   attribute of `minute_hand` (or `hour_hand` for the ... yes: you
   guessed it: the hour hand). This is what matters.
   
 * The path is translated into a sequence of straight lines. Straight
   lines ONLY. Even if you use curves in the SVG, they will be
   interpreted as straight lines.
   
 * Line (or rather: "stroke") attributes like paint and style are
   ignored. The lines will *always* be drawn as solid 1px lines on the
   watch. Unless `THICK_LINES` is true: Then the lines will be
   thicker.

 * Gaps in the path are not supported. Yet.
   
 * Colours, gradients & fills are ignored. The lines will be black (or
   in dark mode: white) and that's it.

 * Each SVG file should contain _only one_ `<path>` element:
 
    * Paths will will result in C++ variables according to their `id`
      attribute. As both the hour and minute hand are `#include`d into
      the same `*.cpp` file, name collisions are possible. Or the
      names may conflict with other (non-hand) variables/functions.
	  
    * All paths will become part of the resulting binary. And space is
      limited on the watch. You don't want to waste space in there.

 * Hands must be drawn in the 3 o'clock position
 
 * The center of the SVG document corresponds to the center of the
   watch face.
   
 * The edges of the document corresponds to the maximum possible
   extent of the hand - which is _different_ from the edge of the
   watch screen.
   
When the hand is drawn at run-time it will rotated to the correct
position and scaled according to `HANDS_RADIUS`.
