// code changement de couleur direct, pas les 3 couleurs qui se suivent 

#include <SPI.h>
#include <LoRa.h>
#include <FastLED.h>

int counter = 0;

int txPower = 14;
int spreadingFactor = 12;
long signalBandwidth = 125E3;
int codingRateDenominator = 5;
int preambleLength = 8;
//String payload = "hello";

#define SS 10
#define RST 8
#define DI0 6
#define BAND 865E6

#define DATA_PIN    4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    21
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          16
#define FRAMES_PER_SECOND  120

uint8_t gHue = 0;

int accelerationThreshold = 500;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("LoRa Receiver");
  Serial.print("SetFrequency : ");
  Serial.print(BAND);
  Serial.println("Hz");
  Serial.print("SetSpreadingFactor : SF");
  Serial.println(spreadingFactor);

  SPI.begin();
  LoRa.setPins(SS, RST, DI0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.setTxPower(txPower, 1);
  LoRa.setSpreadingFactor(spreadingFactor);
  LoRa.setSignalBandwidth(signalBandwidth);
  LoRa.setCodingRate4(codingRateDenominator);
  LoRa.setPreambleLength(preambleLength);

  Serial.begin(115200);
  delay(1000);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //Serial.print("Received packet: '");
    String receivedPacket = "";
    while (LoRa.available()) {
      char receivedChar = (char)LoRa.read();
      receivedPacket += receivedChar;
    }
    Serial.print(receivedPacket);

    char firstChar = receivedPacket.charAt(0);
    //Serial.print(firstChar);
//Serial.print("test2");
    if (firstChar == 'x' || firstChar == 'y' || firstChar == 'z') {
     // Serial.print("test");
      Serial.print(receivedPacket.substring(1));
      int acceleration = receivedPacket.substring(1).toInt();
      if (firstChar == 'x') {
        // Packet starting with 'x': Acceleration on the X-axis
        //mapAndSetLEDs(acceleration, -1000, 1000, CRGB::Red);
        if (acceleration >= accelerationThreshold) {
          // Allumer les LEDs en rouge
        // Serial.print(accelerationThreshold);
         fill_solid(leds, NUM_LEDS, CRGB::Red);
         FastLED.show();



          // Attendre pendant quelques millisecondes
         delay(200); // Adjust the delay time as needed

         // Éteindre les LEDs
         fill_solid(leds, NUM_LEDS, CRGB::Black);
         FastLED.show();

          counter++;
}
    Serial.print(acceleration);

      } if (firstChar == 'y') {
         // Packet starting with 'y': Acceleration on the Y-axis
         //mapAndSetLEDs(acceleration, -1000, 1000, CRGB::Green);
         if (acceleration >= accelerationThreshold) {
  
           fill_solid(leds, NUM_LEDS, CRGB::Green);
           FastLED.show();

           // Attendre pendant quelques millisecondes
           delay(200); 

           // Éteindre les LEDs
           fill_solid(leds, NUM_LEDS, CRGB::Black);
           FastLED.show();

           counter++;
}

      } if (firstChar == 'z') {
        // Packet starting with 'z': Acceleration on the Z-axis
        //mapAndSetLEDs(acceleration, -1000, 1000, CRGB::Blue);
         if (acceleration >= accelerationThreshold) {
  
           fill_solid(leds, NUM_LEDS, CRGB::Blue);
           FastLED.show();

            // Attendre pendant quelques millisecondes
           delay(200); // Adjust the delay time as needed

           // Éteindre les LEDs
           fill_solid(leds, NUM_LEDS, CRGB::Black);
           FastLED.show();

            counter++;
         }

       }
   } else {
      // Other packets: Turn off LEDs
      FastLED.clear();
      FastLED.show();
    }

   Serial.print("' with RSSI ");
   Serial.println(LoRa.packetRssi());
  }

  
  FastLED.show();

  
}

void mapAndSetLEDs(int value, int inMin, int inMax, CRGB color) {
  int outMin = 0;
  int outMax = NUM_LEDS - 1;
  int mappedValue = map(value, inMin, inMax, outMin, outMax);

  for (int i = 0; i < NUM_LEDS; i++) {
    if (i <= mappedValue) {
      leds[i] = color;
    } else {
      leds[i] = CRGB::Black;
    }
  }

  FastLED.show();
}




