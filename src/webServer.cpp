#include "webServer.h"

#include "machines.h"

AsyncWebServer server(webPort);

String getRemainingSessionTime(IPAddress sessionIp) {
    String ip = sessionIp.toString();
    char sessionTimestring[5];

    long ss = 0;
    long mm = 0;

    for (int i = 0; i < 3; i++) {
        webSession ws = sessions[i];
        if (!ws.ipAddress.equals("-1") && ws.ipAddress.equals(ip)) {
            unsigned long maxTime = ws.loginTime + sessionTime;
            unsigned long now = millis();

            if (maxTime >= now) {
                unsigned long remainingTime = maxTime - now;

                ss = int((remainingTime / (1000)) % 60);
                mm = int((remainingTime / (1000 * 60)) % 60);
            }

            break;
            // for output
        }
    }
    sprintf(sessionTimestring, "%02d:%02d", mm, ss);
    return sessionTimestring;
}

boolean saveUserSession(unsigned long loginTime, IPAddress sessionIp) {
    // check if session for this ip exists and if yes then reload time
    for (int i = 0; i < 3; i++) {
        webSession ws = sessions[i];
        if (ws.ipAddress.equals(sessionIp.toString())) {
            Serial.println("found existing session. Refreshing");
            ws.loginTime = loginTime;
            return true;
            break;
        }
    }

    for (int i = 0; i < 3; i++) {
        webSession ws = sessions[i];
        if (ws.ipAddress.equals("-1")) {
            webSession wsNew;
            Serial.print("Found place for session with ip: ");
            Serial.println(sessionIp.toString());
            wsNew.ipAddress = sessionIp.toString();
            wsNew.loginTime = loginTime;
            sessions[i] = wsNew;
            return true;
            break;
        }
    }

    Serial.print("reached max sessions = ");
    Serial.println(sizeof(sessions));
    return false;
}

boolean checkIfSessionExistsAndNotExpired(IPAddress sessionIp) {
    for (int i = 0; i < 3; i++) {
        webSession ws = sessions[i];
        if (ws.ipAddress.equals(sessionIp.toString())) {
            unsigned long maxTime = ws.loginTime + sessionTime;
            if (millis() < maxTime) {
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}

void clearSession(int index, webSession ws) {
    Serial.print("Releasing session with ip: ");
    Serial.println(ws.ipAddress);

    ws.ipAddress = "-1";
    ws.loginTime = 0;
    sessions[index] = ws;
}

boolean removeSessionForIP(IPAddress sessionIp) {
    for (int i = 0; i < 3; i++) {
        webSession ws = sessions[i];
        if (ws.ipAddress.equals(sessionIp.toString())) {
            clearSession(i, ws);
            return true;
        }
    }
    return false;
}

void validateSessions() {
    for (int i = 0; i < 3; i++) {
        webSession ws = sessions[i];
        if (!ws.ipAddress.equals("-1")) {
            unsigned long maxTime = ws.loginTime + sessionTime;
            if (millis() > maxTime) {
                clearSession(i, ws);
            }
        }
    }
}

void clearSessions() {
    for (int i = 0; i < 3; i++) {
        sessions[i] = {"-1", 0};
    }
}

String getUPSData(IPAddress sessionIp) {
    counter++;
    double voltage = upsBatteryVoltage / 1000.0;
    long voltPrec = (voltage / 14.0) * 100;
    return String(voltage) + ";" + String(voltPrec) + ";" +
           String(stateOfCharge) + ";" + String(counter) + ";" + getUptime() +
           ";" + getRemainingSessionTime(sessionIp) + ";" +
           String(CURRENT_STATUS);
}

String getParamValue(AsyncWebServerRequest *request, String paramName) {
    if (request->hasParam(paramName, true)) {
        AsyncWebParameter *p = request->getParam(paramName, true);
        Serial.print(paramName);
        Serial.print("= ");
        Serial.println(p->value());
        return p->value();
    }
    return "";
}

// Check if smartups is connected to AP
void requestPreprocessor(AsyncWebServerRequest *request,
                         String destinationEndpoint) {
    if (!wifiConnected) {
        request->send(LittleFS, "/ap_index.html", "text/html", false,
                      processor);
    } else {
        if (checkIfSessionExistsAndNotExpired(request->client()->remoteIP())) {
            request->send(LittleFS, destinationEndpoint, "text/html", false,
                          processor);
        } else {
            request->send(LittleFS, "/loginPage.html", "text/html", false,
                          processor);
        }
    }
}

boolean isUserAuthorized(AsyncWebServerRequest *request) {
    boolean auth =
        checkIfSessionExistsAndNotExpired(request->client()->remoteIP());
    if (!auth) {
        request->redirect("/login");
        return false;
    }
    return true;
}

void onConfigUpload(AsyncWebServerRequest *request, String filename,
                    size_t index, uint8_t *data, size_t len, bool final) {
    if (final) {
        String fileData = (const char *)data;
        int idx = filename.lastIndexOf(".");
        String fileExt = filename.substring(idx + 1, filename.length());
        fileExt.toLowerCase();

        if (fileExt.equals("bck")) {
            if (writeAndCommitEEPROM(fileData)) {
                configUploadStatusCode = 1;
            } else {
                configUploadStatusCode = -1;
            }
        } else {
            configUploadStatusCode = -2;
        }
    }
}

void startServer() {
    Serial.println("Starting web server at port 80");
    server.on("/login", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/loginPage.html", "text/html", false,
                      processor);
    });
    server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request) {
        removeSessionForIP(request->client()->remoteIP());
        request->redirect("/");
    });
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        requestPreprocessor(request, "/index.html");
    });
    server.on("/getConfigData", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (isUserAuthorized(request)) {
            request->send_P(200, "text/plain", getConfigData().c_str());
        }
    });
    server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request) {
        requestPreprocessor(request, "/settings.html");
    });
    server.on("/upsConfig", HTTP_GET, [](AsyncWebServerRequest *request) {
        requestPreprocessor(request, "/ap_index.html");
    });

    server.on("/addNewServerForm", HTTP_GET,
              [](AsyncWebServerRequest *request) {
                  requestPreprocessor(request, "/newServerForm.html");
              });
    server.on("/calib", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/calib.html", "text/html", false, processor);
        // request->send_P(200, "text/html", index_html, processor);
    });
    server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/style.css", "text/css");
    });

    server.on("/bootstrap.min.css", HTTP_GET,
              [](AsyncWebServerRequest *request) {
                  request->send(LittleFS, "/bootstrap.min.css", "text/css");
              });

    server.on("/bootstrap.min.js", HTTP_GET,
              [](AsyncWebServerRequest *request) {
                  request->send(LittleFS, "/bootstrap.min.js",
                                "application/javascript");
              });

    server.on("/popper.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/popper.min.js", "application/javascript");
    });

    server.on("/logo.svg", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/logo.svg", "image/svg+xml");
    });
    // AJAX
    server.on("/refreshData", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (checkIfSessionExistsAndNotExpired(request->client()->remoteIP())) {
            request->send_P(200, "text/plain",
                            getUPSData(request->client()->remoteIP()).c_str());
        } else {
            request->send(401);
        }
    });
    server.on("/calibData", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/plain", getCalibData().c_str());
    });
    server.on("/initData", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/plain", getInitData().c_str());
    });
    server.on(
        "/getPrecCalibData", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send_P(200, "text/plain", getPrecCalibData().c_str());
        });

    server.on(
        "/getServersListJson", HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send_P(200, "text/plain", getServersListJson().c_str());
        });

    // Calibration posts
    server.on("/setAdcPrecFactor", HTTP_POST,
              [](AsyncWebServerRequest *request) {
                  if (request->hasParam("adcV", true)) {
                      AsyncWebParameter *p = request->getParam("adcV", true);
                      precFactor = p->value().toFloat();
                      saveADCCalibration(precFactor);
                  }
                  request->redirect("/calib");
              });

    server.on("/setVDPrecFactor", HTTP_POST,
              [](AsyncWebServerRequest *request) {
                  if (request->hasParam("battV", true)) {
                      AsyncWebParameter *p = request->getParam("battV", true);
                      vdPrecFactor = p->value().toFloat();
                      saveUPSBatteryVoltageCalibration(vdPrecFactor);
                  }
                  request->redirect("/calib");
              });

    server.on("/setPowerVoltage", HTTP_POST,
              [](AsyncWebServerRequest *request) {
                  if (request->hasParam("pwrVolt", true)) {
                      AsyncWebParameter *p = request->getParam("pwrVolt", true);
                      pwrVolt = p->value().toFloat();
                      saveSystemVoltageCalibration(pwrVolt);
                  }
                  request->redirect("/calib");
              });

    // Login post
    server.on("/doLogin", HTTP_POST, [](AsyncWebServerRequest *request) {
        String reqLogin = "";
        String reqPwd = "";

        if (request->hasParam("username", true)) {
            AsyncWebParameter *p = request->getParam("username", true);
            reqLogin = p->value().c_str();
        }
        if (request->hasParam("password", true)) {
            AsyncWebParameter *p = request->getParam("password", true);
            reqPwd = p->value().c_str();
        }
        String message = "";
        if (reqLogin.equals(webLogin) && reqPwd.equals(webPassword)) {
            boolean sessionPersisted =
                saveUserSession(millis(), request->client()->remoteIP());
            if (sessionPersisted) {
                Serial.println("redirecting to main page");
                request->redirect("/");
            } else {
                message = "Reached max sessions ! Please try later";
            }
        } else {
            message = "Wrong login or password. Please try again.";
        }
        AsyncWebServerResponse *response = request->beginResponse(
            LittleFS, "/loginPage.html", "text/html", false, processor);
        response->addHeader("msg", message);
        response->setCode(401);
        request->send(response);
    });

    // Configuration post
    server.on("/upsConfig", HTTP_POST, [](AsyncWebServerRequest *request) {
        boolean configChanged = false;
        String value = getParamValue(request, "webusr");
        if ((value.length() > 0) & (!value.equals(webLogin))) {
            webLogin = value.c_str();
            configChanged = true;
        }
        value = getParamValue(request, "webpwd");
        if ((value.length() > 0) & (!value.equals(webPassword))) {
            webPassword = value.c_str();
            configChanged = true;
        }
        value = getParamValue(request, "apSSID1");
        if ((value.length() > 0) & (!value.equals(ap_ssid))) {
            ap_ssid = value.c_str();
            configChanged = true;
        }
        value = getParamValue(request, "apPWD1");
        if ((value.length() > 0) & (!value.equals(ap_password))) {
            ap_password = value.c_str();
            configChanged = true;
        }
        value = getParamValue(request, "apSSID2");
        if ((value.length() > 0) & (!value.equals(ap_ssid_2))) {
            ap_ssid_2 = value.c_str();
            configChanged = true;
        }
        value = getParamValue(request, "apPWD2");
        if ((value.length() > 0) & (!value.equals(ap_password_2))) {
            ap_password_2 = value.c_str();
            configChanged = true;
        }
        float lowBattSoC = getParamValue(request, "lowBattSoC").toFloat();
        if (lowBattSoC != lowBatterySoC) {
            lowBatterySoC = lowBattSoC;
            configChanged = true;
        }
        int sTime = getParamValue(request, "sTime").toInt();
        if (sTime != sessionTime) {
            sessionTime = sTime;
            configChanged = true;
        }
        if (configChanged) {
            saveToEEPROM();
            delay(1000);
            ESP.reset();
        }
    });

    // Add new server Post
    server.on("/addServer", HTTP_POST, [](AsyncWebServerRequest *request) {
        managedServer newServer;
        String value = getParamValue(request, "serverName");
        if ((value.length() > 0) & (!value.equals(webLogin))) {
            newServer.serverName = value.c_str();
        }
        value = getParamValue(request, "serverIp");

        if ((value.length() > 0) & (!value.equals(webLogin))) {
            newServer.ipAddress = value.c_str();
        }
        value = getParamValue(request, "serverMAC");
        if ((value.length() > 0) & (!value.equals(webLogin))) {
            newServer.macAddress = value.c_str();
        }
        addServer(newServer);
        request->redirect("/");
    });

    // remove server
    server.on("/removeServer", HTTP_POST, [](AsyncWebServerRequest *request) {
        String serverName;
        String value = getParamValue(request, "serverName");
        if ((value.length() > 0) & (!value.equals(webLogin))) {
            serverName = value.c_str();
        }
        Serial.print("Server name: ");
        Serial.println(serverName);
        removeServer(serverName);
        request->redirect("/");
    });

    // Turn on server
    server.on("/turnOn", HTTP_POST, [](AsyncWebServerRequest *request) {
        String serverName;
        String value = getParamValue(request, "serverName");
        if ((value.length() > 0) & (!value.equals(webLogin))) {
            serverName = value.c_str();
        }
        Serial.print("Server name: ");
        Serial.println(serverName);
        turnOn(serverName);
        request->redirect("/");
    });

    // reboot SmartUPS
    server.on("/doReboot", HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncWebServerResponse *response = request->beginResponse(
            LittleFS, "/msgPage.html", "text/html", false, processor);
        if (isUserAuthorized(request)) {
            response->addHeader("title", "Reboot UPS");
            response->addHeader("msg", "SmartUPS will now reboot");
            response->setCode(200);
            request->send(response);
            rebootFlag = true;
        } else {
            response->addHeader("title", "I'm sorry...");
            response->addHeader("msg",
                                "You are not authorized to do this action");
            response->addHeader("srcUrl", "/settings");
            response->setCode(401);
            request->send(response);
        }
    });

    // Get Config from EEPROM for backup
    server.on(
        "/getBackupDownload", HTTP_GET, [](AsyncWebServerRequest *request) {
            AsyncWebServerResponse *response = request->beginResponse(
                LittleFS, "/msgPage.html", "text/html", false, processor);
            if (isUserAuthorized(request)) {
                request->send_P(200, "text/plain", getDataFromEEPROM().c_str());
            } else {
                response->addHeader("title", "I'm sorry...");
                response->addHeader("msg",
                                    "You are not authorized to do this action");
                response->addHeader("srcUrl", "/settings");
                response->setCode(401);
                request->send(response);
            }
        });

    // Upload config file
    server.on(
        "/uploadConfig", HTTP_POST,
        [](AsyncWebServerRequest *request) {
            AsyncWebServerResponse *response = request->beginResponse(
                LittleFS, "/msgPage.html", "text/html", false, processor);
            response->setCode(200);
            if (configUploadStatusCode == 1) {
                response->addHeader("title", "Load Configuration");
                response->addHeader(
                    "msg", "Configuration restored. SmartUPS will reboot");
                request->send(response);
                rebootFlag = true;
            } else {
                String msg = "";
                response->addHeader("title", "Error !");
                if (configUploadStatusCode == -1) {
                    msg = "Error while commiting configuration !";
                }
                if (configUploadStatusCode == -2) {
                    msg = "Wrong backup file !";
                }
                response->addHeader("msg", msg);
                response->addHeader("srcUrl", "/settings");
                request->send(response);
            }
        },
        onConfigUpload);

    server.begin();

    Serial.println("Registered endpoints");
    server.begin();
    Serial.println("Webserver started");
}