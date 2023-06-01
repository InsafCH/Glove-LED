
#include <SPI.h>
#include <LoRa.h>

int counter = 0;

// Parameters you can play with :

int txPower = 14; // from 0 to 20, default is 14
int spreadingFactor = 12; // from 7 to 12, default is 12
long signalBandwidth = 125E3; // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3,41.7E3,62.5E3,125E3,250E3,500e3, default is 125E3
int codingRateDenominator=5; // Numerator is 4, and denominator from 5 to 8, default is 5
int preambleLength=8; // from 2 to 20, default is 8
//String payload = "hello"; // you can change the payload

#define SS 10
#define RST 8
#define DI0 3
#define BAND 865E6  // Here you define the frequency carrier

#define DEBUG   // If DEBUG, plot on the serial port the accelerometer value

#include <FastLED.h> // http://librarymanager/All#FASTLED
#include "kxtj3-1057.h" // http://librarymanager/All#kxtj3-1057

// Accelerometer provides different Power modes by changing output bit resolution
#define LOW_POWER
//#define HIGH_RESOLUTION

// Enable Serial debbug on Serial UART to see registers wrote
#define KXTJ3_DEBUG Serial


#include "Wire.h"

float   sampleRate = 6.25;  // HZ - Samples per second - 0.781, 1.563, 3.125, 6.25, 12.5, 25, 50, 100, 200, 400, 800, 1600Hz
uint8_t accelRange = 2;     // Accelerometer range = 2, 4, 8, 16g

KXTJ3 myIMU(0x0E); // Address can be 0x0E or 0x0F


#define DATA_PIN    4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    21
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          16
#define FRAMES_PER_SECOND  120

         

uint8_t gHue = 0; // rotating "base color" used by many of the patterns



void setup()
{  
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Sender");
  Serial.print("SetFrequency : ");
  Serial.print(BAND);
  Serial.println("Hz");
  Serial.print("SetSpreadingFactor : SF");
  Serial.println(spreadingFactor);

  SPI.begin();
  LoRa.setPins(SS,RST,DI0);

  

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
 LoRa.setTxPower(txPower,1);
 LoRa.setSpreadingFactor(spreadingFactor);
 LoRa.setSignalBandwidth(signalBandwidth);
 LoRa.setCodingRate4(codingRateDenominator);
 LoRa.setPreambleLength(preambleLength);
// LoRa.setPolarity(1);
 //LoRa.setFSK(); 

  Serial.begin(115200);
  delay(10); //wait until serial is open...


  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);  
  
  if( myIMU.begin(sampleRate, accelRange) != 0 )
  {
    Serial.print("Failed to initialize IMU.\n");
  }
  else
  {
    Serial.print("IMU initialized.\n");
  }
  
  // Detection threshold, movement duration and polarity
  myIMU.intConf(123, 1, 10, HIGH);

  uint8_t readData = 0;

  // Get the ID:
  myIMU.readRegister(&readData, KXTJ3_WHO_AM_I);
  Serial.print("Who am I? 0x");
  Serial.println(readData, HEX);

}


void loop()
{
  myIMU.standby( false );
  int16_t dataHighres = 0;
  if( myIMU.readRegisterInt16( &dataHighres, KXTJ3_OUT_X_L ) == 0 ){}

  #ifdef DEBUG
  Serial.print(" Acceleration X RAW = ");
  //Serial.print(dataHighres);
    #endif
  int offset = dataHighres/1550; 
  LoRa.beginPacket();
  LoRa.print("x");
  LoRa.print(dataHighres);  
  
  LoRa.endPacket();
  //counter++;

  //Serial.print("Sending packet with dataHighres  {");
  Serial.print(dataHighres);
  //Serial.print("} N°");
  //Serial.println(counter);
  delay(10);

  if( myIMU.readRegisterInt16( &dataHighres, KXTJ3_OUT_Y_L ) == 0 ){}

  #ifdef DEBUG
  Serial.print(" Acceleration Y RAW = ");
  //Serial.print(dataHighres);
  #endif

  int color = dataHighres/3000;
  LoRa.beginPacket();
  LoRa.print("y"); //a changer en 1 ( x en 0 et z en 2 ) 
  LoRa.print(dataHighres);  
  
  LoRa.endPacket();
  //counter++;

  //Serial.print("Sending packet with dataHighres  {");
  Serial.print(dataHighres);
  //Serial.print("} N°");
  //Serial.println(counter);
 

  delay(10);

  if( myIMU.readRegisterInt16( &dataHighres, KXTJ3_OUT_Z_L ) == 0 ){};
  #ifdef DEBUG
  Serial.print(" Acceleration Z RAW = ");
  Serial.println(dataHighres);
  #endif 

  LoRa.beginPacket();
  LoRa.print("z");
  LoRa.print(dataHighres);  
  
  LoRa.endPacket();
  //counter++;

  //Serial.print("Sending packet with dataHighres {");
  Serial.print(dataHighres);
  //Serial.print("} N°");
  //Serial.println(counter);


 
  Serial.print('\n');

 

  delay(10);
  
  myIMU.standby( true );

  

}