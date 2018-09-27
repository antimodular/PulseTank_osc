int dipAmt = 5;
int dipPins[] = {A5,A4,A3,A2,A1}; // 19, 20, 14, 15, 16, 17}; //DIP Switch Pins
byte ip_ = 0; //default will be changed once DIP switch is read
byte temp_ip = -1;
int startAddress = 0;

void init_dipSwitch_IP();
void readDipSwitch();
byte address();
byte getIP();

void init_dipSwitch_IP() {

  for (int i = 0; i < dipAmt; i++) {
    pinMode(dipPins[i], INPUT_PULLUP);  // sets the digital pin 28-33 as input
  }
  readDipSwitch();
}

void readDipSwitch() {
  ip_ = address();
  if (temp_ip != ip_) {
    if (bDebug) {
      //Serial.print("ip DIP switch = ");
      //Serial.println(ip_,BIN);
      Serial.print("controller's ip = ");
      Serial.println(ip_, DEC);
    }

    temp_ip = ip_;
  }
}

byte address() {
  int i, j = 0;
  //Get the switches state
  for (i = 0; i < dipAmt; i++) {
    Serial.println(digitalReadFast(dipPins[i]));
//        j = (j << 1) | !digitalReadFast(dipPins[i]);   // read the input pin. ! turns true in to false and vis versa
    j = (j << 1) | digitalReadFast(dipPins[i]);
  }
  return j; //return address
}

byte getIP() {
  return ip_;
}
