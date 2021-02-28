#include <Arduino.h>

// AGM Gel SoC characteristic
float battPercArray[11] = {0.0,  10.0, 20.0, 30.0, 40.0, 50.0,
                           60.0, 70.0, 80.0, 90.0, 100.0};
float socValueArray[11] = {10500.0, 11310.0, 11580.0, 11750.0, 11900.0, 12060.0,
                           12200.0, 12320.0, 12420.0, 12500.0, 12700.0};

int MEASURE_COUNT = 10;
int sessionNr = 0;

float pwrVolt = 3.3;
float precFactor = 1.0;
float vdPrecFactor = 1.0;

uint8_t PHASE_PIN = 13;
uint8_t VD_EN = 16;

char ip[15];  // DO NOT SET THIS VALUE - IT'LL BE ASSIGNED BY ROUTER !
int webPort = 7880;
int mqttPort = 7883;

char* STATUS_ONLINE = "ON-LINE";
char* STATUS_ONBATTERY = "ON-BATT";
char* STATUS_LOWBATT = "LOW BATT";

char* CURRENT_STATUS = "";
char* APP_VERSION = "0.7.0";
char timestring[25];

float stateOfCharge = 0.0;
float upsBatteryVoltage = 0.0;

uint8_t SCREEN_WIDTH = 128;
uint8_t SCREEN_HEIGHT = 64;

unsigned long SESSION_EXPIRE_TIME = 30000;  // 15s

typedef struct {
    String ipAddress;
    unsigned long loginTime;
} webSession;

webSession sessions[3];