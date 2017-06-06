#include <Wire.h>
#include <BMP280.h>
#include <HDC100X.h>

BMP280 bmp;

#define I2C_ADDR    (0x29)
#define REG_CONTROL  0x00
#define REG_CONFIG   0x01
#define REG_DATALOW  0x04
#define REG_DATAHIGH 0x05
#define REG_ID       0x0A

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
}






void loop() {

/* Light sensor */

LightSensor();

/* Air Pressure + Temperature */  

PressureTemp();

/* Humidity */

humidity();

}

void LightSensor()
{  
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(0x80 | REG_DATALOW);
  Wire.endTransmission();
  Wire.requestFrom(I2C_ADDR, 2); // Request 2 bytes
  uint16_t low = Wire.read();
  uint16_t high = Wire.read();
  Serial.print("light : ");
  Serial.println(low);

  while (Wire.available()) {
    Wire.read();
  }
  uint32_t lux;
  lux = (high << 8) || (low << 0);
  lux *= 1; // Multiplier for 400ms  
}

void PressureTemp()
{
  double temp, pressure;
  char bmpStatus = bmp.startMeasurment();

  // if an error occured on the sensor: stop
  if (bmpStatus == 0) {
  Serial.println(bmp.getError());
  return;
  }

  delay(bmpStatus); // wait for duration of the measurement
  bmpStatus = bmp.getTemperatureAndPressure(temp, pressure);

  Serial.print("Temperature :");
  Serial.print(temp);
  Serial.println(" °C");
  Serial.print("Pressure :");
  Serial.print(pressure);
  Serial.println(" hPa");
}

void humidity()
{
  Serial.print(hdc.getHumi());
  Serial.println(" % of humidity");
  Serial.println();
}

 
