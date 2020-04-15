#include <Arduino.h>
#include "Do_sensor.h"
#include <pgmspace.h>//please_include library
#include <EEPROM.h> //please_include library
int DoSensorPin = 35;
void setup() {
        Serial.begin(115200);
        pinMode(DoSensorPin,INPUT);
        _do_sensor.readDoCharacteristicValues();//read Characteristic Values calibrated from the EEPROM
}
void loop()
{
        Serial.print("DO_sensor : ");
        Serial.println(_do_sensor.dosensor_loop(DoSensorPin));
        delay(1000);
}
