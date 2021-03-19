#ifndef UTILS_H
#define UTILS_H

#include "Arduino.h"
#include "statics.h"
#include "variables.h"
#include <ESP8266WiFi.h>
#include "eepromUtils.h"
#include <WiFiUdp.h>

String getUptime();
String getPrecCalibData();
String getCalibData();
String getInitData();
String processor(const String &var);
void checkUPSStatus();
void doFactoryReset();
String getConfigData();

void addServer(managedServer newServer);
void removeServer(String serverName);
void turnOn(String serverName);

String getServersListJson();

managedServer getServerByName(String serverName);
void hexCharacterStringToBytes(byte *byteArray, const char *hexString);

#endif