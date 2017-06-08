#include "sensors.h"

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

void UVSensor()
{
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

float getUVI(int uv) {
  float uvi = refVal * (uv * 5.625) / 1000;
  return uvi;
}

void Microphone ()
{
    Serial.println(getMicVal());
}

long getMicVal() 
{
  int period = 3; // Averaging three values in order to catch any 'outliers'
  int correction_value = 510;
  for (int i = 0; i < period; i++) {
    // Computes the absolute value of the value to intercept negative deflections
    micVal = micVal + abs (analogRead(mic) - correction_value);
    delay(5);
  }
  micVal = constrain(abs(micVal / period), 1, 500);
  return (micVal);
}
