unsigned long touchPrint_timer;

void setup_touchSensor() {

}

void check_touchSensor() {
  int r1 = touchRead(TOUCH_PIN);

  if (r1 < touchThreshold) isTouched = false;
  else isTouched = true;

  //    Serial.print("touch read ");
  //    Serial.print(r1);
  //    Serial.println();

  if (isTouched != old_isTouched) {
    old_isTouched = isTouched;
    insideSend(isTouched);
    //    Serial.print("isTouched ");
    //    Serial.println(isTouched);

    if (isTouched == false) {
      set_actuatorBPM(-1);
    }
  }

  if (bDebug) {
    if (millis()  - touchPrint_timer > 1000) {
      touchReadSend(r1);
      touchPrint_timer = millis();
      Serial.print("touchVal ");
      Serial.print(r1);
      Serial.print(" / ");
      Serial.print(touchThreshold);
      Serial.println();
      
    }
  }
}

