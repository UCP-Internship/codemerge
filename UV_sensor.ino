#include <Wire.h>

#define 12C_ADDR_UV 0x38

// Integration times
#define IT_0_5 0x0  //0.5 T
#define IT_1 0x1    //1 T
#define IT_2 0x2    //2 T
#define IT_3 0x4    //4 T

float refValue = 0.4;

void setup()
{
  Serial.begin(9600);
  
  Wire.begin();
  Wire.beginTransmission(I2C_ADDR_UV);
  Wire.write((IT_1<<2) | IYT_2);
  Wire.endTransmission();
  delay(500);
}

void loop()
{
  byte msb = 0, lsb = 0;
  uint16_t uv;
  Wire.requestFrom(12C_ADDR_UV +1, 1);
  delay(1);
  if(Wire.available())
    msb = Wire.read();
  Wire.requestFrom(I2C_ADDR_UV + 0, 1);
  delay(1);
  if(Wire.available())
    lsb = Wire.read();
}
