
//https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/using-arduino-ide

/*---------------------------------------------------------------------------------------------

  Open Sound Control (OSC) library for the ESP8266

  Example for sending messages from the ESP8266 to a remote computer
  The example is sending "hello, osc." to the address "/test".

  This example code is in the public domain.
  //https://github.com/stahlnow/OSCLib-for-ESP8266
  //https://www.sparkfun.com/products/13711
  //https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/installing-the-esp8266-arduino-addon?_ga=2.154687355.2110226771.1521656366-1623107573.1508894841
  --------------------------------------------------------------------------------------------- */


unsigned long lastPing;

void setup_osc() {
  Serial.println();
  Serial.println();

#ifdef USE_WIFI
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Serial.println(pass);

  //   WiFi.mode(WIFI_AP); //WIFI_STA
  WiFi.begin(ssid, pass); //, inIP);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("send sample and BPM");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());

#else
  Serial.println("Starting Network using DHCP.");
  Serial.print("Teensy's MAC is ");
  Serial.println(myMac);

  delay(500);

  // [WARNING] ethernet module "WIZ850io" specific code:
  // reset pulse to OSC_RESET pin 9 - resets the WIZ850io module
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  digitalWrite(9, HIGH);

  IPAddress ip(188, 0, 1, 0 + deviceId); //replace 0 + with the Device ID

  delay(500);

  Serial.print("deviceId: ");
  Serial.println(deviceId);
  Serial.print("ip: ");
  Serial.println(ip);
  Serial.print("out port: ");
  Serial.println(outPort);
  Serial.print("local port: ");
  Serial.println(localPort);
  Serial.print("MAC addr ");
  Serial.println(myMac);
  //setup ethernet part
  Ethernet.begin(myMac, ip);

  if (Udp.begin(localPort)) Serial.println("Succesful UDP connection");
  else Serial.println("there are no sockets available to use");

  delay(200);
#endif





  lastPing = millis();
}

void setPing(OSCMessage &msg) {
  //  int requestedDeviceId = msg.getInt(0);
  //  if (requestedDeviceId == -1) {
  //   int sensorId = msg.getInt(1);
  if (bDebug) {
    Serial.print(millis() - lastPing);
    Serial.print(" ms , ");
    Serial.println("/ping: ");
  }
  lastPing = millis();
  //  }
}

void setDebug(OSCMessage &msg) {

  int requestedDeviceId = msg.getInt(0);

  if (requestedDeviceId == deviceId) {
    //   int sensorId = msg.getInt(1);
    bDebug = msg.getInt(2);

    Serial.print("/debug: ");
    Serial.println(bDebug);

  }
}

void setForceSolenoid(OSCMessage &msg) {

  int requestedDeviceId = msg.getInt(0);

  if (requestedDeviceId == deviceId) {
    //   int sensorId = msg.getInt(1);
    forceSolenoid = msg.getInt(2);

    Serial.print("/forceS: ");
    Serial.println(forceSolenoid);
    forceTimer = millis();
  }
}


//void setReset(OSCMessage &msg) {
//  int requestedDeviceId = msg.getInt(0);
//  if (requestedDeviceId == -1) {
//    Serial.println("/osc_reset request: ");
//    setup_osc();
//  }
//}

//bundle not good for HUZZAH but good for teensy
//void checkOSC_inputBundle() {
//  OSCBundle bundle;
//  int size = Udp.parsePacket();
//
//  if (size > 0) {
//    while (size--) {
//      bundle.fill(Udp.read());
//    }
//    if (!bundle.hasError()) {
//
//      bundle.dispatch("/thres", setThreshold);
//
//    } else {
//      error = bundle.getError();
//      Serial.print("error: ");
//      Serial.println(error);
//    }
//  }
//}

void checkOSC_inputMsg() {
  //when using the OSC receive code on feather HUZZAH it needs to be send as message and not bundle from openframeworks
  //if used on teensy use/expect bundle

  //https://github.com/CNMAT/OSC/issues/57
  OSCMessage msg;
  int size = Udp.parsePacket();

  if (size > 0) {
    while (size--) {
      msg.fill(Udp.read());
    }
    if (!msg.hasError()) {
      msg.dispatch("/thres", setThreshold);
      msg.dispatch("/touchThr", setTouchThreshold);
      msg.dispatch("/ping", setPing);
      msg.dispatch("/onTimeP", setOnTimePrimary);
      msg.dispatch("/onTimeS", setOnTimeSecondary);
      msg.dispatch("/forceS", setForceSolenoid);
      msg.dispatch("/debug", setDebug);
      msg.empty();
    } else {
      error = msg.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }
}


void setThreshold(OSCMessage &msg) {
  //the finger sensor library has a threshol;d value that can be set
  //the computer GUI can set this value

#ifdef USE_FINGER
  int requestedDeviceId = msg.getInt(0);

  //  Serial.print("requestedDeviceId ");
  //  Serial.println(requestedDeviceId);

  if (requestedDeviceId == deviceId) {
    //   int sensorId = msg.getInt(1);
    THRESHOLD = msg.getInt(2);
    pulseSensor.setThreshold(THRESHOLD);
    if (bDebug) {
      Serial.print("/thres: ");
      Serial.println(THRESHOLD);
    }
  }
#endif
}

void setTouchThreshold(OSCMessage &msg) {
  //the touch sensor has different threshold value depending on the wire length + etc
  //the computer GUI can set this value

#ifdef USE_FINGER
  int requestedDeviceId = msg.getInt(0);

  //  Serial.print("requestedDeviceId ");
  //  Serial.println(requestedDeviceId);

  if (requestedDeviceId == deviceId) {
    //   int sensorId = msg.getInt(1);
    touchThreshold = msg.getInt(2);
    if (bDebug) {
      Serial.print("/touchThres: ");
      Serial.println(touchThreshold);
    }
  }
#endif
}

void setOnTimePrimary(OSCMessage &msg) {
  int requestedDeviceId = msg.getInt(0);

  if (requestedDeviceId == deviceId) {
    onTimePrimary = msg.getInt(2);
    if (bDebug) {
      Serial.print("/onTimeP: ");
      Serial.println(onTimePrimary);
    }
  }
}
void setOnTimeSecondary(OSCMessage &msg) {
  int requestedDeviceId = msg.getInt(0);

  if (requestedDeviceId == deviceId) {
    onTimeSecondary = msg.getInt(2);
    if (bDebug) {
      Serial.print("/onTimeS: ");
      Serial.println(onTimeSecondary);
    }
  }
}

void loop_osc() {


  //  checkOSC_inputBundle();
  checkOSC_inputMsg();

  if (millis() - lastPing > 60000) {
    //    lastPing = millis();
    //    WiFi.stop();

    Serial.print(millis() - lastPing);
    Serial.print(" , ");
    Serial.println("call setup again because no new OSC for x ms"); //-----BUT NOT");
    setup_osc();
  }
}


void tickerSend() {
  //every so often the micro controller sends a ticker signal to computer
  //so computer nows micro is still on the network
  //  Serial.println("tickerSend");
  //  OSCMessage msg("/ticker");

  //  OSCBundle msg;
  //  msg.add("/ticker").add(deviceId);
  OSCMessage msg("/ticker");
  msg.add(deviceId);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();
  //    delay(500);
}

void sampleSend(int _sensorSample) {

  //the finger sensor reads an analog pin signal.
  //this value gets send to the computer

  // Serial.println ("k ");
  //  if (_forceValue == -1) {
  //    sensorSample = pulseSensor.getLatestSample(); // analogRead(PULSE_INPUT);  // read input value
  //  } else {
  //    sensorSample = _forceValue;
  //  }
  //  Serial.println (sample);

  OSCMessage msg("/sample");
  msg.add(deviceId);
  msg.add(_sensorSample);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();

}

void comboSend(int _trough, int _sensorSample, int _peak) {

  OSCMessage msg("/combo");
  msg.add(deviceId);
  msg.add(_trough);
  msg.add(_sensorSample);
  msg.add(_peak);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();

}
void bpmSend(int _BPM) {

  //the finger sensor library or the hand sensor readings are send to the computer as BPM

  //Serial.println ("BPM ");
  //  if (_forceValue == -1) {
  //    BPM = pulseSensor.getBeatsPerMinute();
  //  } else {
  //    BPM = _forceValue;
  //  }
  //  Serial.println (BPM);
  OSCMessage msg("/bpm");
  msg.add(deviceId);
  msg.add(_BPM);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();

}

void bpm2Send(int _BPM) {

  //the custom reading analysiz BPM

  //Serial.println ("BPM ");
  //  if (_forceValue == -1) {
  //    BPM = pulseSensor.getBeatsPerMinute();
  //  } else {
  //    BPM = _forceValue;
  //  }
  //  Serial.println (BPM);
  OSCMessage msg("/bpm2");
  msg.add(deviceId);
  msg.add(_BPM);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();

}

void sensorTypeSend(String _type) {

  OSCMessage msg("/type");
  msg.add(deviceId);
  if (_type == "Fing") msg.add("Fing");
  else if (_type == "Hand")  msg.add("Hand");
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();

}

void insideSend(bool _inside) {

  OSCMessage msg("/inside");
  msg.add(deviceId);
  msg.add(_inside);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();

}

void touchReadSend(int _touchReading, int _touchAverage) {

  OSCMessage msg("/touchRead");
  msg.add(deviceId);
  msg.add(_touchReading);
  msg.add(_touchAverage);
  Udp.beginPacket(outIp, outPort);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();

}

