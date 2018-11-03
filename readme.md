## Information
This is code to lets a teensy 3.2 read the heart rate of a participant.
We use two different sensors. 
A pulse sensor from https://pulsesensor.com is use to measure the heart's pulse by placing a finger on it. 
The Polar heart rate sensor (NANO 5G PSL20) is used to also measure the heart rate by placing two hands on a hand interface.

The #define USE_FINGER or #define USE_HANDS will enable the reading of the specific sensor.

To improve the interactive experience of the participant we added a capacity touch sensing element. This helps to know as soon as someone touches the sensor.


![](https://github.com/antimodular/PulseTank_osc/blob/feature_backAtStudio/images/finger%20sensor%20instruction-01%20arrows.jpg)
![](https://github.com/antimodular/PulseTank_osc/blob/feature_backAtStudio/images/hand%20sensor%20electronics.jpg)
![](https://github.com/antimodular/PulseTank_osc/blob/feature_backAtStudio/images/hand_sensor.jpg)

