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

typedef struct {
    String serverName;
    String ipAddress;
    String macAddress;
} managedServer;

#endif