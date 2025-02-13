#ifndef SMART_H
#define SMART_H

#include <Watchy.h>

class Smart : public Watchy{
  using Watchy::Watchy;
public:
  void drawWatchFace();
protected:
  void drawTime();
  void drawMultiLine(const float *line , uint numPoints, float angle);
  void drawHourHand();
  void drawMinuteHand();

  float hourHandAngle;
  float minuteHandAngle;
};

#endif
