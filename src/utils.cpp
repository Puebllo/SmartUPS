#include "utils.h"

String getUptime(){

  long startTime = millis();

  long ss = int((startTime / (1000)) % 60);
  long mm = int((startTime / (1000 * 60)) % 60);
  long hh = int((startTime / (1000 * 60 * 60)) % 24);
  long dd = int((startTime / (1000 * 60 * 60 * 24)) % 365);

  // for output
  sprintf(timestring, "%d days %02d:%02d:%02d", dd, hh, mm, ss);

 //String time =  systemUpTimeDy + String(" days, ") + systemUpTimeHr + String(":") + systemUpTimeMn;

 // Serial.println(timestring);
  return timestring;
}


String getPrecCalibData()
{
  return String(precFactor,8) + ";" + String(vdPrecFactor,8) + ";" + String(pwrVolt,3);
}

String getCalibData()
{
  return String(adcVal) + ";" + String(espVolt) + ";" + String(upsBatteryVoltage) + ";" + String(stateOfCharge);
}

String getInitData()
{
  return String(ip) + ";" + String(WiFi.macAddress()) + ";" + getUptime();
}

String processor(const String &var)
{
  return String();
}




float mapDouble(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double getVoltage() {
  adcVal = analogRead(A0);
  //Serial.print("ADC: ");   Serial.println(val);
  espVolt = ((pwrVolt / 1024) * adcVal * 1000.0) * precFactor;
    //Serial.print("espVolt: ");   Serial.println(espVolt);

upsBatteryVoltage = mapDouble(espVolt,0.0, 3.256, 0.0, 14.0)* vdPrecFactor;

    //Serial.print("mapped: ");   Serial.println(mapped);

  return upsBatteryVoltage;
}


float getLinearizationRange(double voltage, int linearSize, float voltageRange[], float valueRange[]) {
  int previous_i = 0;
  for (int i = 1 ; i <= linearSize; i++) {
    if (voltage < voltageRange[previous_i] && voltage > voltageRange[i]) {
      return mapDouble(voltage, voltageRange[previous_i], voltageRange[i], valueRange[previous_i],  valueRange[i]);
    }
    previous_i = i;
  }
  return -1;
}





void checkUPSStatus(){

//Check if UPS is online
if (digitalRead(PHASE_PIN) == 1){
CURRENT_STATUS = STATUS_ONBATTERY;
}else{
  CURRENT_STATUS = STATUS_ONLINE;
}

//measure battery voltage
float voltage = 0.0;
  for (int i = 0 ; i < MEASURE_COUNT; i++) {
    voltage += getVoltage();
    delay(10);
  }
  voltage = voltage / MEASURE_COUNT;
   // Serial.print("mean voltage: ");   Serial.println(voltage);

if(voltage > 12700.0){
  stateOfCharge = 100;
}else{
stateOfCharge = getLinearizationRange(voltage, 11, socValueArray, battPercArray);
}
}

void doFactoryReset(){

}

String getConfigData(){
  return webLogin +";"+ ap_ssid+";"+ ap_ssid_2;
}