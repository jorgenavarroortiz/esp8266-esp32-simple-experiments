#include <TheThingsNetwork.h>

// Set your DevAddr, NwkSKey, AppSKey and the frequency plan
const char *devAddr = "0000012D";
const char *nwkSKey = "00000000000000000000000000000001";
const char *appSKey = "00000000000000000000000000000001";

#include <CayenneLPP.h>
#define CHANNEL_INCR 1
#define CHANNEL_FLAM 2
#define CHANNEL_ALAR 3

#define loraSerial Serial1
#define debugSerial Serial

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);
CayenneLPP lpp(51);

unsigned int counter = 0;

int led = 13;       // define the LED pin
int digitalPin = 2; // KY-026 digital interface
int analogPin = A0; // KY-026 analog interface
int digitalVal;     // digital readings
int analogVal;      // analog readings

void setup()
{
  pinMode(led, OUTPUT);
  pinMode(digitalPin, INPUT);

  loraSerial.begin(57600);
  debugSerial.begin(9600);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000)
    ;

  debugSerial.println("-- PERSONALIZE");
  ttn.personalize(devAddr, nwkSKey, appSKey);

  debugSerial.println("-- STATUS");
  ttn.showStatus();
}

void loop()
{
  debugSerial.println("-- LOOP");

  // Read the digital interface
  digitalVal = digitalRead(digitalPin); 
  if(digitalVal == HIGH) // if flame is detected
  {
    digitalWrite(led, HIGH); // turn ON Arduino's LED
    debugSerial.println("Flame HIGH");
  }
  else
  {
    digitalWrite(led, LOW); // turn OFF Arduino's LED
    debugSerial.println("Flame LOW");
  }

  // Read the analog interface
  analogVal = analogRead(analogPin); // 1023 for no flame, near 0 if flame very near
  Serial.println(analogVal); // print analog value to serial

  // Prepare payload of 1 byte to indicate LED status
  byte payload[1];
  payload[0] = (digitalRead(led) == HIGH) ? 1 : 0;
  
  // Send it off
  //ttn.sendBytes(payload, sizeof(payload));
  if (counter % 10 == 0) {
    //Clear buffer 
    lpp.reset();
    //Write data packets into the buffer 
    lpp.addDigitalInput(CHANNEL_INCR, counter/10);
    lpp.addBarometricPressure(CHANNEL_FLAM, analogVal);
    lpp.addDigitalInput(CHANNEL_ALAR, digitalVal);

    ttn.sendBytes(lpp.getBuffer(), lpp.getSize());
  }

  counter++;
  delay(1000);
}
