#include "variables.h"

int adcVal = 0;
float espVolt = 0.0;
long counter = 0;

int screenTick = 0;
int screenSelect = 0;

String ap_ssid = "";
String ap_password = "";

String ap_ssid_2 = "";
String ap_password_2 = "";

String webLogin = "";
String webPassword = "";

boolean factoryReset = false;

boolean wifiConnected = false;

int serversCount = 0;

managedServer managedServers[5];