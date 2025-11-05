void setupEthernet() {
  ethernetTryConnect();

  updateEthStatus();
  lastEthStatus = ethStatus;
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