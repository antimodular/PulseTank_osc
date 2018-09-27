int OFF = 1;
int ON = 0;

bool onOff_state;
unsigned long dutyTimer;

unsigned long actuationTimer;
unsigned long stageDuration;
int pulseStage;

int new_BPM_duration;
int BPM_duration;
int old_BPM_duration;

int new_primaryDuration;
int primaryDuration; // = BPM_duration / 4;
int new_secondaryDuration;
int secondaryDuration; // = BPM_duration - primaryDuration;

bool gotNew_actuation;

//int onTimePrimary = 30; //20; //30;
//int onTimeSecondary = 30; //20; //30;

bool useAnalog = false;

int new_offTimePrimary;
int offTimePrimary; // = primaryDuration - onTimePrimary;
int new_offTimeSecondary;
int offTimeSecondary; // = secondaryDuration - onTimeSecondary;

void setup_actuator() {
  pinMode(actuator0_pin, OUTPUT);

  onOff_state = 0;
  pulseStage = 1;
  stageDuration = onTimePrimary;
  actuationTimer = millis();
  set_actuatorBPM(-1, 0);
}

void set_actuatorBPM(int _BPM, int _whoSent) {

  if (_BPM == -1) {
    new_BPM_duration = 0;
  } else {
    new_BPM_duration = 60000 / _BPM;
    new_primaryDuration = new_BPM_duration / 3;
    new_secondaryDuration = new_BPM_duration - new_primaryDuration;

    new_offTimePrimary = new_primaryDuration - onTimePrimary;
    new_offTimeSecondary = new_secondaryDuration - onTimeSecondary;
  }
  gotNew_actuation = false;
  if (old_BPM_duration != new_BPM_duration) {

    gotNew_actuation = true;
    Serial.print("receive set_actuatorBPM: ");
    Serial.print(_BPM);
    Serial.print(" new_BPM_duration ");
    Serial.print(new_BPM_duration);
    Serial.print(" _whoSent "); // 0 = setup, 1 = finger, 2 = hands, 3 = touch
    Serial.print(_whoSent);

    Serial.println();


  }
  old_BPM_duration = new_BPM_duration;
}

void applyNewValues() {

  Serial.println("apply new_actuatorBPM: ");

  BPM_duration = new_BPM_duration;
  primaryDuration = new_primaryDuration;
  secondaryDuration = new_secondaryDuration;

  offTimePrimary = new_offTimePrimary;
  offTimeSecondary = new_offTimeSecondary;
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
    //computer GUI can force solenoid to actuate
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

          if (gotNew_actuation == true) {
            gotNew_actuation = false;
            applyNewValues();
          }

          stageDuration = onTimePrimary;
          allOn(stageDuration);
        }
      }

    } else {
      if (gotNew_actuation == true) {
        gotNew_actuation = false;
        applyNewValues();
      }
      allOff(0);
    }//end else if (BPM_duration > 0  && (isTouched == true || handsOn == true))
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


