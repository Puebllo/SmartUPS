#include "eepromUtils.h"

boolean saveToEEPROM() {
    StaticJsonDocument<EEPROM_SIZE> data;

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


    char dataAr[EEPROM_SIZE];
    serializeJson(data, Serial);
    delay(1000);
    serializeJson(data, dataAr);

    for (int i = 0; i < EEPROM_SIZE; i++) {
        EEPROM.write(0x0F + i, dataAr[i]);
    }

    boolean ok = EEPROM.commit();
    Serial.println((ok) ? "Commit OK" : "Commit failed");
}

void loadDataFromEEPROM() {
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



               delay(1000);
        Serial.print("webLogin");
        Serial.println(webLogin);
        Serial.print("webPassword");
        Serial.println(webPassword);
        Serial.print("ap_ssid");
        Serial.println(ap_ssid);
        Serial.print("ap_password");
        Serial.println(ap_password);
        Serial.print("ap_ssid_2");
        Serial.println(ap_ssid_2);
        Serial.print("ap_password_2");
        Serial.println(ap_password_2);
        Serial.print("pwrVolt");
        Serial.println(pwrVolt);
        Serial.print("precFactor");
        Serial.println(precFactor);
        Serial.print("vdPrecFactor");
        Serial.println(vdPrecFactor);

    }
}

void clearEEPROM() {
    StaticJsonDocument<EEPROM_SIZE> data;

    data["1"] = 80;
    data["2"] = 15;
    data["3"] = 8;
    data["4"] = 12;
    data["5"] = "";
    data["6"] = "";
    data["7"] = "";
    data["8"] = "";
    data["9"] = "";
    data["0"] = "";
    data["A"] = "";
    data["B"] = "";
    data["C"] = "";
    data["D"] = 1;

    char dataAr[EEPROM_SIZE];

    serializeJson(data, dataAr);

    for (int i = 0; i < EEPROM_SIZE; i++) {
        EEPROM.write(0x0F + i, dataAr[i]);
    }

    boolean ok = EEPROM.commit();
    Serial.println((ok) ? "Commit OK" : "Commit failed");
}