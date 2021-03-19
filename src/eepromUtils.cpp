#include "eepromUtils.h"

boolean saveToEEPROM() {
    DynamicJsonDocument data(EEPROM_SIZE);

    /*
                    BUILD BASIC DATA SET
     */
    data["0"] = factoryReset;
    data["1"] = webLogin;
    data["2"] = webPassword;
    data["3"] = ap_ssid;
    data["4"] = ap_password;
    data["5"] = ap_ssid_2;
    data["6"] = ap_password_2;
    data["7"] = pwrVolt;
    data["8"] = precFactor;
    data["9"] = vdPrecFactor;

    /*
                    BUILD SERVERS DATA LIST
     */
    JsonObject serversData = data.createNestedObject("A");
    JsonArray servers = serversData.createNestedArray("1");  // servers array
    StaticJsonDocument<128> doc;
    int sCount = 0;
    for (int i = 0; i < serversCount; i++) {
        if (managedServers[i].serverName.equals(SERVER_REMOVE_FLAG)){
            continue;
        }
        doc.clear();
        JsonObject obj = doc.createNestedObject();
        obj["0"] = managedServers[i].serverName;  // server name
        obj["1"] = managedServers[i].ipAddress;   // ip address
        obj["2"] = managedServers[i].macAddress;  // mac address
        servers.add(obj);
        sCount++;
    }
    serversData["0"] = sCount;                    // servers count

    String output;
    serializeJson(data, output);
    Serial.println(output);

    unsigned int outSize = output.length();

    for (int i = 0; i < outSize; i++) {
        EEPROM.write(0x0F + i, output[i]);
    }

    boolean ok = EEPROM.commit();

    Serial.println((ok) ? "Commit OK" : "Commit failed");
    return ok;
}

void loadDataFromEEPROM() {
    delay(5000);
    String json;
    for (int i = 0; i < EEPROM_SIZE; i++) {
        json = json + char(EEPROM.read(0x0F + i));
    }
    Serial.println(json);

    StaticJsonDocument<EEPROM_SIZE> root;

    DeserializationError error = deserializeJson(root, json);

    if (!error) {
        Serial.println("loading data to variables");

        int factoryResetInt = root["0"];
        if (factoryResetInt == 1) {
            factoryReset = true;
            Serial.println("Factory reset requested !");
            doFactoryReset();
        }

        webLogin = root["1"].as<String>();
        webPassword = root["2"].as<String>();

        ap_ssid = root["3"].as<String>();
        ap_password = root["4"].as<String>();

        ap_ssid_2 = root["5"].as<String>();
        ap_password_2 = root["6"].as<String>();

        pwrVolt = root["7"];

        precFactor = root["8"];

        vdPrecFactor = root["9"];

        JsonObject jo = root["A"];
        serversCount = jo["0"];
        // Serial.print("servers count: "); Serial.println(serversCount);

        JsonArray servers = jo["1"];
        // Serial.println("load servers from eeprom:");
        for (int i = 0; i < serversCount; i++) {
            managedServer ms;
            ms.serverName = servers[i]["0"].as<String>();
            ms.ipAddress = servers[i]["1"].as<String>();
            ms.macAddress = servers[i]["2"].as<String>();
            // Serial.print(ms.serverName); Serial.print(" ");
            // Serial.print(ms.ipAddress); Serial.print(" ");
            // Serial.print(ms.macAddress); Serial.println(" ");

            managedServers[i] = ms;
        }

    } else {
        Serial.print("Error while deserializing eeprom");
        Serial.println(error.c_str());
    }
}

void clearEEPROM() {
    StaticJsonDocument<EEPROM_SIZE> data;

    data["1"] = false;
    data["2"] = "";
    data["3"] = "";
    data["4"] = "";
    data["5"] = "";
    data["6"] = "";
    data["7"] = "";
    data["8"] = "";
    data["9"] = "";
    data["0"] = "";
    data["A"] = "";

    String output;
    serializeJson(data, output);
    Serial.println(output);

    unsigned int outSize = output.length();

    for (int i = 0; i < outSize; i++) {
        EEPROM.write(0x0F + i, output[i]);
    }

    boolean ok = EEPROM.commit();
    Serial.println((ok) ? "Commit OK" : "Commit failed");
}

boolean cleanEEPROM(){
    for (int i = 0; i < EEPROM_SIZE; i++) {
        EEPROM.write(0x0F + i, 0);
    }

    boolean ok = EEPROM.commit();
    Serial.println((ok) ? "Commit OK" : "Commit failed");
    return ok;
}

void saveSystemVoltageCalibration(float sysVoltageToSave) {
    pwrVolt = sysVoltageToSave;
    saveToEEPROM();
}

void saveADCCalibration(float precFactorToSave) {
    precFactor = precFactorToSave;
    saveToEEPROM();
}

void saveUPSBatteryVoltageCalibration(float vdPrecFactorToSave) {
    vdPrecFactor = vdPrecFactorToSave;
    saveToEEPROM();
}