/*
 MicrotikApi https://github.com/matej2005/RP2040-MicrotikAPI

 This sketch connects to a routerboard
 using an WIZnet Ethernet board.

 Circuit:
 * Ethernet board attached to pins 2, 3, 4, 5

 created 18 Dec 2009
 by David A. Mellis
 modified 19 Jun 2025
 by Matěj Mrkva, based on work by Tom Igoe


 Libraries needed:
    Adafruit NeoPixel by Adafruit
    Ethernet
    ArduinoHttpClient by Arduino
    Arduino_JSON by Arduino
    Base64 by Xander
 */

#include <SPI.h>
#include <Ethernet.h>
#include "config.h"
#include <VFS.h>
#include <LittleFS.h>
#include <Arduino_JSON.h>


#if !(ARDUINO_ARCH_RP2040 | ARDUINO_WAVESHARE_RP2040_ZERO | ARDUINO_WIZNET_5500_EVB_PICO | ARDUINO_WIZNET_5500_EVB_PICO2)
#warning "Code is intended to run on RP2040/RP2350 architecture or Waveshare RP2040 Zero or W5500-EVB-Pico* board"
#endif

#define VERSION "1.1.0"


#if (ARDUINO_ARCH_RP2040)
#define CPU "RP2040"
#else
#if (PICO_RP2350A)
#define CPU "RP2350"
#else
#define CPU "na"
#endif
#endif



// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

bool inputState = false, lastInputState = false;
unsigned long currentMillis, lastMillis;

bool ethStatus, lastEthStatus, DHCP;

#ifdef PIN_NEOPIXEL
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pixels(LEDS, LED_GPIO, NEO_GRB + NEO_KHZ800);
#endif

enum Status {
  OK,
  INFO,
  WARN,
  ERROR,
  CLEAR
};

enum RequestTypes {
  GET,
  PATCH
};



void setup() {
  pinMode(INPUT_PIN, INPUT_PULLUP);
  // Ethernet.init(ETHERNET_CS_PIN);
  setupStatus();
  // Open serial communications
  Serial.begin(115200);

  delay(5000);

  Serial.print(F("Starting RouterOs-deck " VERSION " from " __DATE__ " by Matěj Mrkva\nProject: https://github.com/matej2005/RP2040-MicrotikAPI\nURL: https://mars-engineers.cz/ \nemail: matej.mrkva@mars-engineers.cz\n====================================================================================================\n"));

  prepareAuth();
  setupEthernet();

  LittleFS.begin();
  VFS.root(LittleFS);

  setupHtml();
  // give the Ethernet shield a second to initialize:
  delay(1000);
}

void loop() {
  loopHtml();

  currentMillis = millis();
  if (currentMillis - lastMillis >= 1000) {
    lastMillis = currentMillis;

    updateEthStatus();

    if (ethStatus != lastEthStatus) {
      lastEthStatus = ethStatus;
      if (ethStatus) {
        Serial.println("INFO: Ethernet cable is connected.");
        ethernetTryConnect();
      } else {
        setStatus(WARN);
        Serial.println("INFO: Ethernet cable is not connected.");
      }
    }
    if (ethStatus) {
      inputState = digitalRead(INPUT_PIN);
      if (inputState != lastInputState) {
        lastInputState = inputState;
        sendToApiState(inputState);
      }
    }
  }
}
