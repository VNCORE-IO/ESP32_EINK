
#define ENABLE_GxEPD2_GFX 0
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_4C.h>
#include <GxEPD2_7C.h>
 #include "monthly_calendar.h" 
void setup() {
  Serial.begin(115200);
  Serial.println("setup");
  gfx.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse\
  delay(2000);
  showCalendar();
  Serial.println("setup done");
  gfx.end();
}

void loop() {
}
