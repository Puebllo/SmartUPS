
#include "screens.h"
#include "utils.h"


void screenSelector(Adafruit_SSD1306 &display){
    switch (screenSelect){
  case 0:
    statusScreen(display);
    break;
  case 1:
    upsScreen(display);
    break;
  case 2:
    webScreen(display);
    break;
  case 3:
      uptimeScreen(display);
    break;
  default:
    break;
  }
}

void welcomeScreen(Adafruit_SSD1306 &display)
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(10, 15);
    display.print("SMART-UPS");
    display.setTextSize(1);
    display.setCursor(8, 40);
    display.print("or at least smarter");
    display.setCursor(80, 55);
    display.print("v.");
    display.print(APP_VERSION);
    display.display();
}

void statusScreen(Adafruit_SSD1306 &display)
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    drawCentreString(display, "Status", 0, 13);
    display.setTextSize(2);
    //display.setCursor(10, 15);
    drawCentreString(display, CURRENT_STATUS, 0, 32);
    display.display();
}

void uptimeScreen(Adafruit_SSD1306 &display)
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    drawCentreString(display, "Uptime", 0, 13);
    display.setTextSize(1);
    //display.setCursor(10, 15);
      char buf[25];
    getUptime().toCharArray(buf, 25);
    drawCentreString(display, buf, 0, 32);
    display.display();
}

void upsScreen(Adafruit_SSD1306 &display)
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);

    String temp = String(stateOfCharge) + String(" %");
    char buf[10];
    temp.toCharArray(buf, 10);
        drawCentreString(display, "Battery:", 5, 0);
    display.setTextSize(2);
    drawCentreString(display, buf, 0, 20);

    float f = upsBatteryVoltage / 1000.0;
    temp = String(f) + String(" V");
    temp.toCharArray(buf, 10);
    display.setTextSize(2);

    drawCentreString(display, buf, 0, 45);

    display.display();
}

void webScreen(Adafruit_SSD1306 &display){
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);

     //char buf[20];
    // String temp = ip + String(":") +String(webPort);
    // temp.toCharArray(buf, 20);

   // strcpy(buf,ip);
    //strcat(buf,":");
      //strcat(buf,(char*)webPort);

//Serial.println(ip);
    drawCentreString(display, "WEB Server", 0, 13);
        display.setCursor(0, 25);

     //  display.print(ip); display.print(":"); display.print(webPort);
        char webData[21];// = String(ip) + String(":") + String(webPort);
        char port[5];
        String(webPort).toCharArray(port,5);

        strcpy(webData,ip);
        strcat(webData,":");
        strcat(webData,port);

    drawCentreString(display, webData, 0, 25);

    display.display();
}

void drawCentreString(Adafruit_SSD1306 &display, const char *buf, int x, int y)
{
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(buf, x, y, &x1, &y1, &w, &h); //calc width of new string
    x = (128 - w) / 2;
    display.setCursor(x, y);
    display.print(buf);
}