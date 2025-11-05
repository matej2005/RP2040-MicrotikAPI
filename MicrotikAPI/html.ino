EthernetServer server(WEB_HTTP_PORT);
void setupHtml() {
}

void handleRequest(EthernetClient &client, String method, String path) {
  // --- GET ---
  if (method == "GET") {
    if (path == "/") path = "/index.html";

    if (path == "/system.json") {
      serve_dynamic(client, path);
      return;
    }

    String fsPath = "/html" + path;

    fs::File file = LittleFS.open(fsPath, "r");
    if (!file) {
      client.println("HTTP/1.1 404 Not Found");
      client.println("Content-Type: text/plain");
      client.println(F("Connection: close"));
      client.println();
      client.println("File not found");
      return;
    }

    // MIME
    String contentType = "text/plain";
    if (path.endsWith(".html")) contentType = "text/html";
    else if (path.endsWith(".css")) contentType = "text/css";
    else if (path.endsWith(".js")) contentType = "application/javascript";
    else if (path.endsWith(".json")) contentType = "application/json";

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: " + contentType);
    client.println(F("Connection: close"));
    client.println();

    uint8_t buf[512];
    while (file.available()) {
      size_t len = file.read(buf, sizeof(buf));
      client.write(buf, len);
    }
    file.close();
  }

  // --- PATCH ---
  else if (method == "PATCH") {
    // Přečti tělo požadavku
    String body = "";
    unsigned long start = millis();
    while (client.connected() && (millis() - start < 1000)) {
      while (client.available()) {
        char c = client.read();
        body += c;
        start = millis();
      }
    }

    Serial.println("PATCH body:");
    Serial.println(body);

    // Zatím jen odpověz OK
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println(F("Connection: close"));
    client.println();
    client.println("PATCH received");
  }

  // --- Nepodporovaná metoda ---
  else {
    client.println("HTTP/1.1 405 Method Not Allowed");
    client.println("Content-Type: text/plain");
    client.println(F("Connection: close"));
    client.println();
    client.println("Only GET and PATCH supported");
  }
}

void loopHtml() {
  EthernetClient client = server.available();
  if (!client) return;

  String reqLine = "";
  bool currentLineIsBlank = true;
  String method, path;

  unsigned long start = millis();

  while (client.connected() && (millis() - start < 2000)) {
    if (client.available()) {
      char c = client.read();
      reqLine += c;

      if (c == '\n' && currentLineIsBlank) {
        // první řádka např. "GET /index.html HTTP/1.1"
        int m1 = reqLine.indexOf(' ');
        int m2 = reqLine.indexOf(' ', m1 + 1);
        method = reqLine.substring(0, m1);
        path = reqLine.substring(m1 + 1, m2);

        handleRequest(client, method, path);
        break;
      }

      if (c == '\n') currentLineIsBlank = true;
      else if (c != '\r') currentLineIsBlank = false;
    }
  }

  delay(1);
  client.stop();
}

void serve_dynamic(EthernetClient &client, String path) {
  if (path == "/system.json") {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println(F("Connection: close"));
    client.println();

    // vygeneruj JSON přímo v kódu
/*
    JSONVar systemJson;
    systemJson["Name"] = "MicrotikApi";
    systemJson["Version"] = VERSION;
    systemJson["DHCP"] = (Ethernet.localIP()[0] == 0 ? "true" : "false");
    systemJson["IP"] = (String)Ethernet.localIP();
    systemJson["GATEWAY"] = (String)Ethernet.gatewayIP();
    systemJson["MASK"] = (String)Ethernet.subnetMask();
    systemJson["MAC"] = "";
    systemJson["Uptime"] = (millis() / 1000);
    systemJson["CPU"] = CPU;
    systemJson["Temp"] = (double)analogReadTemp();
    systemJson["Comment"] = "";
    Serial.print("JSON.stringify(systemJson) = ");
    Serial.println(systemJson);
    Serial.println();
*/

    client.print("{");
    client.print("\"Name\":\"MicrotikApi\",");
    client.print("\"Version\":\"" VERSION "\",");
    client.print("\"DHCP\":");
    client.print(Ethernet.localIP()[0] == 0 ? "true" : "false");
    client.print(",");
    client.print("\"IP\":\"");
    client.print(Ethernet.localIP());
    client.print("\",");
    client.print("\"GATEWAY\":\"");
    client.print(Ethernet.gatewayIP());
    client.print("\",");
    client.print("\"MASK\":\"");
    client.print(Ethernet.subnetMask());
    client.print("\",");
    client.print("\"MAC\":\"");
    client.printf("%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    client.print("\",");
    client.print("\"Uptime\":\"");
    client.print(millis() / 1000);
    client.print("s\",");

    client.print("\"CPU\":\"" CPU "\",");
    client.printf("\"Temp\":\"%2.1fC\",", (double)analogReadTemp());

    client.print("\"Comment\":\"Generated dynamically\"");
    client.println("}");
  }
}