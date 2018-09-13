#ifdef USE_HANDS
#include <elapsedMillis.h>

unsigned long typeTimer;

void setup_handsSensor() {

  Serial.println("setup hands sensor");

  pinMode (HAND_INPUT, INPUT);
  pinMode (PULSE_BLINK, OUTPUT);


  signalType = 0;

}

void loop_handsSensor() {

  if (millis() - typeTimer > 2000) {
    typeTimer = millis();
    sensorTypeSend("Hand");
  }

  pulse = digitalRead(PULSE_INPUT);

  if (pulse != lastPulse) {

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


    if (pulse == HIGH)
    {
      //pulse == HIGH
      high_duration = 0;
      digitalWrite(PULSE_BLINK, LOW);

      if (signalType == 2) {
        Serial.print ("dur ");
        Serial.print (low_duration);
        Serial.print (" / BMP ");
        BPM = 60000 / low_duration;
        Serial.print (BPM);
        bpmSend(BPM);
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
      }
      else if (high_duration > 18 && high_duration < 22)
      {
        //20 ms
        signalType = 2;
        Serial.print (" type pulse ");
        Serial.print (high_duration);
        Serial.println();
      }
      else
      {
        //70 ms
        signalType = 3;
        Serial.print (" hands off ");
        Serial.println (high_duration);
      }
    } //end pulse == LOW


    lastPulse = pulse;
  }//end if (pulse != lastPulse)
}

#endif
