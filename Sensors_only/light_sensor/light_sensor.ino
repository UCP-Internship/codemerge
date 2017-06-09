#include <Wire.h>
#define I2C_ADDR (0x29)
#define REG_CONTROL 0x00
#define REG_CONFIG 0x01
#define REG_DATALOW 0x04
#define REG_DATAHIGH 0x05
#define RED_ID 0x0A

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0x80 | REG_CONTROL);
  Wire.write(0x03); // Power on
  Wire.endTransmission();
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0x80 | REG_CONFIG);
  Wire.write(0x00); // 400 ms
  Wire.endTransmission();
}

void loop()
{
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0x80 | REG_DATALOW);
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDR, 2); // Request 2 bytes
  uint16_t low = Wire.read();
  Serial.print("low = ");
  Serial.println(low);
  uint16_t high = Wire.read();
  Serial.print("high = ");
  Serial.println(high);
  while (Wire.available())
  {
    Wire.read();
  }
  uint32_t lux;
  lux = (high << 8) | (low << 0);
  lux *= 1; // Multiplier for 400ms
  Serial.print("lux = ");
  Serial.println(lux);
  Serial.println(" ");
}
