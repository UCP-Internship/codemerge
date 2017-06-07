#include <Wire.h>
#include <HDC100X.h>

HDC100X hdc(0x43);

float humidity, temperature;

void setup()
{
  Serial.begin(9600);
  hdc.begin(HDC100X_TEMP_HUMI, HDC100X_14BIT, DISABLE);
}

void loop()
{
  humidity = hdc.getHumi();
  temperature = hdc.getTemp();
  //  ##############################
  //  ########## Affichage #########
  //  ##############################
  Serial.print("Temperature = ");
  Serial.println(temperature);
  
  Serial.print("Humidity = ");
  Serial.println(humidity);
}
