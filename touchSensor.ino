unsigned long touchPrint_timer;

float touch_alpha = 0.9;

long touchValue;
long raw_touchValue;
long old_touchValue;

int touchAverage;

//unsigned long touch_onTimer; //touch_onHysteresis;
unsigned long touch_offTimer;
unsigned long onHysteresis = 20;
unsigned long offHysteresis = 20;

void setup_touchSensor() {

}

void check_touchSensor() {


  old_touchValue = touchValue;
  raw_touchValue = touchRead(TOUCH_PIN);
  if (raw_touchValue < 0) raw_touchValue = 1;

  //create running average of touch value
  //the larger touch_alpha the greater the smoothing
  //the smaller touch_alpha the closer we get to the raw value
  touchValue = touch_alpha * old_touchValue + (1 - touch_alpha) * raw_touchValue;

  touchValue = constrain(touchValue,0, 65532); //constrain the running average. not quite to maxx int so that later i can send out info about the actual value over osc

  if (touchValue < touchThreshold) {
    //not touching
    if (millis() - touch_offTimer > offHysteresis) isTouched = false;
    touch_onTimer = millis();
  } else {
    //touching
    if (millis() - touch_onTimer > onHysteresis) isTouched = true;
    touch_offTimer = millis();
  }
  //  if (touchValue < touchThreshold) isTouched = false;
  //  else isTouched = true;

  //    Serial.print("touch read ");
  //    Serial.print(r1);
  //    Serial.println();

  if (isTouched != old_isTouched) {
    old_isTouched = isTouched;
    insideSend(isTouched); //finger is inside. send that state to computer
    //    Serial.print("isTouched ");
    //    Serial.println(isTouched);

    if (isTouched == false) {
      set_actuatorBPM(-1, 3);
      bpmSend(-1);
      DEFAULT_BPM = random(DEFAULT_BPM_min, DEFAULT_BPM_max);
    }
  }

  //  float touchAvg_alpha = 0.99;
  //  if (raw_touchValue < 6000) {
  //    touchAverage = touchAvg_alpha * touchAverage + (1 - touchAvg_alpha) * raw_touchValue;
  ////    touchThreshold = touchAverage;
  //  }

#ifdef USE_FINGER
  if (bDebug) {
    if (millis()  - touchPrint_timer > 1000) {

      //to help troubleshoot sound out 1234 if actual touchValue was bad
      if (touchValue >= 0 && touchValue < 65530) touchReadSend(touchValue, 0); //touchAverage);
      else  touchReadSend(1234, 0);

      touchPrint_timer = millis();
      Serial.print("touchVal ");
      Serial.print(touchValue);
      Serial.print(" / ");
      Serial.print(touchThreshold);
      Serial.println();

    }
  }
#endif
}

