#include <Wire.h>
#include <BMP280.h>
#include <HDC100X.h>

void LightSensor();
// prints light value in lux

void PressureTemp();
// prints temperature in °C and pressure in hPa

void humidity();
// prints humidity percentage

void UVSensor();
// prints UV index
  float getUVI(int uv);
  // gets UV value
