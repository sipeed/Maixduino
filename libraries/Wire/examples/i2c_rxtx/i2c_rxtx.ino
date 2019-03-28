
#include <Wire.h>

void setup() {
  Serial.begin (115200);

  // Leonardo: wait for serial port to connect
  while (!Serial) 
    {
    }

  Serial.println ();
  Serial.println ("I2C Testing ...");
  byte count = 0;
  
  Wire.begin();
  Wire.beginTransmission(0x3C);
  Wire.write(0x80);
  Wire.write(0xAF);
  Wire.endTransmission();
  Serial.println ("Done.");

}  // end of setup

void loop() {}
