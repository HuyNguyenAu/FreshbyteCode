#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <NTPClient.h>
#include "AzureIotHub.h"
#include "Esp32MQTTClient.h"

// Send every 10 secs.
#define INTERVAL 10000
#define MESSAGE_MAX_LEN 256

// WiFi AP Credentials
WiFiMulti wifiMulti;
const char* ssid = "OPTUS_A3252C";
const char* password = "diazoscrog56017";
const char* ssidMobile = "Huygen";
const char* passwordMobile = "teqa1526";

// NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.au.pool.ntp.org", 3600 * 11);

// IoTHub Connection String
static const char* connectionString = "HostName=raventl-iot-hub.azure-devices.net;DeviceId=testdevice;SharedAccessKey=[key]";

// Handle and track messages.
int messageCount = 1;

// Check for WiFi.
static bool wifiFound = false;

// Only send one message at a time, and when it goes through successfully.
static bool messageSending = true;

// Delay sending messages. Store the previous time that the message was sent.
static uint64_t previousTime;

// Spoof GPS coordinates.
const uint8_t mapSize = 7;
uint8_t mapCount = 0;
const double coordsLat[mapSize] = { -37.81012, -37.81012, -37.809734, -37.809653, -37.809475, -37.808996, -37.80819};
const double coordsLon[mapSize] = {144.963921, 144.963921, 144.963774, 144.964002, 144.963912, 144.963651, 144.963318};

// Testing. Print all sensor values.
void Test()
{
  Serial.print(F("Temp: "));
  Serial.println(GetTemp());
  Serial.print(F("Humidity: "));
  Serial.println(GetHumidity());
  Serial.print(F("O2: "));
  Serial.println(O2Concentration());
  Serial.print(F("CO2: "));
  Serial.println(GetCO2());
  Serial.print(F("Accel: "));
  Serial.println(GetAccelMag());
}

void SetupWifi()
{
  Serial.println("Connecting to WiFi...");
  // Connect to WiFi.
  wifiMulti.addAP(ssid, password);
  wifiMulti.addAP(ssidMobile, passwordMobile);

  // Keep waiting to connect to WiFi.
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }
  wifiFound = true;
  Serial.println(F("Connecting to WiFi successful."));
}

void SetupNTP()
{
  timeClient.begin();
}

void setup()
{
  Serial.begin(9600);
  Serial.println(F("Setting up..."));

  // Blinky.
  pinMode(LED_BUILTIN, OUTPUT);

  // Prepare sensors.
  SetupAccel();
  SetupTempHumidity();
  O2Concentration();
  SetupWifi();
  SetupNTP();
  SetupGPS();
  SetupDisplay();

  // If somehow we connected to the WiFi but then disconnect, abort.
  if (!wifiFound)
  {
    return;
  }

  Serial.println(F("Setting up IoTHub..."));
  Esp32MQTTClient_SetOption(OPTION_MINI_SOLUTION_NAME, "GetStarted");
  Esp32MQTTClient_Init((const uint8_t*)connectionString, true);
  Esp32MQTTClient_SetSendConfirmationCallback(SendConfirmationCallback);
  Esp32MQTTClient_SetMessageCallback(MessageCallback);
  Esp32MQTTClient_SetDeviceTwinCallback(DeviceTwinCallback);
  Esp32MQTTClient_SetDeviceMethodCallback(DeviceMethodCallback);
  Serial.println(F("Setting up IoTHub successful."));

  previousTime = millis();
}

void loop() {
  ReadGPS();
  //  Test();
  //  Make sure we are connected to WiFi.
  if (wifiFound)
  {
    while (!timeClient.update())
    {
      timeClient.forceUpdate();
    }

    // Only send when allowed. And make sure it's every 10 secs.
    if (messageSending && (int)(millis() - previousTime) >= INTERVAL)
    {
      // Turn on LED.
      digitalWrite(LED_BUILTIN, HIGH);
      char messagePayload[MESSAGE_MAX_LEN];
      CreateMessage(messagePayload);
      Serial.println(messagePayload);
      WriteToScreen(messagePayload);
      EVENT_INSTANCE* message = Esp32MQTTClient_Event_Generate(messagePayload, MESSAGE);
      Esp32MQTTClient_SendEventInstance(message);

      previousTime = millis();
    }
    else
    {
      Esp32MQTTClient_Check();
    }
    // Turn off LED.
    digitalWrite(LED_BUILTIN, LOW);
  }
}
