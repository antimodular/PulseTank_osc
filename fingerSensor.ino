#ifdef USE_FINGER

unsigned long typeTimer;
//bool isInside, old_isInside;

unsigned long fingerInTimer;
unsigned long fingerOutTimer;

int old_latestSample;
unsigned long tempTimer;

void setup_fingerSensor() {

  Serial.println("setup finger sensor");
  // Configure the PulseSensor manager.
  pulseSensor.analogInput(PULSE_INPUT);
  pulseSensor.blinkOnPulse(PULSE_BLINK);
  pulseSensor.fadeOnPulse(PULSE_FADE);

  pulseSensor.setSerial(Serial);
  pulseSensor.setOutputType(OUTPUT_TYPE); //only needed for debugging
  pulseSensor.setThreshold(THRESHOLD);

  // Skip the first SAMPLES_PER_SERIAL_SAMPLE in the loop().
  samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;

  //  isInside = false;
  //  old_isInside = true;

  // Now that everything is ready, start reading the PulseSensor signal.
  if (!pulseSensor.begin()) {
    /*
       PulseSensor initialization failed,
       likely because our Arduino platform interrupts
       aren't supported yet.

       If your Sketch hangs here, try changing USE_PS_INTERRUPT to false.
    */
    for (;;) {
      // Flash the led to show things didn't work.
      digitalWrite(PULSE_BLINK, LOW);
      delay(50);
      digitalWrite(PULSE_BLINK, HIGH);
      delay(100);
    }
  }

}


void loop_fingerSensor() {
  if (millis() - typeTimer > 2000) {
    typeTimer = millis();
    sensorTypeSend("Fing");
  }

  if (pulseSensor.sawNewSample()) {
    /*
       Every so often, send the latest Sample via serial or OSC.
       We don't print every sample, because our baud rate won't support that much I/O.
    */
    if (--samplesUntilReport == (byte) 0) {
      samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
      int latestSample = pulseSensor.getLatestSample();

      if (QS_interval == true) {                      // Quantified Self flag is true when arduino finds a heartbeat
        bpm2Send(BPM_interval);
        //    sendDataToProcessing('B', BPM);  // send heart rate with a 'B' prefix
        //    sendDataToProcessing('Q', IBI);  // send time between beats with a 'Q' prefix
        QS_interval = false;                      // reset the Quantified Self flag for next time
      }

      //      sampleSend(latestSample);
      //       comboSend(pulseSensor.getPulseTrough(), tempBPM, pulseSensor.getPulsePeak());

      comboSend(pulseSensor.getPulseTrough(), latestSample, pulseSensor.getPulsePeak());
      //      Serial.print("getPulseAmplitude = ");
      //      Serial.print(pulseSensor.getPulseTrough());
      //      Serial.print("\t");
      //      Serial.print(pulseSensor.getPulseAmplitude());
      //      Serial.print("\t");
      //      Serial.print(pulseSensor.getPulsePeak());
      //      Serial.println();
      /*
            if (latestSample < fingerInThreshold) {
              fingerOutTimer = millis();
              if (isInside == false && millis() - fingerInTimer > 500) {
                isInside = true;
              }
            } else {
              fingerInTimer = millis();
              if (isInside == true && millis() - fingerOutTimer > 100) {
                isInside = false;
              }
            }

            Serial.print("isInside = ");
            Serial.print(isInside);
            Serial.print(" , inTimer ");
            Serial.print(millis() - fingerInTimer);
            Serial.print(" , outTimer ");
            Serial.print(millis() - fingerOutTimer);
            Serial.println();

            if (isInside != old_isInside) {
              old_isInside = isInside;
              insideSend(isInside);
              Serial.print("isInside ");
              Serial.println(isInside);
            }
      */
      //      pulseSensor.outputSample(); //for debugging via serial port

      //            Serial.print("getPulseAmplitude ");
      //      Serial.println(getPulseAmplitude);

      /*
         At about the beginning of every heartbeat,
         report the heart rate and inter-beat-interval.
      */

      bool isTouched_longTime = false;
      if (millis() - touch_onTimer > maxWaitTime) isTouched_longTime = true;

      if (pulseSensor.sawStartOfBeat() || isTouched_longTime == true) {

        BPM = pulseSensor.getBeatsPerMinute();

        if (BPM < 40 || BPM > 120) BPM = 119; //73;

        //        if (BPM > 55 && BPM < 130) {
        if (isTouched == false) {
          //in case we see sensor readings that might look like a heart beat but no finger touching was detected, we want to ignor that reading
          bpmSend(-1);
        } else {

          //
          bpmSend(BPM);
          //          int BPM2 = 60000 / BPM_interval;
          int abs_interval = abs(73 - BPM_interval);
          int abs_bpm = abs(73 - BPM);


          //          Serial.print("abs_bpm ");
          //          Serial.print(abs_bpm);
          //          Serial.print(" abs_interval ");
          //          Serial.print(abs_interval);
          //          Serial.println();

          if ( abs_interval < abs_bpm ) {
            set_actuatorBPM(BPM_interval, 1);
          } else {
            set_actuatorBPM(BPM, 1);
          }

        }//end else if (isTouched == false)
        //        }
//        if (bDebug) {
//          pulseSensor.outputBeat(); //for debugging via serial port
//        }
      }//end  if (pulseSensor.sawStartOfBeat())

      // Serial.print("inside ");
      //        Serial.println( pulseSensor.isInsideBeat());




    }//end  if (--samplesUntilReport == (byte) 0)

  }// if (pulseSensor.sawNewSample())

  //
}

#endif

