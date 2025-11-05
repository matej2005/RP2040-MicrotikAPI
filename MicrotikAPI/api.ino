#include <ArduinoHttpClient.h>
#include <Arduino_JSON.h>
#include <Base64.h>

HttpClient _HttpClient = HttpClient(client, serverIp, port);
int statusCode;
String response;

String authBase64;
String ID;
String postData;

void sendToApiState(bool state) {
  setStatus(INFO);


  String contentType = "application/json";

  postData = "{\"disabled\":\"";
  postData += (state ? "false" : "true");
  postData += "\"}";


  Serial.print("Making GET request at ");
  Serial.print(serverIp + URL_PATH);

  _HttpClient.beginRequest();
  _HttpClient.get(URL_PATH);
  _HttpClient.sendHeader("Authorization", "Basic " + authBase64);
  _HttpClient.endRequest();
  statusCode = _HttpClient.responseStatusCode();
  response = _HttpClient.responseBody();

#ifdef DEBUG
  Serial.print("Response: ");
  Serial.println(response);
#endif

  Serial.printf("\tStatus code: %i\n", statusCode);
  if (statusCode == 200) {
    setStatus(OK);
    ID = parseResponse(response);
    if (ID != "null") {
      ID = "/" + ID;
      Serial.print("Making PATCH request at ");
      Serial.print(serverIp + URL_PATH + ID);

      _HttpClient.beginRequest();
      _HttpClient.patch(URL_PATH + ID);
      //_HttpClient.sendHeader("Authorization", "Basic YXBpOjEyMzQ=");
      _HttpClient.sendHeader("Authorization", "Basic " + authBase64);
      _HttpClient.sendHeader(HTTP_HEADER_CONTENT_TYPE, contentType);
      _HttpClient.sendHeader(HTTP_HEADER_CONTENT_LENGTH, postData.length());
      _HttpClient.print(postData);
      _HttpClient.endRequest();

      // read the status code and body of the response
      statusCode = _HttpClient.responseStatusCode();
      response = _HttpClient.responseBody();

      Serial.printf("\tStatus code: %i\n", statusCode);
      if (statusCode == 200) setStatus(OK);
      else setStatus(WARN);

#ifdef DEBUG
      Serial.print("Response: ");
      Serial.println(response);
#endif

    } else {
      Serial.print("Cant find the variable: \"");
      Serial.print(targetVariable);
      Serial.print("\" with value: \"");
      Serial.print(targetVariableValue);
      Serial.print("\"\n");
      setStatus(WARN);
    }
  } else setStatus(WARN);
}

String parseResponse(String response) {
  String id = "null";

  JSONVar respObject = JSON.parse((response));
  if (JSON.typeof(respObject) == "undefined") {
    Serial.println("Parsing input failed!");
    return "null";
  }

  for (int i = 0; i < respObject.length(); i++) {
    JSONVar value = respObject[i];
    if (value.hasPropertyEqual(targetVariable, targetVariableValue)) {
      id = String(value[".id"]);
      return id;
    }
  }
  return "null";
}

int apiRequest(RequestTypes type, char* server, String URL, String data) {

  _HttpClient.beginRequest();
  switch (type) {
    case GET:
      _HttpClient.get(URL);
      break;
    case PATCH:
      _HttpClient.patch(URL_PATH + ID);
      break;
  }
  _HttpClient.sendHeader("Authorization", "Basic " + authBase64);

  if (type == PATCH) {
    _HttpClient.sendHeader(HTTP_HEADER_CONTENT_TYPE, "application/json");
    _HttpClient.sendHeader(HTTP_HEADER_CONTENT_LENGTH, postData.length());
    _HttpClient.print(data);
  }

  _HttpClient.endRequest();


  return 0;
}

void prepareAuth() {

  String authString = AUTH_USER + ":" + AUTH_PASSWD;

  unsigned int authCharLength = authString.length() + 1;
  char authChar[authCharLength];
  authString.toCharArray(authChar, authCharLength);

  unsigned int encodedLength = Base64.encodedLength(strlen(authChar));
  char encodedString[encodedLength + 1];
  Base64.encode(encodedString, authChar, authCharLength - 1);
  authBase64 = (String)encodedString;
}
