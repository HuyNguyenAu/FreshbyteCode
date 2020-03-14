#include <ArduinoJson.h>

/*
    Message format:
    {
      "Temp":       value,
      "Humidity":   value,
      "O2":         value,
      "CO2":        value,
      "Accel":      value,
      "ShelfLife":  value,
      "Ethylene":   value,
      "Lat":        value,
      "Lon":        value
    }
*/
// https://github.com/arduino-libraries/NTPClient/issues/36
String GetTimeStampString() {
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);

  uint16_t year = ti->tm_year + 1900;
  String yearStr = String(year);

  uint8_t month = ti->tm_mon + 1;
  String monthStr = month < 10 ? "0" + String(month) : String(month);

  uint8_t day = ti->tm_mday;
  String dayStr = day < 10 ? "0" + String(day) : String(day);

  uint8_t hours = ti->tm_hour;
  String hoursStr = hours < 10 ? "0" + String(hours) : String(hours);

  uint8_t minutes = ti->tm_min;
  String minuteStr = minutes < 10 ? "0" + String(minutes) : String(minutes);

  uint8_t seconds = ti->tm_sec;
  String secondStr = seconds < 10 ? "0" + String(seconds) : String(seconds);

  return yearStr + ":" + monthStr + ":" + dayStr + "T" +
         hoursStr + ":" + minuteStr + ":" + secondStr;
}

// JSON Message: Create a payload that contains the sensor data in JSON format.
void CreateMessage(char *message)
{
  // Get sensor readings.
  const float temp = GetTemp();
  const float humidity = GetHumidity();
  const float o2 = O2Concentration();
  const float co2 = GetCO2();
  const float accel = GetAccelMag();
  //  const float accel = 1.0;

  // Estimate ethylene and shelf life.
  const float pO2 = 715 * (0.21 + (0.0004 * o2)) - (9.26 * (co2 / 10000));
  const float ethylene = (0.14 * pO2) / (0.99 * pO2);
  const float shelfLife = 15.7829 * ethylene;

  // Spoof coordinates.
  const double lat = coordsLat[mapCount];
  const double lon = coordsLon[mapCount];

  // Create message in JSON format.
  StaticJsonDocument<256> doc;
  doc["Time"].set(GetTimeStampString());
  doc["Temp"].set(temp);
  doc["Humidity"].set(humidity);
  doc["O2"].set(o2);
  doc["CO2"].set(co2);
  doc["Accel"].set(accel);
  doc["ShelfLife"].set(shelfLife);
  doc["Ethylene"].set(ethylene);
  doc["Lat"].set(lat);
  doc["Lon"].set(lon);
  // Rotate coordinates.
  mapCount++;

  if (mapCount >= mapSize) {
    mapCount = 0;
  }

  // Copy message to message.
  serializeJson(doc, message, 256);
}
