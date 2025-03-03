

// Quadrature Generator
// https://wokwi.com/projects/403218793081155585

// Define the output pins



#include "Arduino.h"

namespace cycles64
{
    uint64_t get();

    namespace // private -----------------------------
    {
        uint32_t oldLow = ARM_DWT_CYCCNT;
        uint32_t curHigh = 0;

        void SNVS_isr(void)
        {
            SNVS_HPSR |= 0b11; // reset interrupt flag
            get();             // call to check for overflow
            asm("dsb");        // prevent double calls of the isr
        }

    } // end private namespace <<---------------------

    uint64_t get()
    {
        noInterrupts();
        uint32_t curLow = ARM_DWT_CYCCNT;
        if (curLow < oldLow) // roll over
        {
            curHigh++;
        }
        oldLow = curLow;
        uint64_t curVal = ((uint64_t)curHigh << 32) | curLow;
        interrupts();

        return curVal;
        
    }

    void begin()
    {
        // disable periodic snvs interrupt
        SNVS_HPCR &= ~SNVS_HPCR_PI_EN;
        while ((SNVS_HPCR & SNVS_HPCR_PI_EN)) {}

        // interrupt frequency 1Hz
        SNVS_HPCR = SNVS_HPCR_PI_FREQ(0b1111);

        // enable periodic snvs interrupt
        SNVS_HPCR |= SNVS_HPCR_PI_EN;
        while (!(SNVS_HPCR & SNVS_HPCR_PI_EN)) {}

        attachInterruptVector(IRQ_SNVS_IRQ, SNVS_isr);
        NVIC_SET_PRIORITY(IRQ_SNVS_IRQ, 255); // lowest
        NVIC_ENABLE_IRQ(IRQ_SNVS_IRQ);
    }
}


#define pinApos 3
#define pinAneg 4
#define pinBpos 10
#define pinBneg 11

// Define the state machine states
enum State {STATE0, STATE1, STATE2, STATE3};
State currentState = STATE0;

// Timing variables
uint64_t previousNanos = 0;
float interval = 1000.0; // 1000 nanoeconds for each state


int content;
int old = "";
const byte numBytes = 32;
byte receivedBytes[numBytes];
byte numReceived = 0;
boolean newData = false;


void setup() {
  Serial.begin(8000000);
  //while (!Serial);
  // Initialize the output pins
  pinMode(pinApos, OUTPUT);
  pinMode(pinAneg, OUTPUT);
  pinMode(pinBpos, OUTPUT);
  pinMode(pinBneg, OUTPUT);

  pinMode(13, OUTPUT);

  // Initialize the pins to a known state
  digitalWrite(pinApos, LOW);
  digitalWrite(pinAneg, LOW);
  digitalWrite(pinBpos, LOW);
  digitalWrite(pinBneg, LOW);

  digitalWrite(13, HIGH);

  cycles64::begin();

  Serial1.begin(1000000);
  while (!Serial1);
}

void loop() {
  recvBytesWithStartEndMarkers();
  parseNewData();
  quadGenerator();
}

void recvBytesWithStartEndMarkers() {
  /*
  profiler_t profiler(
    (String("Time spent in ") + 
    String(__FUNCTION__) + 
    String("()")).c_str());
  */

    static boolean recvInProgress = false;
    static byte ndx = 0;
    byte startMarker = 0x3C;
    byte endMarker = 0x3E;
    byte rb;
   
    while (Serial1.available() > 0 && newData == false) {
        rb = Serial1.read();
        if (recvInProgress == true) {
            if (rb != endMarker) {
                receivedBytes[ndx] = rb;
                ndx++;
                if (ndx >= numBytes) {
                    ndx = numBytes - 1;
                }
            }
            else {
                receivedBytes[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                numReceived = ndx;  // save the number for use when printing
                ndx = 0;
                newData = true;
            }
        }

        else if (rb == startMarker) {
            recvInProgress = true;
        }
    }
}

void parseNewData() {
  /*
  profiler_t profiler(
    (String("Time spent in ") + 
    String(__FUNCTION__) + 
    String("()")).c_str());
  */
  if (newData == true) {
    newData = false;

    int content = (receivedBytes[0]<<8) + receivedBytes[1];

    Serial.println(content);
    if (content != old) {
      old = content;
      
      unsigned int frequency = (content * 50) + 10000; // Range from 10khz to 110khz
      interval = (1000000.0 / frequency / 4) * 1000.0; // Convert frequency to interval and divide by 4 as we need to do 4 things in a cycle 
      
    }
  }
}

void quadGenerator(){
  /*
  profiler_t profiler(
    (String("Time spent in ") + 
    String(__FUNCTION__) + 
    String("()")).c_str());
  */
  // Get the current time in nanoseconds
  // Check if the interval has passed

  if (cycles64::get() - previousNanos >= interval) {

    // Advance the state machine
    switch (currentState) {
      case STATE0:
        digitalWrite(pinApos, HIGH);
        digitalWrite(pinAneg, LOW);
        currentState = STATE1;
        break;

      case STATE1:
        digitalWrite(pinBpos, HIGH);
        digitalWrite(pinBneg, LOW);
        currentState = STATE2;
        break;

      case STATE2:
        digitalWrite(pinApos, LOW);
        digitalWrite(pinAneg, HIGH);
        currentState = STATE3;
        break;

      case STATE3:
        digitalWrite(pinBpos, LOW);  
        digitalWrite(pinBneg, HIGH);
        currentState = STATE0;
        break;       
    }
    
    previousNanos = cycles64::get();

  }
}