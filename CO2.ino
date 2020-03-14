/* 
 *  CO2 Sensor: Gravity Infrared CO2 SensorPins: 
 *  Signal(A)(GPIO35), VCC, GND 
 */

const uint8_t co2Pin = 35;

// Read and return CO2 sensor value. Return 0 if there is an error, or still preheating.
float GetCO2() {
  // ESP32 ADC's are 12-bit (2^12 = 4096) with a reference voltage of 3.3V (3300mV). 
  const float voltage = analogRead(co2Pin) * (3300.0 / 4095.0);

  // Error. 
  if (voltage == 0)
  {
    Serial.println(F("CO2 sensor fault!"));
  }
  // Voltages below 400mV means it's still heating up.
  else if (voltage < 400)
  {
    Serial.println(F("CO2 sensor preheating..."));
  }
  else
  {
    // Zero ppm reading begins at 400mV. 
    return (voltage - 400) * 50.0 / 16.0;
  }

  delay(100);
  return 0.0;
}
