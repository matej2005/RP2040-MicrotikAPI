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

#ifndef ARDUINO_ARCH_RP2040
#warning "Code is intended to run on RP2040 architecture or Waveshare RP2040 Zero board"
#endif

#define VERSION "1.1.1"

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

void setup() {
  pinMode(INPUT_PIN, INPUT_PULLUP);
  // Ethernet.init(ETHERNET_CS_PIN);

#ifdef PIN_NEOPIXEL
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear();  // Set all pixel colors to 'off'
  pixels.show();
#else
  pinMode(PIN_LED, OUTPUT);
#endif

  // Open serial communications
  Serial.begin(115200);

  delay(5000);

  Serial.print(F("Starting MicrotikAPI " VERSION " from " __DATE__ " by Matěj Mrkva\nProject: https://github.com/matej2005/RP2040-MicrotikAPI\nURL: https://mars-engineers.cz/ \nemail: matej.mrkva@mars-engineers.cz\n====================================================================================================\n"));

  prepareAuth();
  ethernetTryConnect();

  updateEthStatus();
  lastEthStatus = ethStatus;

  // give the Ethernet shield a second to initialize:
  delay(1000);
}

void loop() {
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

void ethernetTryConnect() {
  Ethernet.init(ETHERNET_CS_PIN);
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
    setStatus(WARN);
  } else {
    // start the Ethernet connection:
    Serial.println("Initializing Ethernet.");
    DHCP = Ethernet.begin(mac);
    if (DHCP == 0) {
      Serial.println("ERR\nFailed to configure Ethernet using DHCP");
      // Check for Ethernet hardware present
      if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
        setStatus(ERROR);
        //while (true) { delay(1); }  // do nothing, no point running without Ethernet hardware
        delay(10000);
        ethernetTryConnect();
      }
      // try to configure using IP address instead of DHCP:
      Ethernet.begin(mac, ip, myDns);
    } else {
      printNet();

      inputState = digitalRead(INPUT_PIN);
      lastInputState = inputState;
      sendToApiState(inputState);
      setStatus(OK);
    }
  }
}

Status currentStatus = CLEAR;

void setStatus(Status status) {
  currentStatus = status;
#ifdef PIN_NEOPIXEL
  pixels.clear();  // Set all pixel colors to 'off'
  switch (status) {
    case OK:
      pixels.setPixelColor(0, pixels.Color(0, 50, 0));
      break;
    case WARN:
      pixels.setPixelColor(0, pixels.Color(64, 23, 0));
      break;
    case INFO:
      pixels.setPixelColor(0, pixels.Color(0, 0, 50));
      break;
    case ERROR:
      pixels.setPixelColor(0, pixels.Color(50, 0, 0));
      break;
    case CLEAR:
      break;
  }
  pixels.show();  // Send the updated pixel colors to the hardware.
#else

#endif
}


void updateEthStatus() {
  if (Ethernet.linkStatus() == LinkOFF) ethStatus = false;
  if (Ethernet.linkStatus() == LinkON) ethStatus = true;
}

void printNet() {
  Serial.println("====================================================================================================");

  Serial.print(" MicrotikAPI network configuration : ");
  if (DHCP) Serial.println("DHCP");
  else Serial.println("static");

  Serial.print(" MAC\t\t: ");
  Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.print(" IP\t\t: ");
  Serial.println(Ethernet.localIP());
  Serial.print(" Subnet Mask\t: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print(" Gateway\t: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print(" DNS\t\t: ");
  Serial.println(Ethernet.dnsServerIP());
  Serial.println("====================================================================================================");
}
