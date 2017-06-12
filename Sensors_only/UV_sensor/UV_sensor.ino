
#include <Wire.h>

#define I2C_ADDR_UV 0x38
// integration times
#define IT_0_5 0x0 // 0.5 T
#define IT_1   0x1 // 1 T
#define IT_2   0x2 // 2 T
#define IT_4   0x3 // 4 T

// reference value: 0.01 W/m^2 corresponds to the UV-index 0.4
float refVal = 0.4;

void setup() {
  
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(I2C_ADDR_UV);
  Wire.write((IT_1<<2) | 0x02);
  Wire.endTransmission();
  delay(500);
  
}


void loop() {

  byte msb=0, lsb=0; // first and second byte that will be read from the sensor
  uint16_t uv;

  Wire.requestFrom(I2C_ADDR_UV+1, 1); // MSB (read first byte from sensor)
  delay(1);
  if(Wire.available()) {
    msb = Wire.read();
  }

  Wire.requestFrom(I2C_ADDR_UV+0, 1); // LSB (read second byte from sensor)
  delay(1);
  if(Wire.available()) {
    lsb = Wire.read();
  }

  uv = (msb<<8) | lsb; // combine bytes to an integer through a bitshift op

  Serial.print("μW per cm²: ");
  Serial.println(uv, DEC);     // log value as 16bit integer
  Serial.print("UV-Index: ");
  Serial.println(getUVI(uv));

  delay(1000);

}

/*
 * getUVI()
 * expects the measurement value from the UV-sensor as input
 * and returns the corresponding value on the UV-index
 */
 
float getUVI(int uv) {
  float uvi = refVal * (uv * 5.625) / 1000;
  return uvi;
}
