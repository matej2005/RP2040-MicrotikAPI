#include <sys/_stdint.h>
//////////////////////////////
/*      Configuration       */
//////////////////////////////

// Pinout
#define INPUT_PIN 6
#define LED_GPIO 16
#define LEDS 1

#define ETHERNET_CS_PIN 5


// RouterOS configuration
char server[] = "172.16.1.1";    // IP of the routerboard
const uint16_t port = 80;        // Port of http interface
const String AUTH_USER = "api";
const String AUTH_PASSWD = "1234";
const String targetVariable = "comment";                   // Targeted rule by variable
const String targetVariableValue = "GPIO controled port";  // Targeted rule by variable with value

const String URL_PATH = "/rest/ip/firewall/nat";  // REST url

// Network
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // Mac of this device

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
IPAddress myDns(192, 168, 0, 1);


//#define DEBUG // Unccoment do enable debug