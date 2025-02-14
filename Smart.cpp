/*
This file is part of Smart.

Smart is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your
option) any later version.

Smart is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along
with Smart. If not, see <https://www.gnu.org/licenses/>.
 */

#include  "Smart.h"

#include <Fonts/FreeSans24pt7b.h>

#define DARKMODE true
#define SERIAL_DEBUG 0

#define BACKGROUND (DARKMODE ? GxEPD_BLACK : GxEPD_WHITE)
#define FOREGROUND (DARKMODE ? GxEPD_WHITE : GxEPD_BLACK)

// Placement of the analogue watch face.
// In general, you want to place this in the centre of the screen
#define FACE_CENTER_X 100
#define FACE_CENTER_Y 100

// Outer radius of the clock face - in pixels
//
// Depending on the size of the screen and placement, this may cause
// parts of the face to not fit on screen.
#define FACE_RADIUS 99

// The centre of the watch face is a filled circle.
// Set to zero to disable
#define CENTRE_COVER_RADIUS 7

// The hands will stay within a circle of this radius
#define HANDS_RADIUS 80

// Whether we want thick lines
#define THICK_LINES false

int minute2angle(int minute) {
  return 6 * ((int)minute - 15);
}

int hour2angle(int hour) {
  return 30 * ( (hour-3) % 12);
}

#include "watchface-pixmap.h"

void Smart::drawWatchFace(){
#if SERIAL_DEBUG
  char debug_time[10];
  sprintf(debug_time, "%02d:%02d", currentTime.Hour, currentTime.Minute);
  Serial.println(debug_time);
#endif
  display.fillScreen(BACKGROUND);
  display.setTextColor(FOREGROUND);

  display.drawXBitmap((display.width() - watchface_width)/2,
		      (display.height() - watchface_height)/2,
		      watchface_bits,
		      watchface_width,
		      watchface_height,
		      FOREGROUND);
  drawHourHand();
  drawMinuteHand();

#if CENTRE_COVER_RADIUS
  // Cover the centre of the watch face
  display.fillCircle(FACE_CENTER_X,
		     FACE_CENTER_Y,
		     CENTRE_COVER_RADIUS,
		     FOREGROUND);
  display.fillCircle(FACE_CENTER_X,
		     FACE_CENTER_Y,
		     CENTRE_COVER_RADIUS-2,
		     BACKGROUND);
#endif
}

/*
  About MultiLines

  MultiLines are a sequence of points; defined in a coordinate system
  where (0,0) is at the centre of the clock face, and given in units of
  pixels.

  A multiline array is an array of int's. The array length MUST be
  divisible by 2; each element is interpreted as alternating x's and y's
  for points.

  Lines will be drawn between the points: from the 1st to the 2nd, from
  the 2nd to the 3rd etc.  NO LINE will be drawn from the last point
  back to the 1st point.

  To draw a polygon, make sure that the first and last points are
  identical.

  The idea is that: At 3 o'clock: We can use the coordinates nearly
  "as-is" - they only need to be translated by FACE_CENTER_X and
  FACE_CENTER_Y. No rotation is necessary.

  The Smart::drawMultiLine() will rotate the points as needed for
  rendering.
 */


/* Draw a multiline, rotated angle° around the centre of the clock face

   angle is in degrees (of a 360° circle), with 0° at the 3 o'clock
   position (no rotation) going clockwise for positive angles.
 */
void Smart::drawMultiLine(const float *line , uint numPoints, float angle) {
  float radians = PI * angle / 180;
  float cosval = cos(radians);
  float sinval = sin(radians);

#define ROTX round( FACE_CENTER_X + ((cosval * line[0]) - (sinval * line[1])) )
#define ROTY round( FACE_CENTER_Y + ((sinval * line[0]) + (cosval * line[1])) )

  int last_x = ROTX;
  int last_y = ROTY;

#if SERIAL_DEBUG && 0
  Serial.print("drawMultiLine: ");
  Serial.print(" - (");
  Serial.print(last_x);
  Serial.print(", ");
  Serial.print(last_y);
  Serial.println(")");
#endif
  line += 2;
  for (uint p=1; p < numPoints; p++) {
    int x = ROTX;
    int y = ROTY;

#if SERIAL_DEBUG && 0
    Serial.print("point ");
    Serial.print(p);
    Serial.print(": (");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.println(")");
#endif

#if THICK_LINES
#define DRAWLINE(x1, y1, x2, y2, c) do { \
      int _x1 = (x1), _y1 = (y1), _x2 = (x2), _y2 = (y2), _c = (c); \
      display.drawLine(_x1, _y1, _x2, _y2, _c); \
      display.drawLine(_x1 - 1, _y1 - 1, _x2 - 1, _y2 - 1, _c); \
      display.drawLine(_x1 - 1, _y1 + 1, _x2 - 1, _y2 + 1, _c); \
      display.drawLine(_x1 + 1, _y1 - 1, _x2 + 1, _y2 - 1, _c); \
      display.drawLine(_x1 + 1, _y1 + 1, _x2 + 1, _y2 + 1, _c); \
    } while (0)
#else
#define DRAWLINE(x1, y1, x2, y2, c) display.drawLine(x1, y1, x2, y2, c)
#endif

    DRAWLINE(last_x-1, last_y-1, x-1, y-1, FOREGROUND);
    last_x = x;
    last_y = y;
    line += 2;
  }
}

#include "hour_hand-lines.h"

void Smart::drawHourHand() {
  hourHandAngle = hour2angle(currentTime.Hour) + currentTime.Minute/2.0;

#if SERIAL_DEBUG && 0
  Serial.print("Hour hand angle: ");
  Serial.println(hourHandAngle);
#endif

  drawMultiLine(hour_hand,
		sizeof(hour_hand)/sizeof(float)/2,
		hourHandAngle);
}

#include "minute_hand-lines.h"

void Smart::drawMinuteHand() {
  minuteHandAngle = minute2angle(currentTime.Minute);

#if SERIAL_DEBUG && 0
  Serial.print("Minute hand angle: ");
  Serial.println(minuteHandAngle);
#endif

  drawMultiLine(minute_hand,
		sizeof(minute_hand)/sizeof(float)/2,
		minuteHandAngle);
}
