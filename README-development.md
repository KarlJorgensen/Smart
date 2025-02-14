# Developing/Tweaking Stuff #

You probably want to customise your watch. Here are a few things to beware of.

The watch face consists of 3 parts, which are edited separately:

 * The code - see `Smart.cpp` for details - should be pretty vanilla
   for Arduino folks.

 * The Static Watch Face: This is responsible for most of the "look"
   of the watch face. It is basically a background image.
   
 * The hour & minute hands: Line drawings which are rotated, scaled
   and placed at run-time. Lots of lovely trigonometry involved.

## The Static Watch Face ##

The source for this is `watchface-pixmap.svg` - a perfectly normal SVG
file which you can edit with your favourite editor (e.g. Inkscape).

This _must_ be a square SVG document, as it will result in a square
image.

This gets converted (through a couple of intermediate steps) to a
raster image which gets embedded in the code.

The resulting image will be a very good likeness of the original SVG;
you can basically use all the facilities available in SVG with
near-utter abandon - as long as Inkscape supports it.

Except: The watch has an e-ink screen which _only_ supports black and
white. It does _not_ support any shades of grey:

 * Any pixel which is at least 50% black will be completely black
 
 * Any pixel which is < 50% black will be completely white.

(in dark mode, black and white are obviously reversed)

Which means that nice stuff line antialised lines & gradients are a
non-starter.  But if you are careful of aligning things with the pixel
grid: you can use patterns for dithering.

For convenience, `watchface-pixmap.svg` defines the SVG document in
units of pixels (200 × 200), and has a grid to match.

If you want to get a preview of the watch face (static image only, no
hands), running `make images` will generate `watchface-pixmap.xbm`
which can be viewed with many image viewers. This is useful for
getting quick feedback on whether the result will look as intended.

If your image viewer does not support the XBM format you will be
relieved to know that `watchface-pixmap.png` is also generated and you
can use that instead.

If you want to use the Arduino IDE to upload the resulting binary to
the watch: Be sure to run `make headers` first: This will update the
`*.h` file which contains the image.

## The Hour/Minute Hands ##

These are line drawings consisting of a series of connected straight
lines - in the code these are referred to as `MultiLine`.

Although they are plain SVG files (named `hour_hand-lines.svg` and
`minute_hand-lines.svg` respectively), they come with a number of
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
   
 * The edges of the document corresponds to the edges of the
   screen. Design your hands with suitable lengths to fit the watch
   face.
   
When the hand is drawn at run-time it will rotated to the correct
position.

Remember that positions other than 3/6/9/12 o'clock will thus
unavoidably introduce "jaggered" lines. So the perceived thickness of
lines may be different at different angles, which affects the
"look". Be sure to test the layout.

# Getting Screenshots #

Getting a screen shot of your screen face is far more cumbersome than
on a PC, and requires a separate build.

It takes a couple of steps:

 * Modify `settings.h` and

   * set `SCREENSHOT_MODE` to a nonzero value
   * set `SCREENSHOT_HOUR` and `SCREENSHOT_MINUTE` to the time you
     want the face to show.
   * set `SERIAL_SPEED` to your desired serial port baud rate

 * Patch `../libraries/GxEPD2/src/GxEPD2_BW.h` using
   `GxEPD2_BW.h.patch`.

   I know. This is ugly as it is _literally_ modifying a
   3rd party library, but the [official
   directions](https://github.com/sqfmi/Watchy/wiki/Screenshots-of-Watchfaces)
   aren't any better. Perhaps that is the Arduino way?

   My patch produces Plain PBM format output instead.

   The patch can be applied with e.g. (adjust paths as necessary):

        patch ../libraries/GxEPD2/src/GxEPD2_BW.h GxEPD2_BW.h.patch

   NOTE: This patch was done against version 1.6.1 of GxEPD2. If your
   version is different the patch may not apply.

 * Compile and upload to the watch

 * Once the upload is done connect a serial terminal to
   `/dev/ttyACM0`.

   This should not be done before the upload is complete as the upload
   is also using the same serial port. Having both processes access
   the serial port simultaneously is bound to break things.

   In Emacs, you can use the `serial-term` command for
   this. Alternatively, you can use e.g. `minicom`, and others are
   available too.

 * The watch will now produce output to the serial terminal when it
   redraws the screen - i.e. once per minute.

 * Capture the output and pipe it into `pnmtopng` and send the output
   to a file of your choosing - e.g.:

        pnmtopng < serial-capture.txt > screenshot.png

And you (finally) have a screenshot. If you replace the original
`screenshot.png` in this directory, it should show nicely in the
`README.md`.

Once the code (and screenshot) is to your liking, you can revert
things again:

 * Modify `settings.h` and

   * set `SCREENSHOT_MODE` to zero

 * If you used the patch above, it can be reverted with e.g. (adjust
   paths as necessary):

        patch --reverse ../libraries/GxEPD2/src/GxEPD2_BW.h GxEPD2_BW.h.patch

 * Compile and upload to the watch.

 * The watch should now show the correct time.
