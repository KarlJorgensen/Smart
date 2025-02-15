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

#include "Smart.h"
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

int normaliseAngle(float angle) {
  while (angle < 0)
    angle += 360;
  while (angle >= 360)
    angle -= 360;
  return angle;
}

/* Convert a minute into an angle.

   0° is at 3 o'clock. Return value is normalised to [0..359]
 */
float minute2angle(int minute) {
  return normaliseAngle(6 * ((int)minute - 15));
}

/* Convert an hour into an angle.

   0° is at 3 o'clock. Return value is normalised to [0..359]
 */
float hour2angle(int hour) {
  return normaliseAngle(30 * ( (hour-3) % 12));
}

#include "watchface-pixmap.h"

void Smart::drawWatchFace(){
#if SCREENSHOT_MODE
  currentTime.Hour = SCREENSHOT_HOUR;
  currentTime.Minute = SCREENSHOT_MINUTE;
#endif
#if SERIAL_DEBUG
  char debug_time[30];
  sprintf(debug_time, "\nTime: %02d:%02d", currentTime.Hour, currentTime.Minute);
  Serial.println(debug_time);
#endif
  display.fillScreen(BACKGROUND);

  display.drawXBitmap((display.width() - watchface_width)/2,
		      (display.height() - watchface_height)/2,
		      watchface_bits,
		      watchface_width,
		      watchface_height,
		      FOREGROUND);
  drawHourHand();
  drawMinuteHand();

#if SERIAL_DEBUG
  Serial.print("Hour hand angle: "); Serial.println(hourHandAngle);
  Serial.print("Minute hand angle: "); Serial.println(minuteHandAngle);
#endif

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

  const Box *dowBox = NULL;
  const Box *dateBox = NULL;
  usableBoxes(&dowBox, &dateBox);

  if (dowBox) {
    drawDayOfWeek(dowBox);
  }
  if (dateBox) {
    drawDate(dateBox);
  }
}

/*
  About MultiLines

  MultiLines are a sequence of points; defined in a coordinate system
  where (0,0) is at the centre of the clock face, and given in units of
  pixels.

  A multiline array is an array of multline_t's. The array length MUST
  be divisible by 2; each element is interpreted as alternating x's and
  y's for points.

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
#define DEBUG_DRAWMULTILINE 0
void Smart::drawMultiLine(const multiline_t *line , uint numPoints, float angle) {
  float radians = PI * angle / 180;
  float cosval = cos(radians);
  float sinval = sin(radians);

#define ROTX round( FACE_CENTER_X + ((cosval * line[0]) - (sinval * line[1])) )
#define ROTY round( FACE_CENTER_Y + ((sinval * line[0]) + (cosval * line[1])) )

  int last_x = ROTX;
  int last_y = ROTY;

#if SERIAL_DEBUG && DEBUG_DRAWMULTILINE
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

#if SERIAL_DEBUG && DEBUG_DRAWMULTILINE
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
		sizeof(hour_hand)/sizeof(multiline_t)/2,
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
		sizeof(minute_hand)/sizeof(multiline_t)/2,
		minuteHandAngle);
}

// Boxes where we can place text
//
// Described in decreasing order of preference
//
// To get the coordinates (and a visual guide), make the "Zone1" though
// "Zone6" layers visible in watchface-pixmap.svg (don't forget to make
// them invisible again before saving - otherwise they will appear on
// the watch face!)
const Box boxes[] =
  {
    {"top box, centered", {65, 40}, {135, 65}, {minute2angle(52), minute2angle(8)}},
    {"bottom box, centered", {65, 135}, {135, 160}, {minute2angle(22), minute2angle(38)}},

    {"upper left box", {25, 65}, {95, 90}, {minute2angle(45), minute2angle(0)}},
    {"upper right box", {105, 65}, {175, 90}, {minute2angle(0), minute2angle(15)}},

    {"lower left box", {25, 110}, {95, 135}, {minute2angle(30), minute2angle(45)}},
    {"lower right box", {105, 110}, {175, 135}, {minute2angle(15), minute2angle(30)}},
  };

#define num_boxes (sizeof(boxes)/(sizeof(Box)))

/*
  Find two boxes we can place text in.

  This finds two boxes which are NOT covered by the watch hands.
 */
#define DEBUG_USABLEBOXES 0
void Smart::usableBoxes(const Box **box1, const Box **box2) {
  const Box *thebox;
  int boxno;

  *box1 = NULL;
  *box2 = NULL;
  for (boxno=0, thebox=boxes;
       boxno < num_boxes;
       thebox++, boxno++) {
    int usable = 1; // assume usable until we find out otherwise
#if SERIAL_DEBUG && DEBUG_USABLEBOXES
    Serial.print("Examining box: "); Serial.println(thebox->name);
#endif
    if (thebox->exclude_angles.min > thebox->exclude_angles.max) {
      // covers the zero angle
#if SERIAL_DEBUG && DEBUG_USABLEBOXES
      Serial.println("Covers the zero angle.");
#endif
      if (hourHandAngle >= thebox->exclude_angles.min || hourHandAngle <= thebox->exclude_angles.max)
	usable = 0;
      if (minuteHandAngle >= thebox->exclude_angles.min || minuteHandAngle <= thebox->exclude_angles.max)
	usable = 0;
    } else {
      if (hourHandAngle >= thebox->exclude_angles.min && hourHandAngle <= thebox->exclude_angles.max)
	usable = 0;
      if (minuteHandAngle >= thebox->exclude_angles.min && minuteHandAngle <= thebox->exclude_angles.max)
	usable = 0;
    }

    if (usable) {
#if SERIAL_DEBUG && DEBUG_USABLEBOXES
      Serial.println("Box is usable");
#endif
      if (*box1 == NULL) {
	*box1 = thebox;
      } else {
	*box2 = thebox;
	return; // as soon as we find 2 free boxes, we're done!
      }
    }
  }
}

const char *weekDays[] = {
  "Sun",
  "Mon",
  "Tue",
  "Wed",
  "Thu",
  "Fri",
  "Sat",
};

void Smart::drawDayOfWeek(const Box *box) {
  drawText(weekDays[currentTime.Wday-1], &FreeSansBold12pt7b, box);
}

const char *months[] = {
  "Jan",
  "Feb",
  "Mar",
  "Apr",
  "May",
  "Jun",
  "Jul",
  "Aug",
  "Sep",
  "Oct",
  "Nov",
  "Dec"
};

void Smart::drawDate(const Box *box) {
  char text[10];
  sprintf(text, "%d %s", currentTime.Day, months[currentTime.Month-1]);
  drawText(text, &FreeSansBold9pt7b, box);
}

void Smart::drawText(const char *text, const GFXfont *font, const Box *box) {
  int16_t xOffset, yOffset;
  uint16_t textWidth, textHeight;

  display.setFont(font);
  display.getTextBounds(text, 0, 0, &xOffset, &yOffset, &textWidth, &textHeight);

  // Center the text inside the box
  int boxWidth = box->botright.x - box->topleft.x;
  int boxHeight = box->botright.y - box->topleft.y;

  int x = box->topleft.x + (boxWidth-textWidth)/2 - xOffset;
  int y = box->topleft.y + (boxHeight-textHeight)/2 - yOffset;

#if SERIAL_DEBUG && 0
  // Give a visual indication of the box
  display.drawRect(box->topleft.x, box->topleft.y,
		   boxWidth, boxHeight,
		   FOREGROUND);
  Serial.print("Text: "); Serial.println(text);
  Serial.print("xOffset: "); Serial.print(xOffset); Serial.print(" yOffset:"); Serial.println(yOffset);
  Serial.print("Text width: "); Serial.println(textWidth);
  Serial.print("Text height: "); Serial.println(textHeight);
  Serial.print("Box name: "); Serial.println(box->name);
  Serial.print("Pos: "); Serial.print(x); Serial.print(", "); Serial.println(y);
#endif

  display.setCursor(x, y);
  display.print(text);
}
