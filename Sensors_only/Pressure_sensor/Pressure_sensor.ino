#include <Wire.h>
#include <BMP280.h>

BMP280 bmp;

double temp, pressure;
char bmpStatus;

void setup(){

  Serial.begin(9600);
  if(!bmp.begin())
  {
    Serial.println("BMP init failed");
  }
  bmp.setOversampling(4); // select resolution of the measurements
}

void loop() {
  bmpStatus = bmp.startMeasurment();
  if (bmpStatus == 0) 
  {
    Serial.println(bmp.getError());
    return;
  }
  delay(bmpStatus); //Durée de la mesure
  bmpStatus = bmp.getTemperatureAndPressure(temp, pressure);
  Serial.print("temperature : ");
  Serial.print(temp);
  Serial.println("°C");
  
  Serial.print("pressure : ");
  Serial.println(pressure);
  Serial.println("hPa");
}
