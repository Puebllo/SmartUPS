#ifndef VARIABLES_H
#define VARIABLES_H

#include <Arduino.h>

// IO
#define SECOND_DEFINITION 1000

extern int adcVal;
extern float espVolt;
extern long counter;

extern int screenTick;
extern int screenSelect;

extern String ap_ssid;
extern String ap_password;

extern String ap_ssid_2;
extern String ap_password_2;

extern String webLogin;
extern String webPassword;

extern boolean factoryReset;

extern boolean wifiConnected;

extern int serversCount;

typedef struct {
    String serverName;
    String ipAddress;
    String macAddress;
    boolean isOnline;
} managedServer;

extern managedServer managedServers[5];

extern float lowBatterySoC;
extern unsigned long sessionTime;

extern boolean measureVoltage;
extern int samplesCount;
extern boolean rebootFlag;
extern int configUploadStatusCode;
#endif