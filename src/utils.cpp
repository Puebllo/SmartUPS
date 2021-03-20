#include "utils.h"

float voltage = 0.0;

String getUptime() {
    long startTime = millis();

    long ss = int((startTime / (1000)) % 60);
    long mm = int((startTime / (1000 * 60)) % 60);
    long hh = int((startTime / (1000 * 60 * 60)) % 24);
    long dd = int((startTime / (1000 * 60 * 60 * 24)) % 365);

    sprintf(timestring, "%d days %02d:%02d:%02d", dd, hh, mm, ss);

    return timestring;
}

String getPrecCalibData() {
    return String(precFactor, 8) + ";" + String(vdPrecFactor, 8) + ";" +
           String(pwrVolt, 3);
}

String getCalibData() {
    return String(adcVal) + ";" + String(espVolt) + ";" +
           String(upsBatteryVoltage) + ";" + String(stateOfCharge);
}

String getInitData() {
    return String(ip) + ";" + String(WiFi.macAddress()) + ";" + getUptime();
}

String processor(const String &var) { return String(); }

float mapDouble(double x, double in_min, double in_max, double out_min,
                double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double getVoltage() {
    adcVal = analogRead(A0);
    espVolt = ((pwrVolt / 1024) * adcVal * 1000.0) * precFactor;
    float batt = mapDouble(espVolt, 0.0, 3.256, 0.0, 14.0) * vdPrecFactor;

    return batt;
}

float getLinearizationRange(double voltage, int linearSize,
                            float voltageRange[], float valueRange[]) {
    int previous_i = 0;
    for (int i = 1; i <= linearSize; i++) {
        if (voltage < voltageRange[previous_i] && voltage > voltageRange[i]) {
            return mapDouble(voltage, voltageRange[previous_i], voltageRange[i],
                             valueRange[previous_i], valueRange[i]);
        }
        previous_i = i;
    }
    return -1;
}

void checkUPSStatus() {
    // Check if UPS is online
    if (digitalRead(PHASE_PIN) == 1) {
        CURRENT_STATUS = STATUS_ONBATTERY;
    } else {
        CURRENT_STATUS = STATUS_ONLINE;
    }

    // measure battery voltage
    if (measureVoltage) {
        voltage += getVoltage();
        samplesCount++;
    }
    if (samplesCount == MEASURE_COUNT) {
        samplesCount = 0;
        upsBatteryVoltage = voltage / MEASURE_COUNT;
        voltage = 0.0;
    }

    if (upsBatteryVoltage > 12700.0) {
        stateOfCharge = 100;
    } else {
        stateOfCharge =
            getLinearizationRange(upsBatteryVoltage, 11, socValueArray, battPercArray);
    }

    if (stateOfCharge <= lowBatterySoC) {
        CURRENT_STATUS = STATUS_LOWBATT;
    }
}

void doFactoryReset() {}

String getConfigData() {
    long sTime = sessionTime / 1000;
    return webLogin + ";" + ap_ssid + ";" + ap_ssid_2 + ";" + lowBatterySoC +
           ";" + sTime;
}

void addServer(managedServer newServer) {
    managedServers[serversCount] = newServer;
    serversCount++;
    saveToEEPROM();
}

String getServersListJson() {
    DynamicJsonDocument data(512);

    data["servers_count"] = serversCount;                   // servers count
    JsonArray servers = data.createNestedArray("servers");  // servers array
    StaticJsonDocument<128> doc;

    for (int i = 0; i < serversCount; i++) {
        if (managedServers[i].serverName.equals(SERVER_REMOVE_FLAG)) {
            continue;
        }
        doc.clear();
        JsonObject obj = doc.createNestedObject();
        obj["0"] = managedServers[i].serverName;  // server name
        obj["1"] = managedServers[i].ipAddress;   // ip address
        obj["2"] = managedServers[i].macAddress;  // mac address
        obj["3"] = true;  // managedServers[i].isOnline;  // mac address
        servers.add(obj);
    }

    String output;
    serializeJson(data, output);
    return output;
}

void removeServer(String serverName) {
    int serverIndex = -1;
    for (int i = 0; i < serversCount; i++) {
        if (managedServers[i].serverName.equals(serverName)) {
            serverIndex = i;
            managedServers[i].serverName = SERVER_REMOVE_FLAG;
            break;
        }
    }
    if (serverIndex != -1) {
        cleanEEPROM();
        saveToEEPROM();
    }
}

managedServer getServerByName(String serverName) {
    for (int i = 0; i < serversCount; i++) {
        if (managedServers[i].serverName.equals(serverName)) {
            return managedServers[i];
        }
    }
}

byte nibble(char c) {
    if (c >= '0' && c <= '9') return c - '0';

    if (c >= 'a' && c <= 'f') return c - 'a' + 10;

    if (c >= 'A' && c <= 'F') return c - 'A' + 10;

    return 0;  // Not a valid hexadecimal character
}

void hexCharacterStringToBytes(byte *byteArray, const char *hexString) {
    bool oddLength = strlen(hexString) & 1;

    byte currentByte = 0;
    byte byteIndex = 0;

    for (byte charIndex = 0; charIndex < strlen(hexString); charIndex++) {
        bool oddCharIndex = charIndex & 1;

        if (oddLength) {
            // If the length is odd
            if (oddCharIndex) {
                // odd characters go in high nibble
                currentByte = nibble(hexString[charIndex]) << 4;
            } else {
                // Even characters go into low nibble
                currentByte |= nibble(hexString[charIndex]);
                byteArray[byteIndex++] = currentByte;
                currentByte = 0;
            }
        } else {
            // If the length is even
            if (!oddCharIndex) {
                // Odd characters go into the high nibble
                currentByte = nibble(hexString[charIndex]) << 4;
            } else {
                // Odd characters go into low nibble
                currentByte |= nibble(hexString[charIndex]);
                byteArray[byteIndex++] = currentByte;
                currentByte = 0;
            }
        }
    }
}
