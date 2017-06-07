#include "sensors.h"

BMP280 bmp;

#define I2C_ADDR    (0x29)
#define REG_CONTROL  0x00
#define REG_CONFIG   0x01
#define REG_DATALOW  0x04
#define REG_DATAHIGH 0x05
#define REG_ID       0x0A
#define I2C_ADDR_UV 0x38
// integration times
#define IT_0_5 0x0 // 0.5 T
#define IT_1   0x1 // 1 T
#define IT_2   0x2 // 2 T
#define IT_4   0x3 // 4 T

// reference value: 0.01 W/m^2 corresponds to the UV-index 0.4
float refVal = 0.4;

HDC100X hdc(0x43);



void setup() {

  /* Light Sensor */

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

  /* Air Pressure + temperature */

  if (!bmp.begin()) {
    Serial.println("BMP init failed!");
  }
  bmp.setOversampling(4); // select resolution of the measurements 

  /* Humidity */

  hdc.begin(HDC100X_TEMP_HUMI, HDC100X_14BIT, DISABLE);

  /* UV */
  Wire.begin();
  Wire.beginTransmission(I2C_ADDR_UV);
  Wire.write((IT_1<<2) | 0x02);
  Wire.endTransmission();
  delay(500);
}



void loop() {

// Light sensor 

LightSensor();

/* Air Pressure + Temperature */  

PressureTemp();

/* Humidity */

humidity();
  
/* UV */

UVSensor();

}
