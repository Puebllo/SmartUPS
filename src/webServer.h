#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h" 
#include "statics.h"
#include "utils.h"
#include "eepromUtils.h"

void startServer();
void validateSessions();
void clearSessions();

#endif