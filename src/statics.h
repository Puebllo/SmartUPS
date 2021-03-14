#ifndef STATICS_H
#define STATICS_H

#define EEPROM_SIZE 1024

extern char ip[15]; 
extern int webPort;
extern int mqttPort;

extern uint8_t PHASE_PIN;
extern uint8_t VD_EN;


//AGM Gel SoC characteristic
extern float battPercArray[11];
extern float socValueArray[11];
extern int MEASURE_COUNT;

extern float pwrVolt;
extern float precFactor;
extern float vdPrecFactor;

extern char*  STATUS_ONLINE;
extern char* STATUS_ONBATTERY;
extern char*  STATUS_LOWBATT;

extern char* CURRENT_STATUS;
extern char* APP_VERSION;
extern char timestring[25]; 

extern float stateOfCharge;
extern float upsBatteryVoltage;

typedef struct {
    String ipAddress;
    unsigned long loginTime;
} webSession;

extern unsigned long SESSION_EXPIRE_TIME;
extern int sessionNr;

extern webSession sessions[3];

extern uint8_t SCREEN_WIDTH; 
extern uint8_t SCREEN_HEIGHT;
#endif