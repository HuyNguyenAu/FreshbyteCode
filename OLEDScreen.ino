#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);

void ClearDisplay()
{
  display.clearDisplay();
  display.setCursor(0, 0);
}

void SetupDisplay()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
  }
  delay(2000);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  ClearDisplay();
}

void WriteToScreen(const char* text)
{
  ClearDisplay();
  display.println(text);
  display.display();
}
