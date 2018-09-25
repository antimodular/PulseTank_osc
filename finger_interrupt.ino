volatile int rate[10];                    // used to hold last ten IBI values
volatile unsigned long sampleCounter = 0;          // used to determine pulse timing
volatile unsigned long lastBeatTime = 0;           // used to find the inter beat interval
volatile int P = 512;                     // used to find peak in pulse wave
volatile int T = 512;                     // used to find trough in pulse wave
volatile int thresh = 512;                // used to find instant moment of heart beat
volatile int amp = 100;                   // used to hold amplitude of pulse waveform
volatile boolean firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
volatile boolean secondBeat = true;       // used to seed rate array so we startup with reasonable BPM


int ledState = LOW;
volatile unsigned long blinkCount = 0; // use volatile for shared variables


//void interruptSetup(){
//  // Initializes Timer2 to throw an interrupt every 2mS.
//  TCCR2A = 0x02;     // DISABLE PWM ON DIGITAL PINS 3 AND 11, AND GO INTO CTC MODE
//  TCCR2B = 0x06;     // DON'T FORCE COMPARE, 256 PRESCALER
//  OCR2A = 0X7C;      // SET THE TOP OF THE COUNT TO 124 FOR 500Hz SAMPLE RATE
//  TIMSK2 = 0x02;     // ENABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
//  sei();             // MAKE SURE GLOBAL INTERRUPTS ARE ENABLED
//}


volatile uint32_t timerCounter0;

void timerCallback0() {
  timerCounter0++;
  //Serial.println(timerCounter0);

  // THIS IS THE TIMER 2 INTERRUPT SERVICE ROUTINE.
  // Timer 2 makes sure that we take a reading every 2 miliseconds
  //ISR(TIMER2_COMPA_vect){                         // triggered when Timer2 counts to 124

  cli();                                      // disable interrupts while we do this
  Signal_interval = analogRead(PULSE_INPUT);              // read the Pulse Sensor
  //    Serial.println(Signal);
  sampleCounter += 2;                         // keep track of the time in mS with this variable
  int N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise

  //  find the peak and trough of the pulse wave
  if (Signal_interval < thresh && N > (IBI_interval / 5) * 3) { // avoid dichrotic noise by waiting 3/5 of last IBI
    if (Signal_interval < T) {                       // T is the trough
      T = Signal_interval;                         // keep track of lowest point in pulse wave
    }
  }

  if (Signal_interval > thresh && Signal_interval > P) {        // thresh condition helps avoid noise
    P = Signal_interval;                             // P is the peak
  }                                        // keep track of highest point in pulse wave

  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
  if (N > 250) {                                  // avoid high frequency noise
    if ( (Signal_interval > thresh) && (Pulse_interval == false) && (N > (IBI_interval / 5) * 3) ) {
      Pulse_interval = true;                               // set the Pulse flag when we think there is a pulse
      //    digitalWrite(blinkPin,HIGH);                // turn on pin 13 LED
      IBI_interval = sampleCounter - lastBeatTime;         // measure time between beats in mS
      lastBeatTime = sampleCounter;               // keep track of time for next pulse

      if (firstBeat) {                       // if it's the first time we found a beat, if firstBeat == TRUE
        firstBeat = false;                 // clear firstBeat flag
        return;                            // IBI value is unreliable so discard it
      }
      if (secondBeat) {                      // if this is the second beat, if secondBeat == TRUE
        secondBeat = false;                 // clear secondBeat flag
        for (int i = 0; i <= 9; i++) {   // seed the running total to get a realisitic BPM at startup
          rate[i] = IBI_interval;
        }
      }

      // keep a running total of the last 10 IBI values
      word runningTotal = 0;                   // clear the runningTotal variable

      for (int i = 0; i <= 8; i++) {          // shift data in the rate array
        rate[i] = rate[i + 1];            // and drop the oldest IBI value
        runningTotal += rate[i];          // add up the 9 oldest IBI values
      }

      rate[9] = IBI_interval;                          // add the latest IBI to the rate array
      runningTotal += rate[9];                // add the latest IBI to runningTotal
      runningTotal /= 10;                     // average the last 10 IBI values
      BPM_interval = 60000 / runningTotal;             // how many beats can fit into a minute? that's BPM!
      QS_interval = true;                              // set Quantified Self flag
      // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }
  }

  if (Signal_interval < thresh && Pulse_interval == true) {    // when the values are going down, the beat is over
    //      digitalWrite(blinkPin,LOW);            // turn off pin 13 LED
    Pulse_interval = false;                         // reset the Pulse flag so we can do it again
    amp = P - T;                           // get amplitude of the pulse wave
    thresh = amp / 2 + T;                  // set thresh at 50% of the amplitude
    P = thresh;                            // reset these for next time
    T = thresh;
  }

  if (N > 2500) {                            // if 2.5 seconds go by without a beat
    thresh = 512;                          // set thresh default
    P = 512;                               // set P default
    T = 512;                               // set T default
    lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date
    firstBeat = true;                      // set these to avoid noise
    secondBeat = true;                     // when we get the heartbeat back
  }

  sei();                                     // enable interrupts when youre done!

}
