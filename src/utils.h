#ifndef UTILS_H
#define UTILS_H

#include "Arduino.h"
#include "statics.h"
#include "variables.h"
#include <ESP8266WiFi.h>

String getUptime();
String getPrecCalibData();
String getCalibData();
String getInitData();
String processor(const String &var);
void checkUPSStatus();
void doFactoryReset();
String getConfigData();


#endif