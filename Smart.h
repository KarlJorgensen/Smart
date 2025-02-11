#ifndef SMART_H
#define SMART_H

#include <Watchy.h>

class Smart : public Watchy{
  using Watchy::Watchy;
public:
  void drawWatchFace();
  void drawTime();
  void drawHourMarkers();
  void drawHourMarker(int hour, int pixels);
};

#endif
