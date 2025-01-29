/*

  Analog input, analog output, serial output

  Reads an analog input pin, maps the result to a range from 0 to 255 and uses

  the result to set the pulse width modulation (PWM) of an output pin.

  Also prints the results to the Serial Monitor.

  The circuit:

  - potentiometer connected to analog pin 0.

    Center pin of the potentiometer goes to the analog pin.

    side pins of the potentiometer go to +5V and ground

  - LED connected from digital pin 9 to ground

  created 29 Dec. 2008

  modified 9 Apr 2012

  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/AnalogInOutSerial

*/

// These constants won't change. They're used to give names to the pins used:


const int analogInPin = A1;  // Analog input pin that the potentiometer is attached to

int sensorValue = 0;  

byte startMarker = 0x3C;
byte endMarker = 0x3E;

void setup() {

  // initialize serial communications at 9600 bps:
  analogReference(EXTERNAL); 
  Serial.begin(1000000);

}

void loop() {

  // read the analog in value:

  int sensorValue = analogRead(analogInPin);

  Serial.write(0x3C);
  Serial.write(highByte(sensorValue));
  Serial.write(lowByte(sensorValue));
  Serial.write(0x3E);
 
  delay(10);
}