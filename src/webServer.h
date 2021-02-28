#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h" 
#include "statics.h"
#include "utils.h"
//#include "variables.h"
#include "eepromUtils.h"
//#include "mqttUtils.h"
//#include "views.cpp"

void startServer();
void validateSessions();
void clearSessions();

#endif