/*
 * Author: Jorge Navarro-Ortiz, based on JP Meijers's work
 * Date: 2024-04-24
 *
 * Transmit a one byte packet via TTN. This happens as fast as possible, while still keeping to
 * the 1% duty cycle rules enforced by the RN2483's built in LoRaWAN stack. Even though this is
 * allowed by the radio regulations of the 868MHz band, the fair use policy of TTN may prohibit this.
 *
 * CHECK THE RULES BEFORE USING THIS PROGRAM!
 *
 * CHANGE ADDRESS!
 * Change the device address, network (session) key, and app (session) key to the values
 * that are registered via the TTN dashboard.
 * The appropriate line is "myLora.initABP(XXX);" or "myLora.initOTAA(XXX);"
 * When using ABP, it is advised to enable "relax frame count".
 *
 * Connect the RN2xx3 as follows:
 * RN2xx3 -- ESP8266
 * Uart TX -- GPIO13
 * Uart RX -- GPIO15
 * Reset -- GPIO12
 * Vcc -- 3.3V
 * Gnd -- Gnd
 *
 */
#include <rn2xx3.h>
#include <SoftwareSerial.h>

#define RN2483NODE 1
//#define ONECHANNELGW 1
#define OLED 1
#define DHTSENSOR
#define BMP180SENSOR
#define MPU6050SENSOR

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#include <Wire.h>
#include <Adafruit_Sensor.h>

#ifdef OLED
#include "SSD1306.h" 
#include "images.h"
SSD1306 display(0x3c, 4, 5); // ESP8266 with HALLARD board
#endif

#ifdef DHTSENSOR
#include <DHT.h>
#define DHTPIN 2     // Digital pin connected to the DHT sensor
// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);
// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;
#endif

#ifdef BMP180SENSOR
#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
float bmpT = 0.0;
float bmpA = 0.0;
float bmpP = 0.0;
#endif

#ifdef MPU6050SENSOR
#include <Adafruit_MPU6050.h>
Adafruit_MPU6050 mpu;
sensors_event_t a, g, temp;
float roll, pitch;
#endif

const long interval = 10000;      // Updates DHT readings every 10 seconds
unsigned long previousMillis = 0; // will store last time DHT was updated

#include <CayenneLPP.h>
#define CHANNEL_INCR 1
#define CHANNEL_TEMP 2
#define CHANNEL_HUM 3
#define CHANNEL_PRES 4
#define CHANNEL_ALT 5
#define CHANNEL_ROLL 6
#define CHANNEL_PITCH 7
struct LPP_BLOCK {
  uint8_t typ;
  int val;
};
//Receive buffer for a maximum of 8 channels 
LPP_BLOCK empBuf[8] ; 
// Buffer CayenneLPP data format. 
CayenneLPP lpp(64) ; 

#define RESET 12
SoftwareSerial mySerial(13, 15 ); // RX, TX !! labels on relay board is swapped !!

//create an instance of the rn2xx3 library,
//giving the software UART as stream to use,
//and using LoRa WAN
rn2xx3 myLora(mySerial);

unsigned int counter = 0;

#ifdef OLED
void logo(){
  display.clear();
  display.drawXbm(35,12,LoRa_Logo_width,LoRa_Logo_height,LoRa_Logo_bits);
  display.display();
}
#endif

// the setup routine runs once when you press reset:
void setup() {
  // LED pin is GPIO2 which is the ESP8266's built in LED
  pinMode(2, OUTPUT);
  led_on();

  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  mySerial.begin(57600); // It does not work with 115200 bps.

  delay(1000); //wait for the arduino ide's serial console to open

  Serial.println("");
  Serial.println("Startup (chip ID: " + String(ESP.getChipId()) + ")");
  Serial.println("Node RN2483 #" STR(RN2483NODE));

#ifdef OLED
    display.init();
    display.flipScreenVertically();  
    display.setFont(ArialMT_Plain_10);
    logo();
    delay(1500);
#endif

  initialize_radio();

  //transmit a startup message
  //myLora.tx("RN2483 #" STR(RN2483NODE));

#ifdef DHTSENSOR
  // DHT
  dht.begin();
#endif

#ifdef BMP180SENSOR
    //Wire.begin(4, 5);
    int result = bmp.begin();
    if (result == 0) {
      Serial.println("BMP initialization: failed!");
    } else {
      Serial.println("BMP initialization: ok");
    }
#endif

#ifdef MPU6050SENSOR
  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("MPU6050 initialization: failed!");
  }
  Serial.println("MPU6050 initialization: ok");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("MPU6050 Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("MPU6050 Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("MPU6050 Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }
#endif

  led_off();
  delay(2000);
}

void initialize_radio()
{
  //reset RN2xx3
  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, LOW);
  delay(100);
  digitalWrite(RESET, HIGH);

  delay(100); //wait for the RN2xx3's startup message
  mySerial.flush();

  //check communication with radio
  String hweui = myLora.hweui();
  while(hweui.length() != 16)
  {
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  //print out the HWEUI so that we can register it via ttnctl
  Serial.println("When using OTAA, register this DevEUI:");
  Serial.println(hweui);
  Serial.println("RN2xx3 firmware version:");
  Serial.println(myLora.sysver());

  //configure your keys and join the network
  Serial.println("Trying to join TTN");
  bool join_result = false;

  //ABP: initABP(String addr, String AppSKey, String NwkSKey);
#if RN2483NODE==1
  join_result = myLora.initABP("260115E4", "B3C72EDF4030FFA1E0D9BB420E29E4A0", "E6C47723F78A6E80C178B1DDABF900F8");
#elif RN2483NODE==2
  join_result = myLora.initABP("26011AD3", "A540ACC9D2E1F02407C08DD12BD77986", "C7F3E5ABAB2C4F656FD3D18B530BA5A6");
#endif

#ifdef ONECHANNELGW
  // Check RN2483 commands at http://ww1.microchip.com/downloads/en/DeviceDoc/40001784B.pdf

  // From https://www.thethingsnetwork.org/forum/t/how-to-make-rn2483-node-use-a-fixed-channel-and-spreading-factor/2156/5
  // Disable 7 of the 8 default channels. This basically disables the
  // mandatory LoRaWAN channel hopping, so FOR TESTING ONLY.
  // From RN2483 command reference:
  // "The default settings consider only three default channels (0-2), and their default duty cycle is 0.33%."
  Serial.println("Enabling only one channel (to operate with a single channel gateway)");
  //exec("mac set ch status 0 off");
  exec("mac set ch status 1 off");
  exec("mac set ch status 2 off");

  // Print the default duty cycle: 799 means 100/(799+1) = 0.125%, which
  // is 1% for the whole sub-band, divided over 8 channels; see explanation
  // on https://www.thethingsnetwork.org/wiki/LoRaWAN/Duty-Cycle
  Serial.println("Configuring a duty cycle of 1%");
  exec("mac set ch dcycle 0 99"); // <dutyCycle> = (100/X) - 1, where X is the duty cycle in percentage (e.g. if duty cycle is 1%, X=1 -> <dutyCycle> = (100/1) - 1 = 99

  // From https://www.thethingsnetwork.org/forum/t/setting-fixed-spreading-factor-on-rn2483/8838 (see table 4 in LoRaWAN_Regional_Parameters_v1_0-20161012_1397_1.pdf)
  // 0=SF12/125kHz, 1=SF11/125kHz, 2=SF10/125kHz, 3=SF9/125kHz, 4=SF8/125kHz, 5=SF7/125kHz, 6=SF7/250kHz, 7=FSK, 8...15=RFU
  Serial.println("Configuring SF7/125kHz");
  exec("mac set adr off");
  exec("mac set dr 5"); // On EU863-870; SF7/125 kHz
#endif

  //OTAA: initOTAA(String AppEUI, String AppKey);
  //join_result = myLora.initOTAA("70B3D57EF0005169", "16A744BF2699E9C571602D24CE436083");

  while(!join_result)
  {
    Serial.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
    delay(60000); //delay a minute before retry
    join_result = myLora.init();
  }
  Serial.println("Successfully joined TTN");

}

// the loop routine runs over and over again forever:
void loop() {
  led_on();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
#ifdef DHTSENSOR
    // Read temperature as Celsius (the default)
    float newT = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float newT = dht.readTemperature(true);
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
      Serial.println("DHT Temperature: " + String(t) + "ºC");
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      Serial.println("DHT Humidity: " + String(h) + "%");
    }
#endif

#ifdef BMP180SENSOR
    bmpT = bmp.readTemperature();
    if (isnan(bmpT)) {
      Serial.println("Failed to read from BMP180 sensor!");
    }
    else {
      Serial.println("BMP180 Temperature: " + String(bmpT) + "ºC");
    }
    bmpP = bmp.readPressure() / 100; // mbar
    if (isnan(bmpP)) {
      Serial.println("Failed to read from BMP180 sensor!");
    }
    else {
      Serial.println("BMP180 Pressure: " + String(bmpP) + " mbar");
    }
    bmpA = bmp.readAltitude(101000); // Pressure (Pascal = 100 * mbar) at sea level in that area
    if (isnan(bmpA)) {
      Serial.println("Failed to read from BMP180 sensor!");
    }
    else {
      Serial.println("BMP180 Altitude: " + String(bmpA) + " m");
    }
#endif

#ifdef MPU6050SENSOR
  mpu.getEvent(&a, &g, &temp);

   /* Print out the values */
  Serial.print("MPU6050 Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("MPU6050 Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  // formula from https://wiki.dfrobot.com/How_to_Use_a_Three-Axis_Accelerometer_for_Tilt_Sensing
  // We could add a filter here...
  roll = atan2(a.acceleration.y , a.acceleration.z) * 180.0 / PI;
  pitch = atan2(-a.acceleration.x , sqrt(a.acceleration.y * a.acceleration.y + a.acceleration.z * a.acceleration.z)) * 180.0 / PI; //account for roll already applied

  Serial.print("MPU6050 Roll = ");
  Serial.print(roll,1);
  Serial.print(", pitch = ");
  Serial.print(pitch,1);
  Serial.println(" degrees");

  Serial.print("MPU6050 Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");
#endif

    //Clear buffer 
    lpp.reset();
    //Write data packets into the buffer 
    lpp.addDigitalInput(CHANNEL_INCR, counter);
#ifdef DHTSENSOR
    lpp.addTemperature(CHANNEL_TEMP, t);
    lpp.addRelativeHumidity(CHANNEL_HUM, h);
#endif
#ifdef BMP180SENSOR
    //lpp.addBarometricPressure(CHANNEL_PRES, bmpP); // Error decoding on ChirpStack
    //lpp.addAltitude(CHANNEL_ALT, bmpA);            // Error decoding on ChirpStack
    lpp.addBarometricPressure(CHANNEL_PRES, bmpP);
    lpp.addBarometricPressure(CHANNEL_ALT, bmpA); // addAnalogInput has +-327 for range, too low, better range with barometric pressure
#endif
#ifdef MPU6050SENSOR
    lpp.addBarometricPressure(CHANNEL_ROLL, roll); // addAnalogInput has +-327 for range, too low, better range with barometric pressure
    lpp.addBarometricPressure(CHANNEL_PITCH, pitch);
#endif

    Serial.println("TXing");
    //myLora.tx("RN2483 #" STR(RN2483NODE)); //one byte, blocking function
    myLora.txBytes(lpp.getBuffer(), lpp.getSize());
    counter++;
  }

#ifdef OLED
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Node HALLARD RN2483 #" STR(HALLARDNODE));
    display.drawString(0, 10, "CID: " + String(ESP.getChipId()));
    display.drawString(0, 20, "Sending packet: ");
    display.drawString(90, 20, String(counter));
#ifdef DHTSENSOR
    display.drawString(0,30, "temp: " + String(t) + ", hum: " + String(h) + "%");
#endif
#ifdef BMP180SENSOR
    display.drawString(0,40, "pres: " + String(bmpP) + " mb, alt: " + String(bmpA) + "m");
#endif
#ifdef BMP180SENSOR
    display.drawString(0,50, "roll: " + String(roll) + " pitch: " + String(pitch) + " º");
#endif
    display.display();
#endif

    led_off();
    delay(200);
}

void led_on()
{
  digitalWrite(2, 1);
}

void led_off()
{
  digitalWrite(2, 0);
}

/**
 * Executes the given AT command and prints its result, if any. 
 * FOR TESTING ONLY; might cause a memory leak with String?
 */
#define DEBUGSERIAL Serial
#define LORASERIAL mySerial
String exec(const char* cmd) {
  DEBUGSERIAL.print(cmd);
  DEBUGSERIAL.print(F(": "));
  LORASERIAL.println(cmd);
  while(!LORASERIAL.available()) {
    delay(50);
  }
  String s = "";
  // This includes \r\n newline:
  while(LORASERIAL.available()) {
    s.concat((char)LORASERIAL.read());
  }
  DEBUGSERIAL.write(s.c_str());
  
  return s;
}
