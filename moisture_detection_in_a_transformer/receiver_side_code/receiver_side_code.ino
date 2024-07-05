
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <SPI.h>
#include <LoRa.h>

#define ledBusy D0

const char* ssid = "ABCD_123"; //Your WFi Name or Hotspot Name
const char* password = "QWERY_1234"; //WiFi Password

String serverName = "http://uniquecnc.in/api/SilicaGel/addsilicageldatav1";
unsigned long lastTimeWiFi = 0;
unsigned long timerDelayWiFi = 30000;

int data[4] = {0};

String inString = "";    // Variable for LoRa data
String MyMessage = "0000,000,000,000";   // Variable to store LoRa message

int transformerIDs[] = {1234, 5678, 9012, 3456, 7890, 2345, 6789, 0123, 4567, 8901};

void setup() {
  pinMode(ledBusy, OUTPUT);
  digitalWrite(ledBusy, HIGH);

  Serial.begin(9600);
  Serial.println("Serial Started...");
  // WiFi setup
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // LoRa setup
  Serial.println("Starting LoRa...");
  if (!LoRa.begin(433E6)) { // or 915E6
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("LoRa Started");
  delay(10000);
  digitalWrite(ledBusy, LOW);
  Serial.println("Loop Started...");
}

void loop() {
  if (LoRa.parsePacket()) {
    digitalWrite(ledBusy, HIGH);
    getLoraReceivedData();
    digitalWrite(ledBusy, LOW);
  }
}

void getLoraReceivedData() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    while (LoRa.available()) {
      int inChar = LoRa.read();
      inString += (char)inChar;
      MyMessage = inString;
    }
    inString = "";
    LoRa.packetRssi();

    int msg_len = MyMessage.length() + 1;
    char msg_array[msg_len];
    MyMessage.toCharArray(msg_array, msg_len);

    char* ptr = strtok(msg_array, ",");

    byte i = 0;
    while (ptr) {
      data[i] = atol(ptr);
      Serial.println(data[i]);
      ptr = strtok(NULL, ",");
      i++;
    }

    for (int i = 0; i < sizeof(transformerIDs); i++) {
      if (data[0] == transformerIDs[i]) {
        Serial.println(MyMessage);
        handleWiFi();
        break;
      }
    }
  }


}

void handleWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String param = "apikey=gridsandy22112008";
    String serverPath = serverName + "?" + param;

    String jsonString = "{\"transformerId\":" + String(data[0]) + "\"r\":" + String(data[1]) + "\"g\":" + String(data[2]) + "\"b\":" + String(data[3]) + "}";

    http.begin(serverPath.c_str());
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(jsonString);

    Serial.print("WiFi - HTTP Response code: ");
    Serial.println(httpResponseCode);
    Serial.println(http.getString());
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}
