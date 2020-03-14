#include <Wire.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

/* 
 *  Accelerometer: LIS3DH
 *  Pins: SLC(GPIO22), SDA(GPIO21), VCC, GND
 */

Adafruit_LIS3DH accel = Adafruit_LIS3DH();
// I2C address of accel sensor.
const uint8_t accelAddress = 0x18;

// Set range to 4G, and I2C address to 0x18.
void SetupAccel()
{
  Serial.println(F("Accel setting up..."));

  // Set I2C address. Do not continue if not found.
  if (!accel.begin(accelAddress))
  {
    Serial.println(F("Accel fault!"));
    while (1);
  }
  // Set range.
  accel.setRange(LIS3DH_RANGE_4_G);

  Serial.print(F("Accel range is "));
  Serial.print(2 << accel.getRange());
  Serial.println("G.");
  Serial.println(F("Accel setting up completed."));
}

// Get accel reading as magnitude of x, y, and z.
float GetAccelMag()
{
  // Get readings.
  sensors_event_t event;
  accel.getEvent(&event);

  // Check if the readings are successful. That is, not equal to nan.
  if (isnan(event.acceleration.x) || isnan(event.acceleration.y) || isnan(event.acceleration.z)) 
  {
    return 0.0;
  } 
  else 
  {
   // Readings at rest yeild 9.4m/s^2. Need to offset by 9.4.
  return sqrt(sq(event.acceleration.x) + sq(event.acceleration.y) + sq(event.acceleration.z)) - 9.4;
  }
}
