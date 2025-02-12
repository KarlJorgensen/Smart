#include "Smart.h"
#include "settings.h"

Smart watchy(settings);

void setup(){
  Serial.begin(9600);
  watchy.init();
}

void loop(){}

// Local Variables:
// mode: c++
// End:
