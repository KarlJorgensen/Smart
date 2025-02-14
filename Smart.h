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

#include <Watchy.h>

// This must be a signed type as coordinates may be negative
#define multiline_t int8_t

class Smart : public Watchy{
  using Watchy::Watchy;
public:
  void drawWatchFace();
protected:
  void drawMultiLine(const multiline_t *line , uint numPoints, float angle);
  void drawHourHand();
  void drawMinuteHand();

  float hourHandAngle;
  float minuteHandAngle;
};

#endif
