#include <SPI.h>
#include <LoRa.h>
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8
#define ledBusy 13
#define ledRelay 12
int frequencyArray[3] = {0};
int readingsCount = 0;
unsigned long lastAverageTime = 0;
void setup() {
 pinMode(S0, OUTPUT);
 pinMode(S1, OUTPUT);
 pinMode(S2, OUTPUT);
 pinMode(S3, OUTPUT);
 pinMode(ledBusy, OUTPUT);
 pinMode(ledRelay, OUTPUT);
 pinMode(sensorOut, INPUT);
 digitalWrite(ledBusy, HIGH);
 digitalWrite(ledRelay, LOW);
 digitalWrite(S0, HIGH);
 digitalWrite(S1, LOW);
 Serial.begin(9600);
 while (!Serial);
 Serial.println("LoRa Sender : Serial Started");
 if (!LoRa.begin(433E6)) {
 Serial.println("Starting LoRa failed!");
 while (1);
 }
 Serial.println("LoRa Sender : LoRa Started");
 digitalWrite(ledBusy, LOW);
}
void loop() {
 unsigned long currentTime = millis();
 if (currentTime - lastAverageTime >= 60000) {
 Serial.println("LoRa Sender : Sending RGB Values ...");
 digitalWrite(ledBusy, HIGH);
 collectRGBValues();
 calculateAndSendAverage();
 resetReadings();
 lastAverageTime = currentTime;
 digitalWrite(ledBusy, LOW);
 Serial.println("LoRa Sender : Sending RGB Values END");
 }
}
void collectRGBValues() {
 Serial.println("LoRa Sender : Collecting RGB Values ...");

 for (int i = 0; i <= 10; i++) {
 digitalWrite(S2, HIGH);
 digitalWrite(S3, LOW);
 frequencyArray[0] += pulseIn(sensorOut, LOW); // Add R value to the array
 digitalWrite(S2, HIGH);
 digitalWrite(S3, HIGH);
 frequencyArray[1] += pulseIn(sensorOut, LOW); // Add G value to the array
 digitalWrite(S2, LOW);
 digitalWrite(S3, HIGH);
 frequencyArray[2] += pulseIn(sensorOut, LOW); // Add B value to the array
 readingsCount++;
 }
 Serial.println("LoRa Sender : Collection RGB Values END");
}
void calculateAndSendAverage() {
 Serial.println("LoRa Sender : Calculating RGB Values Average ...");
 int averageR = frequencyArray[0] / readingsCount;
 int averageG = frequencyArray[1] / readingsCount;
 int averageB = frequencyArray[2] / readingsCount;
 int transformerid = 1234;

 String data = String(transformerid) + "," + String(averageR) + "," + String(averageG) + "," + String(averageB);

 Serial.println("LoRa Sender : Sending Via LoRa ...");
 LoRa.beginPacket();
 LoRa.print(data);
 LoRa.endPacket();
 Serial.println("LoRa Sender : Sended Via LoRa END");

 if ((averageR >= 50 || averageR > 100) && (averageG >= 50 || averageG > 100) && (averageB >= 50 || averageB
> 100)) {
 Serial.println("blue");
 }

 else if ((averageR >= 150 || averageR > 200) && (averageG >= 150 || averageG > 200) && (averageB >= 150
|| averageB > 200)) {
 Serial.println("pink, critical situation");
 digitalWrite(ledRelay, HIGH);
 }
}
void resetReadings() {
 memset(frequencyArray, 0, sizeof(frequencyArray));
 readingsCount = 0;
}