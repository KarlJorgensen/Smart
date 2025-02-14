# Smart #

A watch face for the [sqfmi Watchy](https://watchy.sqfmi.com/) watch
and compatible ones.

This is a simple analogue watch face without any frills.

It is a little different from other watchy projects out there in a
couple of respects:

 * The static part of the watch face is designed in
   [Inkscape](https://inkscape.org/) and converted to raster format at
   compile-time.  Attempting to draw lines, decorations and whatnot
   using C++ was ... ahem... simply not suitable. And slow.  Designing
   this in Inkscape is much nicer.

 * The watch hands are also designed using Inkscape, and drawn as
   lines by the code. It makes it very easy to tweak their design.
 
 * The watch hands move precisely as they would on a real mechanical
   analogue watch (but only once per minute). A little fun with
   trigonometry was needed.

# Building It #

To build this, you will need a few things installed:

 * And operating system. I use Debian GNU/Linux, but any decent Linux
   distribution should work just as well.  For MacOS/Windows: Your
   mileage will vary, and might not be much at all.
 
 * Assuming you want to actually run it: A compatible watch.
 
 * The Arduino IDE. I used 1.8.19 (as packaged in Debian 12/bookworm),
   but other versions will probably work too.
   
 * The Aduino IDE board files for the watch. See
   <https://watchy.sqfmi.com/docs/legacy#arduino-setup> for the gory
   details.
 
 * GNU Make. That's how I build things. It is simple, and it
   works. All the compile-time graphics manipulation is done through
   the `Makefile`.  You can use the Arduino IDE to compile/upload the
   code, but you _need to_ use e.g. `make headers` first. Or you can
   let the `Makefile` do the whole thing with just `make upload` -
   this makes it easy to use an external editor.

 * Inkscape: Used at build time to get a decent SVG -> raster
   conversion. Also nice if you want to edit the designs. I used
   v1.2.2, but other versions are very likely to work out of the box
   as well.
   
 * imagemagick: The compilation process uses it to convert raster
   images to various formats.
   
 * python: We have a custom python script (`svg2multiline.py`) to
   convert the watch hand SVGs into line drawing coordinates.

 * Patience. The build process is slow. Coffee is optional (but
   desirable as always).
   
# Just So You Know ... #

This is my first foray into the world of Arduino and other embedded
systems: so please bear with me if I'm not following best practices
etc. I don't know what they are yet.

It is also my first attempt at designing a watch face for Watchy;
although I'm pretty pleased with the result, I'm sure there are
graphics artists out there who would rip my design to shreds. That's
OK. I'm still learning.
