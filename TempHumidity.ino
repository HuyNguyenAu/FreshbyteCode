#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

/* 
 *  Temperature and Humidity Sensor
 *  Pins: DATA(IO0), VCC(VCC), GND(GND)
 */

const uint8_t tempHumidityPin = 0;

DHT_Unified dht(tempHumidityPin, DHT11);
uint32_t delayMS;

// Setup temp and humidity sensor.
void SetupTempHumidity() {
  Serial.println(F("DHT11 setting up..."));
  // Set max and min alues temp and humidity sensor.
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  // Get the minimum delay.
  delayMS = sensor.min_delay / 1000;
  Serial.println(F("DHT11 setting up completed."));
}

// Read temp.
float GetTemp() {
  delay(delayMS);
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  // Make sure the value read is valid.
  if (isnan(event.temperature)) {
    return 0.0;
  } else {

    return event.temperature;
  }
}

// Read humidity.
float GetHumidity() {
  delay(delayMS);
  sensors_event_t event;
  dht.humidity().getEvent(&event);
  // Make sure the value read is valid.
  if (isnan(event.relative_humidity)) {
    return 0.0;
  } else {
    return event.relative_humidity;
  }
}
