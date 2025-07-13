#include <sys/_stdint.h>
//////////////////////////////
/*      Configuration       */
//////////////////////////////


// Pinout
#define INPUT_PIN 6
#define LEDS 1

#ifdef ARDUINO_WIZNET_5500_EVB_PICO
#define ETHERNET_CS_PIN 17u
#define LED_GPIO PIN_LED
#endif

#ifdef ARDUINO_WAVESHARE_RP2040_ZERO
#define ETHERNET_CS_PIN 5
#define LED_GPIO PIN_NEOPIXEL

#endif


// RouterOS configuration
char server[] = "172.16.1.1";    // IP of the routerboard
const uint16_t port = 80;        // Port of http interface
const String AUTH_USER = "api";
const String AUTH_PASSWD = "1234";
const String targetVariable = "comment";                   // Targeted rule by variable
const String targetVariableValue = "GPIO controled port";  // Targeted rule by variable with value

const String URL_PATH = "/rest/ip/firewall/nat";  // REST url

// Network
//byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // Mac of this device
byte mac[] = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56};  // Mac of this device

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);


//#define DEBUG // Unccoment do enable debug