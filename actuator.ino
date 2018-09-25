int OFF = 1;
int ON = 0;

bool onOff_state;
unsigned long dutyTimer;

unsigned long actuationTimer;
unsigned long stageDuration;
int pulseStage;

int BPM_duration;
int old_BPM_duration;

int primaryDuration; // = BPM_duration / 4;
int secondaryDuration; // = BPM_duration - primaryDuration;

//int onTimePrimary = 30; //20; //30;
//int onTimeSecondary = 30; //20; //30;

bool useAnalog = false;

int offTimePrimary; // = primaryDuration - onTimePrimary;
int offTimeSecondary; // = secondaryDuration - onTimeSecondary;

void setup_actuator() {
  pinMode(actuator0_pin, OUTPUT);

  onOff_state = 0;
  pulseStage = 1;
  stageDuration = onTimePrimary;
  actuationTimer = millis();
  set_actuatorBPM(-1);
}

void set_actuatorBPM(int _BPM) {

  if (_BPM == -1) {
    BPM_duration = 0;
  } else {
    BPM_duration = 60000 / _BPM;
    primaryDuration = BPM_duration / 3;
    secondaryDuration = BPM_duration - primaryDuration;

    offTimePrimary = primaryDuration - onTimePrimary;
    offTimeSecondary = secondaryDuration - onTimeSecondary;
  }

  if (old_BPM_duration != BPM_duration) {
    Serial.print("set_actuatorBPM: ");
    Serial.print(_BPM);
    Serial.print(" BPM_duration ");
    Serial.print(BPM_duration);
    Serial.println();
  }
  old_BPM_duration = BPM_duration;
}


void loop_actuator() {
  //
  //      Serial.print("BPM_duration ");
  //      Serial.print(BPM_duration);
  //      Serial.print(" stage ");
  //      Serial.print(pulseStage);
  //      Serial.print(" stageDuration ");
  //      Serial.print(stageDuration);
  //      Serial.println();
  //  if (millis() < 4000) BPM_duration = -1;
  //  BPM_duration = 800;
  //  primaryDuration = BPM_duration / 4;
  //  secondaryDuration = BPM_duration - primaryDuration;
  //
  //  offTimePrimary = primaryDuration - onTimePrimary;
  //  offTimeSecondary = secondaryDuration - onTimeSecondary;

  if (forceSolenoid == true) {
    if (millis() - forceTimer < onTimePrimary) {
      allOn(0);
    } else {
      allOff(0);
      forceSolenoid = false;
    }
  } else {
    if (BPM_duration > 0  && (isTouched == true || handsOn == true)) {
      if (pulseStage == 1) {
        //primary beat on
        //      allOn(stageDuration);
        if (millis() - actuationTimer >= stageDuration) {
          if (bDebug) {
            Serial.print(" stage 2 ");
            Serial.print(stageDuration);
            Serial.println();
          }
          actuationTimer = millis();
          pulseStage = 2;
          stageDuration = offTimePrimary;
          allOff(stageDuration);
        }
      }
      if (pulseStage == 2) {
        //primary beat off
        //      allOff(stageDuration);
        if (millis() - actuationTimer >= stageDuration) {
          if (bDebug) {
            Serial.print(" stage 3 ");
            Serial.print(stageDuration);
            Serial.println();
          }
          actuationTimer = millis();
          pulseStage = 3;
          stageDuration = onTimeSecondary;
          allOn(stageDuration);
        }
      }
      if (pulseStage == 3) {
        //secondary beat on
        //      allOn(stageDuration);
        if (millis() - actuationTimer >= stageDuration) {
          if (bDebug) {
            Serial.print(" stage 4 ");
            Serial.print(stageDuration);
            Serial.println();
          }
          actuationTimer = millis();
          pulseStage = 4;
          stageDuration = offTimeSecondary;
          allOff(stageDuration);
        }
      }
      if (pulseStage == 4) {
        //secondary beat off
        //      allOff(stageDuration);
        if (millis() - actuationTimer >= stageDuration) {
          if (bDebug) {
            Serial.print(" stage 1 ");
            Serial.print(stageDuration);
            Serial.println();
          }
          actuationTimer = millis();
          pulseStage = 1;
          stageDuration = onTimePrimary;
          allOn(stageDuration);
        }
      }

    } else {
      allOff(0);
    }//end else if BPM_duration == 0
  }//end else if (forceSolenoid == true)
}


void allOn(int _duration) {
  //_duration does not do anything.
  
  //    Serial.print("allOn ");
  //    Serial.print(_duration);
  //    Serial.println();

  onOff_state = 1;
  int duty;
  if (onOff_state == 0) {
    //first time around give fully power
    duty = 1023;
    dutyTimer = millis();
  }
  if (millis() - dutyTimer > 30) {
    //later give partical power
    duty = 600;
  }
  //  digitalWrite(led0, ON);   // turn the LED on (HIGH is the voltage level)
  if (useAnalog) {
    duty = 1023;
    analogWrite(actuator0_pin, duty);
    //    analogWrite(actutatorPin1, duty);
  } else {
    digitalWrite(actuator0_pin, 1);
    //    Serial.print("1");
    //    digitalWrite(actutatorPin1, 1);
  }
}

void allOff(int _duration) {
  //_duration does not do anything.
  //  Serial.print("allOff ");
  onOff_state = 0;
  //  digitalWrite(led0, OFF);    // turn the LED off by making the voltage LOW
  if (useAnalog) {
    analogWrite(actuator0_pin, 0);
    //    analogWrite(actutatorPin1, 0);
  } else {
    digitalWrite(actuator0_pin, 0);
    //    Serial.print("0");
    //    digitalWrite(actutatorPin1, 0);
  }

}


