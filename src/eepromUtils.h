#ifndef EEPROM_UTILS_H
#define EEPROM_UTILS_H

#include <ArduinoJson.h>
#include <EEPROM.h>
#include "variables.h"
#include "statics.h"
#include "utils.h"

boolean saveToEEPROM();
void loadDataFromEEPROM();
void clearEEPROM();

void saveSystemVoltageCalibration(float sysVoltageToSave);
void saveADCCalibration(float precFactorToSave);
void saveUPSBatteryVoltageCalibration(float vdPrecFactorToSave);
boolean cleanEEPROM();

String getDataFromEEPROM();
boolean writeAndCommitEEPROM(String data);

#endif