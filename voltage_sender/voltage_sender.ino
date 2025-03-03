// David Pilling July 2017

#include "voltage_sender.c"

int sensorValue = 0;

byte msg[4];

void setup() 
{
 Serial1.begin(1000000); 
 while(!Serial1);
}


void loop() 
{
 int code;
 int vcc;
 int sensorValue;                 // Millivolts

 vcc = analogVoltageSetup();

 code = analogGetCode(A4,A5,1);
 sensorValue=analogVoltageValue(analogReadDiffCode(code),1,1);

 msg[0] = 0x3C;
 msg[1] = highByte(sensorValue);
 msg[2] = lowByte(sensorValue);
 msg[3] = 0x3E;

 Serial1.write(msg,4);

 delay(20);
}


