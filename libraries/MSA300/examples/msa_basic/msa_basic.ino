#include <MSA300.h>
#include <Wire.h>

// This is the length of the string that will be created
// included minus and decimal point
const signed char formattedStringLength = 11;

//  The number of digits after the deimal point to print
const unsigned char numVarsAfterDecimal = 6;

acc_t data;
MSA300 msa;

static char outstr[formattedStringLength];

char * formatValue(float value)
{
  dtostrf(value, formattedStringLength, numVarsAfterDecimal, outstr);
  return outstr;
}

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  msa.begin();
}

void loop()
{
  data = msa.getAcceleration();

  Serial.printf("Xa:%s ", formatValue(data.x));
  Serial.printf("Ya:%s ", formatValue(data.y));
  Serial.printf("Za:%s", formatValue(data.z));
  Serial.println();

  delay(100);
}
