#include "Do_sensor.h"
void Do_sensor :: readDoCharacteristicValues(void)
{
        EEPROM_read(SaturationDoVoltageAddress, SaturationDoVoltage);
        EEPROM_read(SaturationDoTemperatureAddress, SaturationDoTemperature);
        if(EEPROM.read(SaturationDoVoltageAddress)==0xFF && EEPROM.read(SaturationDoVoltageAddress+1)==0xFF && EEPROM.read(SaturationDoVoltageAddress+2)==0xFF && EEPROM.read(SaturationDoVoltageAddress+3)==0xFF)
        {
                SaturationDoVoltage = 1127.6; //default voltage:1127.6mv
                EEPROM_write(SaturationDoVoltageAddress, SaturationDoVoltage);
        }
        if(EEPROM.read(SaturationDoTemperatureAddress)==0xFF && EEPROM.read(SaturationDoTemperatureAddress+1)==0xFF && EEPROM.read(SaturationDoTemperatureAddress+2)==0xFF && EEPROM.read(SaturationDoTemperatureAddress+3)==0xFF)
        {
                SaturationDoTemperature = 25.0; //default temperature is 25^C
                EEPROM_write(SaturationDoTemperatureAddress, SaturationDoTemperature);
        }
}
int Do_sensor :: getMedianNum(int bArray[], int iFilterLen)
{
        int bTab[iFilterLen];
        for (byte i = 0; i<iFilterLen; i++)
        {
                bTab[i] = bArray[i];
        }
        int i, j, bTemp;
        for (j = 0; j < iFilterLen - 1; j++)
        {
                for (i = 0; i < iFilterLen - j - 1; i++)
                {
                        if (bTab[i] > bTab[i + 1])
                        {
                                bTemp = bTab[i];
                                bTab[i] = bTab[i + 1];
                                bTab[i + 1] = bTemp;
                        }
                }
        }
        if ((iFilterLen & 1) > 0)
                bTemp = bTab[(iFilterLen - 1) / 2];
        else
                bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
        return bTemp;
}

float Do_sensor :: dosensor_loop(int DoSensorPin)
{
        float doValue;
        static unsigned long analogSampleTimepoint = millis();
        if(millis()-analogSampleTimepoint > 30U) //every 30 milliseconds,read the analog value from the ADC
        {
                analogSampleTimepoint = millis();
                analogBuffer[analogBufferIndex] = analogRead(DoSensorPin); //read the analog value and store into the buffer
                analogBufferIndex++;
                if(analogBufferIndex == SCOUNT)
                        analogBufferIndex = 0;
        }

        static unsigned long tempSampleTimepoint = millis();
        if(millis()-tempSampleTimepoint > 500U) // every 500 milliseconds, read the temperature
        {
                tempSampleTimepoint = millis();
                //temperature = readTemperature();  // add your temperature codes here to read the temperature, unit:^C
        }

        static unsigned long printTimepoint = millis();
        if(millis()-printTimepoint > 1000U)
        {
                printTimepoint = millis();
                for(copyIndex=0; copyIndex<SCOUNT; copyIndex++)
                {
                        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
                }
                averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 4495.0; // read the value more stable by the median filtering algorithm
                // Serial.print(F("averageVoltage:"));
                // Serial.print(SaturationDoVoltage,1);
                // Serial.print(F("^C"));
                doValue = pgm_read_float_near( &SaturationValueTab[0] + (int)(SaturationDoTemperature+0.5) ) * averageVoltage / SaturationDoVoltage; //calculate the do value, doValue = Voltage / SaturationDoVoltage * SaturationDoValue(with temperature compensation)
                // Serial.print(F(",  DO Value:"));
                // Serial.print(doValue,2);
                // // tb.sendTelemetryFloat("DO_SENSOR-",doValue);
                // Serial.println(F("mg/L"));
                // Serial.print(F("Temperature:  "));
                // Serial.print(analogRead(DoSensorPin));
        }
        return doValue;

}
Do_sensor _do_sensor;
