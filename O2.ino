/*
    O2 Sensor
    Pins: Signal(A)(GPIO34), VCC, GND
*/

// O2 Pin
const uint8_t o2Pin = 34;
// Not sure if this is used. But it is defined in the example.
#define O2_preheat_time 60000
// For getting calibration voltage once.
bool initFlag = false;
// Keep the calibration voltage.
float calibrationVoltage = 0.0;

void SetupGroveO2() {
  Serial.println(F("O2 setting up..."));
  O2Concentration();
  Serial.println(F("O2 setting up completed."));
}

float O2Concentration()
{
  unsigned int sum = 0;

  // Get the calibration voltage once only.
  if (!initFlag)
  {
    // Do this only once.
    initFlag = true;
    pinMode(o2Pin, INPUT);

    // Get an average of 64 readings.
    for (uint8_t i = 64; i > 0; i--)
    {
      sum = sum + analogRead(o2Pin);
      delay(100);
    }
    sum = sum >> 6;

    // Calculate and set the calibreation voltage.
    calibrationVoltage = sum / 20.80;

    return 20.80;
  }
  // After finding the calibration voltage, find the O2 concentration.
  else
  {
    // Get an average of 32 readings.
    for (uint8_t i = 32; i > 0; i--)
    {
      sum = sum + analogRead(o2Pin);
      delay(50);
    }
    sum = sum >> 5;

    // Calculate and return O2 concentration.
    return sum / calibrationVoltage;
  }
}
