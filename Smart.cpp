#include  "Smart.h"

#include <Fonts/FreeSans24pt7b.h>

#define DARKMODE false
#define SERIAL_DEBUG 1

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

// The hour markers are arraged in a circle with this radius marking the
// "outermost" part of the markers.
//
// The markers are circles, whose "outer edge" (furthest from the centre
// of the watch face) will "touch" this (imaginary) circle
#define MARKER_OUTER_RADIUS (FACE_RADIUS - 2)

// 3-hour markers & 5-minute markers are filled circles with this radius
#define MARKER_MAJOR_HOUR_RADIUS 7
#define MARKER_MINOR_HOUR_RADIUS 4
// 1-minute markers are lines of this length
#define MARKER_MINUTE_LENGTH (2 * MARKER_MAJOR_HOUR_RADIUS)

// The centre of the watch face is a filled circle.
// Set to zero to disable
#define CENTRE_COVER_RADIUS MARKER_MAJOR_HOUR_RADIUS

// The hands will stay within a circle of this radius
#define HANDS_RADIUS (MARKER_OUTER_RADIUS - MARKER_MAJOR_HOUR_RADIUS)

// Whether we want thick lines
#define THICK_LINES true

int minute2angle(int minute) {
  return 6 * (minute - 15);
}

int hour2angle(uint hour) {
  return 30 * ( (hour-3) % 12);
}

void Smart::drawWatchFace(){
#if SERIAL_DEBUG
  char debug_time[10];
  sprintf(debug_time, "%02d:%02d", currentTime.Hour, currentTime.Minute);
  Serial.println(debug_time);
#endif
  display.fillScreen(BACKGROUND);
  display.setTextColor(FOREGROUND);

  // drawTime();

  // A thick circle for the outside border of the face
  // display.drawCircle(FACE_CENTER_X, FACE_CENTER_Y, FACE_RADIUS+1, FOREGROUND);
  display.drawCircle(FACE_CENTER_X, FACE_CENTER_Y, FACE_RADIUS, FOREGROUND);
  display.drawCircle(FACE_CENTER_X, FACE_CENTER_Y, FACE_RADIUS-1, FOREGROUND);

  display.drawCircle(FACE_CENTER_X, FACE_CENTER_Y, FACE_RADIUS - MARKER_MINUTE_LENGTH, FOREGROUND);

  drawOuterMarks();
  drawHourHand();
  drawMinuteHand();

#if CENTRE_COVER_RADIUS
  // Cover the centre of the watch face
  display.fillCircle(FACE_CENTER_X,
		     FACE_CENTER_Y,
		     CENTRE_COVER_RADIUS,
		     FOREGROUND);
#endif
}

void Smart::drawOuterMarks(){
  for (int minute=0;
       minute < 60;
       minute += (MARKER_MINUTE_LENGTH ? 1 : 15)
       ) {
    if ((minute % 15) == 0) {
      drawOuterMark(minute2angle(minute), MARKER_MAJOR_HOUR_RADIUS);
    } else if ((minute % 5) == 0) {
      drawOuterMark(minute2angle(minute), MARKER_MINOR_HOUR_RADIUS);
    }
#if MARKER_MINUTE_LENGTH
    else {
      float radians = PI * minute2angle(minute) / 180;
      float cosval = cos(radians), sinval = sin(radians);
      display.drawLine(round(FACE_CENTER_X + cosval * (MARKER_OUTER_RADIUS - MARKER_MINUTE_LENGTH)),
		       round(FACE_CENTER_Y + sinval * (MARKER_OUTER_RADIUS - MARKER_MINUTE_LENGTH)),
		       round(FACE_CENTER_X + cosval * (MARKER_OUTER_RADIUS)),
		       round(FACE_CENTER_X + sinval * (MARKER_OUTER_RADIUS)),
		       FOREGROUND);
    }
#endif
  }
}

// angle: 0° is 3'oclock, with positive values going clockwise.
void Smart::drawOuterMark(int angle, int pixels){
  float radians = PI * angle / 180;
  float cosval = cos(radians);
  float sinval = sin(radians);

  display.fillCircle(round(FACE_CENTER_X + (MARKER_OUTER_RADIUS - pixels) * cosval),
		     round(FACE_CENTER_Y + (MARKER_OUTER_RADIUS - pixels) * sinval),
		     pixels,
		     FOREGROUND);
}

void Smart::drawTime(){
  display.setCursor(50, 100);
  display.setFont(&FreeSans24pt7b);

  char time[10];
  sprintf(time, "%02d:%02d", currentTime.Hour, currentTime.Minute);
  display.println(time);

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

#include "hour_hand.h"

void Smart::drawHourHand() {
  float angle = hour2angle(currentTime.Hour) + currentTime.Minute/2.0;

#if SERIAL_DEBUG && 0
  Serial.print("Hour hand angle: ");
  Serial.println(angle);
#endif

  drawMultiLine(hour_hand,
		sizeof(hour_hand)/sizeof(float)/2,
		angle);
}

#include "minute_hand.h"

void Smart::drawMinuteHand() {
  float angle = minute2angle(currentTime.Minute);

#if SERIAL_DEBUG && 1
  Serial.print("Minute hand angle: ");
  Serial.println(angle);
#endif

  drawMultiLine(minute_hand,
		sizeof(minute_hand)/sizeof(float)/2,
		angle);
}
