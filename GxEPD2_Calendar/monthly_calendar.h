//https://github.com/adafruit/Adafruit_Learning_System_Guides/tree/main/Airlift_ePaper_Calendar
#include "RTClib.h"
#define ENABLE_GxEPD2_GFX 0
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_4C.h>
#include <GxEPD2_7C.h>
// #include <WiFiNINA.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSerifBold12pt7b.h>
#define BUSY_Pin 0
#define RST_Pin 3
#define DC_Pin 2
#define CS_Pin 1
// Define SPI pins
#define SPI_MISO -1
#define SPI_MOSI 6
#define SPI_SCK 4
#define SPI_CS -1
// can use only half buffer size
GxEPD2_3C<GxEPD2_420c, GxEPD2_420c::HEIGHT / 2> gfx(GxEPD2_420c(/*CS=15*/ CS_Pin, /*DC=4*/ DC_Pin, /*RST=2*/ RST_Pin, /*BUSY=5*/ BUSY_Pin));  // GDEW042Z15 400x300, UC8176 (IL0398)
//GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT / 2> display(GxEPD2_420c_Z21(/*CS=15*/ SS, /*DC=4*/ 4, /*RST=2*/ 2, /*BUSY=5*/ 5)); // GDEQ042Z21 400x300, UC8276
//GxEPD2_3C<GxEPD2_420c_GDEY042Z98, GxEPD2_420c_GDEY042Z98::HEIGHT> display(GxEPD2_420c_GDEY042Z98(/*CS=15*/ SS, /*DC=4*/ 4, /*RST=2*/ 2, /*BUSY=5*/ 5)); // GDEY042Z98 400x300, SSD1683 (no inking)
enum dayhighlight { RedCircle,
                    BlackCircle,
                    Bold,
                    None };
// pick one of these options for displaying the current day in the current month
// const char *wifi_ssid = "iPG";
// const char *wifi_password = "0888484822";
// const char *aio_username = "AIO_USERNAME";
// const char *aio_key = "AIO_KEY";
// // WiFiSSLClient client;
dayhighlight currentday = RedCircle;
//dayhighlight currentday = BlackCircle;
//dayhighlight currentday = Bold;
//dayhighlight currentday = None;
struct tm *today;
struct tm *pickdate = new struct tm;
bool isLeapYear(int year) {
  if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0))
    return true;
  return false;
}

int getDaysInMonth(int month, int year) {
  int daysInMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  if (month != 2)
    return daysInMonth[(month - 1) % 12];
  if (isLeapYear(year))
    return 29;
  return 28;
}

int getDayOfWeek(int year, int month, int day) {
  uint16_t months[] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
  };  // days until 1st of month

  uint32_t days = year * 365;  // days until year
  for (uint16_t i = 4; i < year; i += 4)
    if (isLeapYear(i)) days++;  // adjust leap years, test only multiple of 4 of course

  days += months[month - 1] + day;              // add the days of this year
  if ((month > 2) && isLeapYear(year)) days++;  // adjust 1 if this year is a leap year, but only after febr

  // make Sunday 0
  days--;
  if (days < 0)
    days += 7;
  return days % 7;  // remove all multiples of 7
}

void drawCalendar(struct tm *today, struct tm *pickdate) {
  char *dows[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
  char *months[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

  pickdate->tm_wday = getDayOfWeek(pickdate->tm_year, pickdate->tm_mon, pickdate->tm_mday);

  Serial.println("drawing calendar for " + String(months[pickdate->tm_mon - 1]) + " " + String(pickdate->tm_year));

  // draw calendar
  String stryear = String(pickdate->tm_year);
  gfx.setCursor(8, 14);
  gfx.print(stryear);

  String strmonth = months[pickdate->tm_mon - 1];

  int daysinmonth = getDaysInMonth(pickdate->tm_mon, pickdate->tm_year);
  gfx.setTextColor(GxEPD_BLACK);
  gfx.setFont(&FreeSansBold9pt7b);
  int16_t fx, fy;
  uint16_t w, h;
  gfx.getTextBounds((char *)strmonth.c_str(), 0, 0, &fx, &fy, &w, &h);

  gfx.setCursor((gfx.width() - w) / 2, 14);
  gfx.print(strmonth);

  int curday = pickdate->tm_mday - pickdate->tm_wday;
  while (curday > 1)
    curday -= 7;

  int x = 0;
  int y = 40;
  for (int i = 0; i < 7; i++) {
    x = 4 + i * (gfx.width() - 8) / 7;
    gfx.setTextColor(GxEPD_BLACK);
    gfx.setFont();
    gfx.getTextBounds(String(dows[i]).substring(0, 3), 0, 0, &fx, &fy, &w, &h);
    gfx.setCursor(x + (gfx.width() / 7 - w) / 2, y - 8);
    gfx.print(String(dows[i]).substring(0, 3));
  }
  gfx.drawLine(0, 27, gfx.width(), 27, GxEPD_BLACK);
  y = 45;
  while (curday <= daysinmonth) {
    for (int i = 0; i < 7; i++) {
      x = 4 + i * (gfx.width() - 8) / 7;
      if (curday >= 1 && curday <= daysinmonth) {
        gfx.setCursor(x, y);
        gfx.setTextColor(GxEPD_BLACK);
        gfx.setFont(&FreeSans9pt7b);
        int16_t fx, fy;
        uint16_t w, h;
        String strday = String(curday);
        if ((today->tm_year == pickdate->tm_year) && (today->tm_mon == pickdate->tm_mon) && (curday == pickdate->tm_mday)) {
          if (currentday != None) {
            gfx.setFont(&FreeSansBold9pt7b);
          }
          if (currentday == BlackCircle) {
            //gfx.setTextColor(EPD_INVERSE);
            gfx.fillCircle(x + (gfx.width() - 8) / 7 / 2, y, 16, GxEPD_BLACK);
          } else if (currentday == RedCircle) {
            gfx.setTextColor(GxEPD_BLACK);
            gfx.fillCircle(x + (gfx.width() - 8) / 7 / 2, y, 16, GxEPD_BLACK);
          }
        }
        gfx.getTextBounds(strday.c_str(), 0, 0, &fx, &fy, &w, &h);
        gfx.setCursor(x + (gfx.width() - 8) / 7 / 2 - w / 2 - fx, y + h / 2);
        //gfx.setColorBuffer(1, true);  // red is inverted
        gfx.print(curday);
        //gfx.setColorBuffer(1, false);  // red is not inverted
      }
      curday++;
    }
    y += 24;
  }
  Serial.println("display update completed");
}

#define SERVER "io.adafruit.com"
#define PATH "/api/v2/%s/integrations/time/strftime?x-aio-key=%s"
// our strftime is %Y-%m-%d %H:%M:%S.%L %j %u %z %Z see http://strftime.net/ for decoding details
// See https://apidock.com/ruby/DateTime/strftime for full options
#define TIME_SERVICE_STRFTIME "&fmt=%25Y-%25m-%25d+%25H%3A%25M%3A%25S.%25L+%25j+%25u+%25z+%25Z"
void *wget(const char *host, const char *path, int port, char *buff) {
  // if (client.connect(host, port)) {
  //   Serial.println("connected to server");
  //   // Make a HTTP request:
  //   // Using HTTP/1.1 to avoid "Transfer-Encoding: chunked" reply
  //   client.println(String("GET ") + path + String(" HTTP/1.0"));
  //   client.println("Host: " + String(host));
  //   client.println("Connection: close");
  //   client.println();

  //   uint32_t bytes = 0;
  //   int capturepos = 0;
  //   bool capture = false;
  //   int linelength = 0;
  //   char lastc = '\0';
  //   while (true) {
  //     while (client.available()) {
  //       char c = client.read();
  //       if ((c == '\n') && (lastc == '\r')) {
  //         if (linelength == 0) {
  //           capture = true;
  //         }
  //         linelength = 0;
  //       } else if (capture) {
  //         buff[capturepos++] = c;
  //         //Serial.write(c);
  //       } else {
  //         if ((c != '\n') && (c != '\r'))
  //           linelength++;
  //       }
  //       lastc = c;
  //       bytes++;
  //     }

  //     // if the server's disconnected, stop the client:
  //     if (!client.connected()) {
  //       //Serial.println();
  //       Serial.println("disconnecting from server.");
  //       client.stop();
  //       buff[capturepos] = '\0';
  //       Serial.println("read " + String(bytes) + " bytes");
  //       Serial.println("captured " + String(capturepos) + " bytes");
  //       break;
  //     }
  //   }
  // }
}
struct tm *getDate(bool force = false) {
  static tm date;
  date.tm_year = 2026;
  date.tm_mon = 06;
  date.tm_mday = 06;
  date.tm_wday = 2;
  date.tm_hour = 1;
  date.tm_min = 1;
  date.tm_sec = 1;
  return &date;
  // char buff[500];
  // char pathbuff[500];
  // sprintf(pathbuff, PATH, aio_username, aio_key);
  // String path = String(pathbuff) + String(TIME_SERVICE_STRFTIME);
  //Serial.println(String("path to check: " ) + String(SERVER) + path);
  // wget(SERVER, path.c_str(), 443, buff);
  // Serial.println("wget got: " + String(buff));
  // String datestr = String(buff);
  // date.tm_year = atoi(datestr.substring(0, 4).c_str());
  // date.tm_mon = atoi(datestr.substring(5, 7).c_str());
  // date.tm_mday = atoi(datestr.substring(8, 10).c_str());
  // date.tm_wday = atoi(datestr.substring(28, 29).c_str());
  // date.tm_hour = atoi(datestr.substring(11, 13).c_str());
  // date.tm_min = atoi(datestr.substring(14, 16).c_str());
  // date.tm_sec = atoi(datestr.substring(17, 19).c_str());
  // return &date;
}

void showCalendar() {
  gfx.setFont(&FreeSansBold9pt7b);
  gfx.setTextColor(GxEPD_BLACK);
  gfx.setFullWindow();
  today = getDate();
  Serial.println("today is " + String(today->tm_mon) + "/" + String(today->tm_mday) + "/" + String(today->tm_year));
  pickdate->tm_year = today->tm_year;
  pickdate->tm_mon = today->tm_mon;
  pickdate->tm_mday = today->tm_mday;
  pickdate->tm_hour = today->tm_hour;
  pickdate->tm_min = today->tm_min;
  pickdate->tm_sec = today->tm_sec;
  gfx.firstPage();
  do {
    gfx.fillScreen(GxEPD_WHITE);
    drawCalendar(today, pickdate);
  } while (gfx.nextPage());
  delay(2000);
}
