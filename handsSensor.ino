#ifdef USE_HANDS
//#include <elapsedMillis.h>

unsigned long typeTimer;
unsigned long hands_onTimer = 0;
unsigned long hands_offTimer;

void setup_handsSensor() {

  Serial.println("setup hands sensor");

  pinMode (HAND_INPUT, INPUT);
  pinMode (PULSE_BLINK, OUTPUT);


  signalType = 0;
  handsOn = false;
}

//try to put as little as possible inside the interrupt function and do sending and digitelwrite outside of it.
//really just use it to measure timing of pulse coming from polar heart rate sensor
void handInput_interrupt() {
  old_cnt = new_cnt;
  new_cnt++;
  //   pulse = digitalRead(HAND_INPUT);
  pulse = digitalReadFast(HAND_INPUT);

  //    Serial.print (" ");
  //    Serial.print (pulse);
  //    Serial.print (" / ");
  //    Serial.println (lastPulse);
  //    duration = millis() - lastTimer;
  //    Serial.println(duration);
  //    lastTimer = millis();

  //    Serial.print ("periode ");
  //    Serial.println(pulsePeriode);
  //
  //    Serial.print ("width ");
  //    Serial.println(pulseWidth);


  if (pulse == HIGH) {
    //pulse == HIGH
    high_duration = 0;
    digitalWrite(PULSE_BLINK, LOW);

    if (signalType == 2) {
      //      old_BPM = new_BPM;
      new_BPM = 60000 / low_duration;
      if (bDebug) {
        Serial.print ("dur ");
        Serial.print (low_duration);
        Serial.print (" / new_BPM ");
        Serial.print (new_BPM);
        Serial.println();
      }
      //      bpmSend(new_BPM);
      //      set_actuatorBPM(new_BPM);
    }
  } else {
    digitalWrite(PULSE_BLINK, HIGH);
    //pulse == LOW
    low_duration = 0;

    if (high_duration > 40 && high_duration < 52)
    {
      //50 ms
      signalType = 1;
      if (bDebug) {
        Serial.print (" hands on ");
        Serial.println (high_duration);
      }
      new_handsOn = true;
      //      insideSend(new_handsOn);
    }
    else if (high_duration > 18 && high_duration < 22)
    {
      //20 ms
      signalType = 2;
      if (bDebug) {
        Serial.print (" type pulse ");
        Serial.print (high_duration);
        Serial.println();
      }
    }
    else //if(high_duration >= 70)
    {
      //70 ms
      signalType = 3;
      if (bDebug) {
        Serial.print (" hands off ");
        Serial.println (high_duration);
      }
      new_handsOn = false;
      new_BPM = -1;
      //      set_actuatorBPM(new_BPM);
      //      insideSend(new_handsOn);
    }
  } //end pulse == LOW
}

void loop_handsSensor() {

  if (millis() - typeTimer > 2000) {
    typeTimer = millis();
    sensorTypeSend("Hand");
  }

  if (old_cnt != new_cnt) {
    old_cnt = new_cnt;

    //    Serial.print("old_BPM ");
    //    Serial.print(old_BPM);
    //    Serial.print(" new_BPM ");
    //    Serial.print(new_BPM);
    //    Serial.print("       ");
    //
    //    Serial.print("old_handsOn ");
    //    Serial.print(old_handsOn);
    //    Serial.print(" new_handsOn ");
    //    Serial.print(new_handsOn);
    //    Serial.print("       ");
    //
    //    Serial.print(" new_cnt ");
    //    Serial.print(new_cnt);
    //    Serial.print(" signalType ");
    //    Serial.print(signalType);
    //    Serial.println();
  }

  if (old_handsOn != new_handsOn) {
    old_handsOn = new_handsOn;
    handsOn = new_handsOn;
    insideSend(handsOn);
    //    if (handsOn == false) new_BPM = -1;
  }

  if (handsOn == false) hands_onTimer = millis();
  else  hands_offTimer = millis();

  bool isTouched_longTime = false;
  if (millis() - hands_onTimer > maxWaitTime) isTouched_longTime = true;

  if (old_BPM != new_BPM || isTouched_longTime == true) {

    //    if (new_BPM != -1 && (new_BPM < 40 || new_BPM > 120) ) new_BPM = random(97, 99);
    //    if (isTouched_longTime == true && (new_BPM < 40 || new_BPM > 120)) new_BPM = 99;
    //    if (isTouched_longTime == false && (new_BPM < 40 || new_BPM > 120)) new_BPM = 100;

    if (new_BPM < 40 || new_BPM > 120) new_BPM = 73;

    if (handsOn == false) {
      new_BPM = -1;
    }

    if (old_BPM != new_BPM) {
      BPM = new_BPM;
      bpmSend(BPM);
      set_actuatorBPM(BPM, 2);
    }

    old_BPM = new_BPM;
  }


  //  pulse = digitalRead(HAND_INPUT);


}

#endif
