#ifdef USE_HANDS
//#include <elapsedMillis.h>

unsigned long typeTimer;

void setup_handsSensor() {

  Serial.println("setup hands sensor");

  pinMode (HAND_INPUT, INPUT);
  pinMode (PULSE_BLINK, OUTPUT);


  signalType = 0;
  handsOn = false;
}

//try to put as little as possible inside the interrupt function and do sending and digitelwrite outside of it.
//really just use it to measure timing

void handInput_interrupt() {
  old_cnt = new_cnt;
  new_cnt++;
  pulse = digitalRead(HAND_INPUT);

  //  if (pulse != lastPulse) {
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
      //        Serial.println();
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


  //    lastPulse = pulse;
  //  }//end if (pulse != lastPulse)
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
  if (old_BPM != new_BPM) {
    old_BPM = new_BPM;
    BPM = new_BPM;
    bpmSend(BPM);
    set_actuatorBPM(BPM);
  }
  if (old_handsOn != new_handsOn) {
    old_handsOn = new_handsOn;
    handsOn = new_handsOn;
    insideSend(handsOn);
  }
  pulse = digitalRead(HAND_INPUT);


}

void loop_handsSensor2() {

  if (millis() - typeTimer > 2000) {
    typeTimer = millis();
    sensorTypeSend("Hand");
  }

  //  pulse = digitalRead(HAND_INPUT);

  if (pulse != lastPulse) {
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
        BPM = 60000 / low_duration;
        //        if (bDebug) {
        Serial.print ("dur ");
        Serial.print (low_duration);
        Serial.print (" / BPM ");
        Serial.print (BPM);
        Serial.println();
        //        }
        bpmSend(BPM);
        set_actuatorBPM(BPM);
        //        Serial.println();
      }
    } else {
      digitalWrite(PULSE_BLINK, HIGH);
      //pulse == LOW
      low_duration = 0;

      if (high_duration > 40 && high_duration < 52)
      {
        //50 ms
        signalType = 1;
        Serial.print (" hands on ");
        Serial.println (high_duration);
        handsOn = true;
        insideSend(handsOn);
      }
      else if (high_duration > 18 && high_duration < 22)
      {
        //20 ms
        signalType = 2;
        Serial.print (" type pulse ");
        Serial.print (high_duration);
        Serial.println();
      }
      else //if(high_duration >= 70)
      {
        //70 ms
        signalType = 3;
        Serial.print (" hands off ");
        Serial.println (high_duration);
        handsOn = false;
        set_actuatorBPM(-1);
        insideSend(handsOn);
      }
    } //end pulse == LOW


    lastPulse = pulse;
  }//end if (pulse != lastPulse)
}

#endif
