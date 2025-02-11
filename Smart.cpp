#include  "Smart.h"

#include <Fonts/FreeSans24pt7b.h>

#define DARKMODE false

#define BACKGROUND (DARKMODE ? GxEPD_BLACK : GxEPD_WHITE)
#define FOREGROUND (DARKMODE ? GxEPD_WHITE : GxEPD_BLACK)

// Placement of the analogue watch face
#define FACE_CENTER_X 100
#define FACE_CENTER_Y 100

// This is the _outer_ radius.
#define FACE_RADIUS 99

// The hour markers are arraged in a circle with this radius marking the
// "outermost" part of the markers.
#define MARKER_OUTER_RADIUS (FACE_RADIUS - 4)

// Individual hour markers are filled circles with this radius
#define MAJOR_HOUR_RADIUS 6
#define MINOR_HOUR_RADIUS 4

void Smart::drawWatchFace(){
  display.fillScreen(BACKGROUND);
  display.setTextColor(FOREGROUND);

  drawTime();

  // Make a thick circle
  display.drawCircle(FACE_CENTER_X, FACE_CENTER_Y, FACE_RADIUS, FOREGROUND);
  // display.drawCircle(FACE_CENTER_X, FACE_CENTER_Y, FACE_RADIUS-1, FOREGROUND);
  display.drawCircle(FACE_CENTER_X, FACE_CENTER_Y, FACE_RADIUS-2, FOREGROUND);
  drawHourMarkers();
}

void Smart::drawHourMarkers(){
  drawHourMarker(-60, MINOR_HOUR_RADIUS);
  drawHourMarker(-30, MINOR_HOUR_RADIUS);
  drawHourMarker(0, MAJOR_HOUR_RADIUS); // 3 o'clock
  drawHourMarker(30, MINOR_HOUR_RADIUS);
  drawHourMarker(60, MINOR_HOUR_RADIUS);
  drawHourMarker(90, MAJOR_HOUR_RADIUS);
  drawHourMarker(120, MINOR_HOUR_RADIUS);
  drawHourMarker(150, MINOR_HOUR_RADIUS);
  drawHourMarker(180, MAJOR_HOUR_RADIUS);
  drawHourMarker(210, MINOR_HOUR_RADIUS);
  drawHourMarker(240, MINOR_HOUR_RADIUS);
  drawHourMarker(270, MAJOR_HOUR_RADIUS);
}

// angle: 0° is 3'oclock, with positive values going clockwise.
void Smart::drawHourMarker(int angle, int pixels){
  float radians = PI * angle / 180;
  float cosval = cos(radians);
  float sinval = sin(radians);

  display.fillCircle(FACE_CENTER_X + (uint16_t)((MARKER_OUTER_RADIUS - pixels) * cosval),
		     FACE_CENTER_Y + (uint16_t)((MARKER_OUTER_RADIUS - pixels) * sinval),
		     pixels,
		     FOREGROUND);
  // int inner = MARKER_OUTER_RADIUS - pixels;
  // display.drawLine(FACE_CENTER_X + (uint16_t)(MARKER_OUTER_RADIUS * cosval),
  // 		   FACE_CENTER_Y + (uint16_t)(MARKER_OUTER_RADIUS * sinval),
  // 		   FACE_CENTER_X + (uint16_t)(inner * cosval),
  // 		   FACE_CENTER_Y + (uint16_t)(inner * sinval),
  // 		   FOREGROUND);
}

void Smart::drawTime(){
  display.setCursor(50, 100);
  display.setFont(&FreeSans24pt7b);

  char time[10];
  sprintf(time, "%02d:%02d", currentTime.Hour, currentTime.Minute);
  display.println(time);

}

