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

float lowBatterySoC = 40.0;

unsigned long sessionTime = 300; 

boolean measureVoltage = false;
int samplesCount = 0;

boolean rebootFlag = false;
int configUploadStatusCode = -1;    // [1] - config uploaded and stored ok ; 
                                    // [0] - ready status (waiting for upload action); 
                                    // [-1] - error while commiting config ; 
                                    // [-2] - wrong file (check extension) 

