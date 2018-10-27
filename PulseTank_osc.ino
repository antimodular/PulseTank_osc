
#define USE_FINGER
//#define USE_HANDS

//#define USE_WIFI

#define DEFAULT_BPM 73

#include "build_version.h"

bool bDebug = false; // can be enabled by computer GUI sending the right osc message

int deviceId = 0; //set this via dip switch. starts with 0
#include "dipSwitch.h"

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
const int PULSE_INPUT = A6; // aka 20
const int HAND_INPUT = 14;
const int PULSE_FADE = A9;
const int TOUCH_PIN = A8;
const int MICRO_PIN = A5; // aka 19;
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

int THRESHOLD = 400;  // Adjust this number to avoid noise when idle

byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 10;

PulseSensorPlayground pulseSensor;

//----interval timer------

IntervalTimer timer0;

volatile int BPM_interval;                   // used to hold the pulse rate
volatile int Signal_interval;                // holds the incoming raw data
volatile int IBI_interval = 600;             // holds the time between beats, the Inter-Beat Interval
volatile boolean Pulse_interval = false;     // true when pulse wave is high, false when it's low
volatile boolean QS_interval = false;        // becomes true when Arduoino finds a beat.
#endif

//-------- Hands Sensor Variables --------------
bool handsOn = false;
#ifdef USE_HANDS
#include <elapsedMillis.h>

volatile bool pulse = 0;
volatile bool lastPulse = 0;

//TODO: can ellapsedMillis be used in interrupt function?
elapsedMillis high_duration;
elapsedMillis low_duration;

unsigned long myTimer;

unsigned long pulsePeriode;
unsigned long pulseWidth;

volatile int signalType; //hand sensor's digital pin creates different on off timings for different types of data. hands on, pulse, hands off
volatile int old_cnt;
volatile int new_cnt;

volatile int new_BPM;
volatile int old_BPM;
volatile bool new_handsOn;
volatile bool old_handsOn;
//volatile int new_low_duration;
//volatile int new_high_duration;

//unsigned long hands_onTimer;

#endif

//------------touch sensor--------------

int touchThreshold = 5500;

bool isTouched = false;
bool old_isTouched = false;
unsigned long touch_onTimer = 0;

//------------ actuator ---------------
#ifdef USE_WIFI
const int actuator0_pin = 12;
const int actuator1_pin = 22; // i know there is no pin 22 maybe this way ???
#else
const int actuator0_pin = 4;
const int actuator1_pin = 3;
#endif

#ifdef USE_HANDS
int onTimePrimary = 34; //20; //30;
int onTimeSecondary = 34; //20; //30;
#else
int onTimePrimary = 20; //20; //30;
int onTimeSecondary = 20; //20; //30;
#endif

bool forceSolenoid = false;
unsigned long forceTimer;

int maxWaitTime = 4000;

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
unsigned long tickTimer2;
int alive_cnt;
bool micro_state;

bool bUseFake;
bool bUseFinder;

int fakeSample, fakeBPM;
unsigned long fakeTimer;




void setup() {
  Serial.println("START APP- PulseTank_osc");
  Serial.begin(115200);

  Serial.println("---PulseTank_osc---");

  Serial.print("version ");
  Serial.println(VERSIONNUM);

  Serial.print("deviceId ");
  Serial.println(deviceId);

#ifdef USE_FINGER
  Serial.print("sensor type: ");
  Serial.println("USE_FINGER");
#else
  Serial.print("sensor type: ");
  Serial.println("USE_HANDS");
#endif

  pinMode(MICRO_PIN, OUTPUT);
  digitalWrite(MICRO_PIN, HIGH);
  alive_cnt = 0;
  micro_state = true;

  init_dipSwitch_IP();
  deviceId = getIP();

  //----------Finger PulseSensor
#ifdef USE_FINGER
  timer0.begin(timerCallback0, 2000);

  setup_fingerSensor();
//  setup_touchSensor();
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
     check_touchSensor();
#endif

    loop_actuator();
    /*******
      Here is a good place to add code that could take up
      to a millisecond or so to run.
    *******/
  }// bUseFake == false;

  //------update ticker
  bool bMicroLED_touch = false;
#ifdef USE_FINGER
  if (isTouched == true) bMicroLED_touch = true;
#else
  if (handsOn == true) bMicroLED_touch = true;
#endif

  if (millis() - tickTimer > 1000) {
    tickTimer = millis();
    tickerSend();
    alive_cnt++;
    //    if (alive_cnt == 2) {
    //      alive_cnt = 0;
    //      micro_state = !micro_state;
    //      digitalWrite(MICRO_PIN, micro_state);
    //    }
  }


  if (millis() - tickTimer2 > 200) {
    tickTimer2 = millis();

    alive_cnt++;
    int blinkMod = 3;
    if (bMicroLED_touch == true) {
      blinkMod = 1;
    }
    if (alive_cnt % blinkMod == 0) {
      micro_state = !micro_state;
      digitalWrite(MICRO_PIN, micro_state);
    }
    if (alive_cnt == 24) {
      alive_cnt = 0;
      Serial.println("alive");
    }
  }

}


