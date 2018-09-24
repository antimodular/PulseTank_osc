
#define USE_FINGER
//#define USE_HANDS

//#define USE_WIFI

#include "build_version.h"

int deviceId = 2; //set this via drip switch. starts with 0

//------------------ wifi or ethernet + OSC -------------------
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>

#ifdef USE_WIFI

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#else


#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>
#include "mac.h"
#include "SPI.h"
#include <EEPROM.h>
#include <alloca.h>
#include <string>
#include <cstring>
#include <alloca.h>
#include <string>
#include <cstring>

#pragma GCC diagnostic ignored "-Wwrite-strings"

#endif




//--------------wifi or ethernet + OSC ----------
#ifdef USE_WIFI
char ssid[] = "Eltopo";          // your network SSID (name)
char pass[] = "octavio1914";

WiFiUDP Udp;                                // A UDP instance to let us send and receive packets over UDP
const IPAddress outIp(10, 0, 1, 86);//set this to your computer's IP

#else
EthernetUDP Udp;
const IPAddress outIp(188, 0, 1, 255);//set this to your computer's IP
macAddress myMac;               // uses the library T3Mac
#endif


const unsigned int outPort = 9999;          // remote port to receive OSC
const unsigned int localPort = 8888;        // local port to listen for OSC packets (actually not used for sending)

OSCErrorCode error;

//-----------pin assignment-------

#ifdef USE_WIFI
//via feather HUZZAH
const int PULSE_INPUT = A0;
const int HAND_INPUT = 13;
const int PULSE_FADE = 0;
#else
//via wiznet + teensy
const int PULSE_INPUT = A6;
const int HAND_INPUT = 14;
const int PULSE_FADE = A9;
const int TOUCH_PIN = A1; //A1;
#endif


//-------- Finger Sensor Variables --------------
#ifdef USE_FINGER

//#ifdef USE_WIFI //via feather huzzah
#define USE_ARDUINO_INTERRUPTS false //when using feather huzzah we can't use interrupts
//#else
//#define USE_ARDUINO_INTERRUPTS true
//#endif

#include <PulseSensorPlayground.h>

//const int OUTPUT_TYPE = SERIAL_PLOTTER;  //only needed for debugging
const int OUTPUT_TYPE = PROCESSING_VISUALIZER;  //only needed for debugging

bool bSensorType =  0; //0 == finger, 1 = hands

int THRESHOLD = 750;  // Adjust this number to avoid noise when idle

byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

PulseSensorPlayground pulseSensor;

#endif

//-------- Hands Sensor Variables --------------
bool handsOn = false;
#ifdef USE_HANDS
#include <elapsedMillis.h>

volatile bool pulse = 0;
bool lastPulse = 0;

elapsedMillis high_duration;
elapsedMillis low_duration;

unsigned long myTimer;

unsigned long pulsePeriode;
unsigned long pulseWidth;

int signalType; //hand sensor's digital pin creates different on off timings for different types of data. hands on, pulse, hands off


#endif

//------------touch sensor--------------

int touchThreshold = 3000;

bool isTouched = false;
bool old_isTouched = false;

//------------ actuator ---------------
#ifdef USE_WIFI
const int actuator0_pin = 12;
const int actuator1_pin = 22; // i know there is no pin 22 maybe this way ???
#else
const int actuator0_pin = 4;
const int actuator1_pin = 3;
#endif

int onTimePrimary = 20; //20; //30;
int onTimeSecondary = 20; //20; //30;

bool forceSolenoid = false;
unsigned long forceTimer;

//int downTimePrimary = 35 * 4;
//int downTimeSecondary = 35 * 4;
//int new_downTimePrimary = downTimePrimary;
//int new_downTimeSecondary = downTimeSecondary;
//int primaryDuration = 25;

//--------- helper --------------------
const int PULSE_BLINK = 0;    // Pin 13 is the on-board LED

int sensorSample;
int BPM;

unsigned long tickTimer; //code will send a tick every so often so computer knows connecgtion is still alive

bool bUseFake;
bool bUseFinder;

int fakeSample, fakeBPM;
unsigned long fakeTimer;

bool bDebug = false;


void setup() {
  Serial.println("START APP- PulseTank_osc");
  Serial.begin(115200);

  Serial.println("---PulseTank_osc---");

  Serial.print("version ");
  Serial.println(VERSIONNUM);

  Serial.print("deviceId ");
  Serial.println(deviceId);

#ifdef USE_FINGER
  Serial.print("sensor type ");
  Serial.println("USE_FINGER");
#else
  Serial.print("sensor type ");
  Serial.println("USE_HANDS");
#endif

  //----------Finger PulseSensor
#ifdef USE_FINGER
  setup_fingerSensor();
  setup_touchSensor();
#endif

#ifdef USE_HANDS
  setup_handsSensor();
   attachInterrupt(digitalPinToInterrupt(HAND_INPUT), handInput_interrupt, CHANGE);

#endif

  setup_actuator();
  //----------- Connect to WiFi network

  setup_osc();


  //-------helper
  bUseFake = false;
  fakeSample = 512;
  fakeBPM = 60;


}



void loop() {

  loop_osc();


  if (bUseFake == true) {
    if (millis() - fakeTimer > 100) {
      fakeTimer = millis();
      fakeSample += random(-14, 15);
      fakeSample = constrain(fakeSample, 0, 1024);
      if (fakeSample == 0) fakeSample = 512;
      sampleSend(fakeSample);

      fakeBPM += random(-4, 5);
      fakeBPM = constrain(fakeBPM, 20, 200);
      if (fakeBPM == 0) fakeBPM = 60;
      bpmSend(fakeBPM);
    }

  } else {

#ifdef USE_FINGER
    loop_fingerSensor();
    check_touchSensor();
#endif

#ifdef USE_HANDS
    loop_handsSensor();
#endif

    loop_actuator();
    /*******
      Here is a good place to add code that could take up
      to a millisecond or so to run.
    *******/
  }// bUseFake == false;

  //------update ticker
  if (millis() - tickTimer > 1000) {
    tickTimer = millis();
    tickerSend();
  }

}


