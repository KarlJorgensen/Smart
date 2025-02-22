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

#ifndef SMART_H
#define SMART_H

#include "settings.h"
#include <Watchy.h>

// This must be a signed type as coordinates may be negative
#define multiline_t int8_t

// Describes a box we can put text in, and an "exclusion zone": If any
// watch hand is inside this exclusion zone, the box is considered
// "covered" and thus not usable.
typedef struct {
  const char *name;
  struct {
    int16_t x;
    int16_t y;
  } topleft;
  struct {
    int16_t x;
    int16_t y;
  } botright;
  // The range of angles for the exclusion zone.
  // If min > max, this means that the area includes the 0° angle
  // These angles must be normalised - i.e. in the range of [0..359]
  struct {
    float min;
    float max;
  } exclude_angles;
} Box;

class Smart : public Watchy{
  using Watchy::Watchy;
public:
  void drawWatchFace();
protected:
  void drawMultiLine(const multiline_t *line , uint numPoints, float angle);
  void drawHourHand();
  void drawMinuteHand();
  void drawDayOfWeek(const Box *box);
  void drawDate(const Box *box);
  void drawText(const char *text, const GFXfont *font, const Box *box);
  uint usableBoxes(uint boxes_wanted, const Box **boxes);

  float hourHandAngle;
  float minuteHandAngle;
};

#endif
