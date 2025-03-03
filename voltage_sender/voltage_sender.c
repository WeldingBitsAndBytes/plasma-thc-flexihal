/*
 Differential ADC library
 David Pilling 
 Version 0.00 1st August 2017

 Do what you like with it, I don't care, but note the conditions on the two sources below.

 Some code and ideas from

 wiring_differential.c - analog differential input
  by Muh Nahdhi Ahsan http://sekarlangit.com/arduino-differential-gain.php
                      https://lawas.nahdhi.com/arduino-differential-adc-and-gain-adc.html
                      
 wiring_differential.c modified from wiring_analog.c
 wiring_analog.c - Copyright (c) 2005-2006 David A. Mellis
 wiring_analog.c - modified 28 September 2010 by Mark Sproul
 wiring_analog.c - Part of Arduino - http://arduino.cc
  
*/


#include "wiring_private.h"
#include "pins_arduino.h"


#define UNUSEDCHANNEL 255
#define VBG11 254
#define VGND 253

typedef struct
{
 uint8_t c1;
 uint8_t c2;
 uint8_t gain;
} tablestr;


#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

#define TABLESIZE 64

static tablestr codetable[TABLESIZE]=
{
0,UNUSEDCHANNEL,1,
1,UNUSEDCHANNEL,1,
2,UNUSEDCHANNEL,1,
3,UNUSEDCHANNEL,1,
4,UNUSEDCHANNEL,1,
5,UNUSEDCHANNEL,1,
6,UNUSEDCHANNEL,1,
7,UNUSEDCHANNEL,1,
0,0,1,
1,0,1,
0,0,200,
1,0,200,
2,2,10,
3,2,10,
2,2,200,
3,2,200,
0,1,1,
1,1,1,
2,1,1,
3,1,1,
4,1,1,
5,1,1,
6,1,1,
7,1,1,
0,2,1,
1,2,1,
2,2,1,
3,2,1,
4,2,1,
5,2,1,
VBG11,UNUSEDCHANNEL,0,
VGND,UNUSEDCHANNEL,0,
8,UNUSEDCHANNEL,1,
9,UNUSEDCHANNEL,1,
10,UNUSEDCHANNEL,1,
11,UNUSEDCHANNEL,1,
12,UNUSEDCHANNEL,1,
13,UNUSEDCHANNEL,1,
14,UNUSEDCHANNEL,1,
15,UNUSEDCHANNEL,1,
8,8,10,
9,8,10,
8,8,200,
9,8,200,
10,10,10,
11,10,10,
10,10,200,
11,10,200,
8,9,1,
9,9,1,
10,9,1,
11,9,1,
12,9,1,
13,9,1,
14,9,1,
15,9,1,
8,10,1,
9,10,1,
10,10,1,
11,10,1,
12,10,1,
13,10,1,
UNUSEDCHANNEL,UNUSEDCHANNEL,0,
UNUSEDCHANNEL,UNUSEDCHANNEL,0
};


#elif defined(__AVR_ATmega32U4__)

#define TABLESIZE 64

static tablestr codetable[TABLESIZE]=
{
0,UNUSEDCHANNEL,1,
1,UNUSEDCHANNEL,1,
UNUSEDCHANNEL,UNUSEDCHANNEL,0,
UNUSEDCHANNEL,UNUSEDCHANNEL,1,
4,UNUSEDCHANNEL,1,
5,UNUSEDCHANNEL,1,
6,UNUSEDCHANNEL,1,
7,UNUSEDCHANNEL,1,
UNUSEDCHANNEL,UNUSEDCHANNEL,0,
1,0,10,
UNUSEDCHANNEL,UNUSEDCHANNEL,0,
1,0,200,
UNUSEDCHANNEL,UNUSEDCHANNEL,0,
UNUSEDCHANNEL,UNUSEDCHANNEL,0,
UNUSEDCHANNEL,UNUSEDCHANNEL,0,
UNUSEDCHANNEL,UNUSEDCHANNEL,0,
0,1,1,
UNUSEDCHANNEL,UNUSEDCHANNEL,0,
UNUSEDCHANNEL,UNUSEDCHANNEL,0,
UNUSEDCHANNEL,UNUSEDCHANNEL,0,
4,1,1,
5,1,1,
6,1,1,
7,1,1,
UNUSEDCHANNEL,UNUSEDCHANNEL,0,
UNUSEDCHANNEL,UNUSEDCHANNEL,0,
UNUSEDCHANNEL,UNUSEDCHANNEL,0,
UNUSEDCHANNEL,UNUSEDCHANNEL,0,
UNUSEDCHANNEL,UNUSEDCHANNEL,0,
UNUSEDCHANNEL,UNUSEDCHANNEL,0,
VBG11,UNUSEDCHANNEL,0,
VGND,UNUSEDCHANNEL,0,
8,UNUSEDCHANNEL,1,
9,UNUSEDCHANNEL,1,
10,UNUSEDCHANNEL,1,
11,UNUSEDCHANNEL,1,
12,UNUSEDCHANNEL,1,
13,UNUSEDCHANNEL,1,
1,0,40,
UNUSEDCHANNEL,UNUSEDCHANNEL,0,
4,0,10,
5,0,10,
6,0,10,
7,0,10,
4,1,10,
5,1,10,
6,1,10,
7,1,10,
4,0,40,
5,0,40,
6,0,40,
7,0,40,
4,1,40,
5,1,40,
6,1,40,
7,1,40,
4,0,200,
5,0,200,
6,0,200,
7,0,200,
4,1,200,
5,1,200,
6,1,200,
7,1,200
};


#elif defined(__AVR_ATmega1284__)

#define TABLESIZE 32

static tablestr codetable[TABLESIZE]=
{
0,UNUSEDCHANNEL,1
1,UNUSEDCHANNEL,1
2,UNUSEDCHANNEL,1
3,UNUSEDCHANNEL,1
4,UNUSEDCHANNEL,1
5,UNUSEDCHANNEL,1
6,UNUSEDCHANNEL,1
7,UNUSEDCHANNEL,1
0,0,10
1,0,10
0,0,200
1,0,200
2,2,10
3,2,10
2,2,200
3,2,200
0,1,1
1,1,1
2,1,1
3,1,1
4,1,1
5,1,1
6,1,1
7,1,1
0,2,1
1,2,1
2,2,1
3,2,1
4,2,1
5,2,1
VBG11,UNUSEDCHANNEL,0,
VGND,UNUSEDCHANNEL,0,
};


#else
#error "unsupported processor"
#endif




static uint8_t diff_analog_reference = DEFAULT;

/*
 * DEFAULT: the default analog reference of 5 volts (on 5V Arduino boards) or 3.3 volts (on 3.3V Arduino boards)
INTERNAL: an built-in reference, equal to 1.1 volts on the ATmega168 or ATmega328 and 2.56 volts on the ATmega8 (not available on the Arduino Mega)
INTERNAL1V1: a built-in 1.1V reference (Arduino Mega only)
INTERNAL2V56: a built-in 2.56V reference (Arduino Mega only)
EXTERNAL: the voltage applied to the AREF pin (0 to 5V only) is used as the reference.
 */

#define ADCDELAY 200


void analogReferenceDiff(uint8_t mode)
{
  diff_analog_reference = mode;
}


int analogSetDiffCode(uint8_t code)
{
 uint8_t low, high;

 ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((code>>5) & 0x01) << MUX5);
 ADMUX = (diff_analog_reference << 6) | (code & 0x1F);
  
 delayMicroseconds(ADCDELAY);
}


int analogReadDiff(void)
{
  uint8_t low, high;

  sbi(ADCSRA, ADSC);

  while (bit_is_set(ADCSRA, ADSC));

  low  = ADCL;
  high = ADCH;

  return (high << 8) | low;
}

int analogReadDiffCode(uint8_t code)
{
 analogSetDiffCode(code);
 return(analogReadDiff());
}


int analogArduinoToAVR(uint8_t pin)
{
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  if (pin >= 54) pin -= 54; // allow for channel or pin numbers
#elif defined(__AVR_ATmega32U4__)
  if (pin >= 18) pin -= 18; // allow for channel or pin numbers
  pin = analogPinToChannel(pin);
#elif defined(__AVR_ATmega1284__)
  if (pin >= 24) pin -= 24; // allow for channel or pin numbers
#else
  if (pin >= 14) pin -= 14; // allow for channel or pin numbers
#endif
 
 return(pin);
}



  

#define NOCODE 0

int analogGetCodeAVR(uint8_t pin1,uint8_t pin2,uint16_t gain)
{
 int i;
 int j;
 int temp;

 for(j=0;j<2;j++)
 {
  for(i=0;i<TABLESIZE;i++)
  {
   if(codetable[i].c1==pin1 && codetable[i].c2==pin2 && codetable[i].gain==gain) return(i);
  }
  
  temp=pin1;
  pin1=pin2;
  pin2=temp;
 } 
 return(NOCODE);
}



int analogGetCode(uint8_t pin1,uint8_t pin2,uint16_t gain)
{
 pin1=analogArduinoToAVR(pin1);
 if(pin2!=UNUSEDCHANNEL) pin2=analogArduinoToAVR(pin2);
 return(analogGetCodeAVR(pin1,pin2,gain));
}


int analogSignedValue(int value)
{
  if(value < 511 )
  {
    value = -1 * value ;
  }
  else
  {
    value = -1 * ( value - 1023 ) ;
  }

 return(value);
}

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1284__)

// based on https://code.google.com/p/tinkerit/wiki/SecretVoltmeter

long readVcc() 
{
 long result;
 int  save_adcsrb=ADCSRB;
 int  save_admux=ADMUX;

 // Read 1.1V reference against AVcc

 ADCSRB = (ADCSRB & ~(1 << MUX5));
 ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
 delayMicroseconds(ADCDELAY);

 result=analogReadDiff();
 ADMUX=save_admux;
 ADCSRB=save_adcsrb;

 if(result)  result = 1126400L / result; // Back-calculate AVcc in mV
 return result;
}

#else
#error "unsupported processor"
#endif


static int voltscale=1100;

int analogVoltageSetup(void)
{
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1284__)

if(diff_analog_reference==0x3) voltscale=2560;
if(diff_analog_reference==0x2) voltscale=1100;
if(diff_analog_reference==0x1) voltscale=readVcc();

#elif defined(__AVR_ATmega32U4__)

if(diff_analog_reference==0x3) voltscale=2560;
if(diff_analog_reference==0x1) voltscale=readVcc();

#else
#error "unsupported processor"
#endif
 
 return(voltscale); 
}




int analogVoltageValue(int value,int base,int gain)
{
 value=((long)value*(long)voltscale*(long)base)/((long)512*(long)gain);
 return(value);
}



