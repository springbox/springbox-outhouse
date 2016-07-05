#include <SPI.h>
#include <WiFi101.h>
#include <b64.h>
#include <HttpClient.h>
#include <Adafruit_SleepyDog.h>
#include ".\conf.h"

const unsigned int SECOND = 1000;

WiFiClient client;
int wifiStatus = WL_IDLE_STATUS;

int previousSensorState;

void setup() {
  // Initialize sensor pin:
  pinMode(OH_SENSOR_PORT, INPUT);

  // Calibrate sensor pin:
  digitalWrite(OH_SENSOR_PORT, HIGH);
  
  // Initialize serial and wait for port to open:
  Serial.begin(9600);

  // Wait for serial port to connect. Needed for native USB port only.
  while (!Serial);

  // Check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");

    // Don't continue:
    while (true);
  }

  // Attempt to connect to Wifi network:
  while (wifiStatus != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(OH_SSID);

    // Connect to WPA/WPA2 network:
    wifiStatus = WiFi.begin(OH_SSID, OH_PASSWORD);

    // Wait 10 seconds for connection:
    delay(10 * SECOND);
  }

  // You're connected now, so print out the data:
  Serial.print("You're connected to the network\n");
  Serial.println("Ready to read");
}

void loop() {
  int sensorState = digitalRead(OH_SENSOR_PORT);

  Serial.println(String(sensorState) + " " + String(previousSensorState));

  // Only make an HTTP request if our cached state is different from the
  // measured state.
  //
  // This means if the state is overridden on the server, we will not make
  // a change request until our cache is forced to update.
  if (sensorState != previousSensorState) {
    httpRequest(sensorState);
    previousSensorState = sensorState;
  }

  // Force a deep sleep. This should conserve a lot of battery life.
  deepSleep(OH_TARGET_POSTING_INTERVAL);
}

void deepSleep(int sleepTime) {
  Serial.println("Sleeping.");
  
  // Watchdog will aim to sleep the requested amount of time. It looks like
  // it can only sleep in powers of 2. This means if we request 5 seconds,
  // it will deep sleep 8.
  int actualSleepMs = Watchdog.sleep(sleepTime * SECOND);

  Serial.println("Slept deeply for " + String(actualSleepMs) + " ms.");
}

void httpRequest(int doorStatus) {
  client.stop();

  String status = (doorStatus == 0) ? "true" : "false";

  if (!client.connect(OH_SERVER, 80)) {
    Serial.println("Connection failed.");
    return;
  }

  Serial.println("Connecting....");

  // Send the http GET request
  Serial.println("GET /setStatus?isopen=" + status + " HTTP/1.1");
  client.println("GET /setStatus?isopen=" + status + " HTTP/1.1");
  client.println("Host: " + String(OH_HOST));
  client.println("Connection: close\n");
  client.println();

  Serial.println("Successfully made HTTP Request.");
}
