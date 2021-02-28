#include "screens.h"
#include "utils.h"

#include <WiFiUdp.h>

#include <SPI.h>
#include <Wire.h>
// LittleFS is declared
#include <ESPAsyncWebServer.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "webServer.h"
#include "eepromUtils.h"

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long screenTickMillis = 1000; //the value is a number of milliseconds between
const int SCREEN_COUNT = 4;

//set screen display time [s]
int screenTime[SCREEN_COUNT] = {5, 5, 2, 2};



boolean waitTillNextTick = false;

long onGridPing = -1;
long lastOnGridPing = -1;
long onGridInterval = 3000; 


#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//AsyncWebServer server(webPort);


//long startTime = 0;

String msg = "";
WiFiUDP UDP;
/**
 * The Magic Packet needs to be sent as BROADCAST in the LAN
 */
IPAddress computer_ip(192, 168, 1, 255);

/**
 * The targets MAC address to send the packet to
 */

//NAS
//byte mac[] = { 0x00, 0x11, 0x32, 0xAA, 0x0F, 0xD9 };

//SPEEDO
byte mac[] = {0x24, 0xB6, 0xFD, 0x3F, 0x44, 0x38};





void sendWOL(IPAddress addr, WiFiUDP udp, byte *mac, size_t size_of_mac){

  byte preamble[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  byte i;

  udp.beginPacket(addr, 9); //sending packet at 9,

  udp.write(preamble, sizeof preamble);

  for (i = 0; i < 16; i++)
  {
    udp.write(mac, size_of_mac);
  }
  udp.endPacket();
}



boolean apStarted = false;
unsigned long now = millis();
unsigned long apModeStartTime = 0;








boolean setupWifiInner(String ssid, String pwd) {
    boolean toReturn = true;
    if (ssid.length() > 0 && pwd.length() > 0) {
        delay(10);
        Serial.println();
        Serial.print("Connecting to ");
        Serial.print(ssid);
        WiFi.setSleepMode(WIFI_NONE_SLEEP);

        WiFi.begin(ssid, pwd);

        int attempt = 1;
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
            display.print(".");
             display.display();
            if (attempt == 20) {
                toReturn = false;
                break;
            }
            attempt++;
        }
        if (WiFi.status() == WL_CONNECTED) {
                           WiFi.localIP().toString().toCharArray(ip, 15);

            Serial.println("");
            Serial.println("WiFi connected");
            Serial.println("IP address: ");
            Serial.println(ip);
    display.setCursor(0, 20);
    display.print("Connected to ");
    display.print(ssid);
    display.setCursor(0, 30);

    display.print(ip);
        } else {
            Serial.print("Connection to: ");
            Serial.print(ssid);
            Serial.println(" failed !");
                display.println("");
    display.println("Connection failed.");
        }
         display.display();
    }
    return toReturn;
}

boolean setupWifi() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(1000);
    boolean isConnected = false;
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);


  display.print("Connecting to 1. AP");
  display.setCursor(0, 10);
 display.display();


    if (ap_ssid.length() > 0 && ap_password.length() > 0) {
        //Try to connect to first AP
        isConnected = setupWifiInner(ap_ssid, ap_password);

        //Try to connect to second AP if configured
        if (!isConnected && ap_ssid_2.length() > 0 && ap_password_2.length() > 0) {
          display.clearDisplay();
            display.print("Connecting to 2. AP");
  display.setCursor(0, 10);
 display.display();
            isConnected = setupWifiInner(ap_ssid_2, ap_password_2);
        }
    }

    //If connection to Wifi failed, then set node in AP mode
    if (!isConnected) {
                display.clearDisplay();

        now = millis();
        apModeStartTime = now;
        if (!apStarted) {
            WiFi.disconnect();
            delay(1000);

            apStarted = true;

            Serial.println("Setting UPS in AP mode");
            IPAddress localIp(192, 168, 0, 1);
            IPAddress gateway(192, 168, 0, 1);
            IPAddress subnet(255, 255, 255, 0);

            WiFi.softAP("UPS", "12345678");
            WiFi.softAPConfig(localIp, gateway, subnet);

            
            display.println("UPS in AP mode");
            display.println("IP: 192.168.0.1");
             display.display();
             delay(5000);
        }
    } else {
        apModeStartTime = 0;
        apStarted = false;
    }

    return isConnected;
}
















/*boolean connectWifi()
{

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);

  display.print("Connecting to WiFi");

  // delay(2000);
  // display.display();
  boolean state = true;
  int i = 0;

  WiFi.begin(ap_password, ap_password_2);
  // Serial.println("");
  // Serial.println("Connecting to WiFi");

  // Wait for connection
  //  Serial.print("Connecting");
  display.setCursor(0, 10);
  //display.print(counter);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    display.print(".");
    display.display();

    if (i > 20)
    {
      state = false;
      break;
    }
    i++;
  }
  if (state)
  {
   
     WiFi.localIP().toString().toCharArray(ip, 15);
    display.setCursor(0, 20);
    display.print("Connected to ");
    display.print(ssid);
    display.setCursor(0, 30);

    display.print(ip);
  }
  else
  {
    display.println("");
    display.println("Connection failed.");
  }
  display.display();
  delay(3000);

  return state;
}*/

void setup(){

      EEPROM.begin(EEPROM_SIZE);
    delay(200);
    loadDataFromEEPROM();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  pinMode(PHASE_PIN,INPUT);
  pinMode(VD_EN, OUTPUT);
  digitalWrite(VD_EN,HIGH);
  display.setTextColor(WHITE);

  display.clearDisplay();
  Serial.begin(115200);

  welcomeScreen(display);

  delay(2000);

  // Initialise Serial connection


clearSessions();

  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Initialise wifi connection
  wifiConnected = setupWifi();
  UDP.begin(9); //start UDP client, not sure if really necessary.

 // if (wifiConnected){
    startServer();
    //Serial.println("Sending WOL Packet...");
    // sendWOL(computer_ip, UDP, mac, sizeof mac);
    //  delay(4000);
  //}
  startMillis = millis(); //initial start time
}



void loop(){
  currentMillis = millis();
  screenSelector(display);

  checkUPSStatus();
  validateSessions();

  if (currentMillis - startMillis >= screenTickMillis){
    screenTick++;
    if (screenSelect == (SCREEN_COUNT)){
      screenSelect = 0;
    }
    int currentScreenTime = screenTime[screenSelect];
    if (screenTick == currentScreenTime){
      screenTick = 0;
      screenSelect++;
    }

    startMillis = currentMillis;
  }
}
