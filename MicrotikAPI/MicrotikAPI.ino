/*
 MicrotikApi

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
#include <Adafruit_NeoPixel.h>

#ifndef ARDUINO_ARCH_RP2040
#warning "Code is intended to run on RP2040 architecture or Waveshare RP2040 Zero board"
#endif


// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

bool inputState = false, lastInputState = false;
unsigned long currentMillis, lastMillis;

bool ethStatus, lastEthStatus;

Adafruit_NeoPixel pixels(LEDS, LED_GPIO, NEO_GRB + NEO_KHZ800);

enum Status {
  OK,
  INFO,
  WARN,
  ERROR,
  CLEAR
};

void setup() {
  pinMode(INPUT_PIN, INPUT_PULLUP);
  Ethernet.init(ETHERNET_CS_PIN);
  pixels.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.clear();  // Set all pixel colors to 'off'
  pixels.show();

  // Open serial communications
  Serial.begin(115200);

  delay(5000);

  Serial.print("Starting MicrotikAPI v1 07/10/2025 by Matěj Mrkva\nProject: https://github.com/matej2005/RP2040-MicrotikAPI\nURL: https://mars-engineers.cz/ \nemail: matej.mrkva@mars-engineers.cz\n");

  ethernetTryConnect();
  prepareAuth();
  sendToApiState(inputState);

  // give the Ethernet shield a second to initialize:
  delay(1000);
}

void loop() {
  currentMillis = millis();
  if (currentMillis - lastMillis >= 1000) {
    lastMillis = currentMillis;

    if (Ethernet.linkStatus() == LinkOFF) ethStatus = false;
    if (Ethernet.linkStatus() == LinkON) ethStatus = true;

    if (ethStatus != lastEthStatus) {
      lastEthStatus = ethStatus;
      if (ethStatus) {
        Serial.println("Ethernet cable is connected.");
        ethernetTryConnect();
      } else {
        setStatus(WARN);
        Serial.println("Ethernet cable is not connected.");
      }
    }

    inputState = digitalRead(INPUT_PIN);
    if (inputState != lastInputState) {
      lastInputState = inputState;
      if (ethStatus) sendToApiState(inputState);
    }
  }
}

void ethernetTryConnect() {
  // start the Ethernet connection:
  Serial.print("Initialize Ethernet with DHCP: ");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("ERR\nFailed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");

      setStatus(ERROR);

      while (true) {
        delay(1);  // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
      setStatus(WARN);
    }
    // try to configure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, myDns);
  } else {
    Serial.print("assigned IP: ");
    Serial.println(Ethernet.localIP());
    setStatus(OK);
  }
}


void setStatus(Status status) {
  pixels.clear();  // Set all pixel colors to 'off'
  switch (status) {
    case OK:
      pixels.setPixelColor(0, pixels.Color(0, 100, 0));
      break;
    case WARN:
      pixels.setPixelColor(0, pixels.Color(255, 92, 0));
      break;
    case INFO:
      pixels.setPixelColor(0, pixels.Color(0, 0, 255));
      break;
    case ERROR:
      pixels.setPixelColor(0, pixels.Color(255, 0, 0));
      break;
    case CLEAR:
      break;
  }
  pixels.show();  // Send the updated pixel colors to the hardware.
}
