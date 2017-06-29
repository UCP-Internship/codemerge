// #################################
// This is the code for the elevator
// #################################

// SD card
#include <SPI.h>
#include <SD.h>

// humidity / temperature
#include <Wire.h>
#include <BMP280.h>

// Ultrasound sensor
#include <NewPing.h>

// Constants
#define MAX_DISTANCE 399 // Sensor doesn't detect farther than 3.90 m
#define TRIGGER_PIN  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     9   // Arduino pin tied to echo pin on the ultrasonic sensor.
#define UPDATE_TIME 60000 // Write on the SD card every x loop

// Declare the Us sensor
NewPing sensor(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Declare the humidity/temperature sensor
BMP280 bmp;

// Declare the file on the SD card
File elevator_data;

// Variables
  // General
  unsigned long uptime = 0;
  
  // Ultrasound
  int dist_max;
  int nb_people = 0; // People counter
  int distance; // Distance to sensor
  unsigned int echo_time; // Time it takes to the signal to come back to the sensor
  boolean state = 0; // What is happening in front of the sensor (no one / someone)
  
  // Humidity/Temperature
  double pressure, temperature;
  char bmpStatus;

void setup()
{
  Serial.begin(115200);
  //while(!Serial){;}
  
  // Initialize the pressure/temperature sensor and stop if it failed
  if(!bmp.begin())
  {
    Serial.println("BMP initalization failed");
    for(;;);
  }
  
  // Initialize the SD card
  if(!SD.begin(4))
  {
    Serial.println("Failed to initialize. Stopping now...");
    for(;;);
  }
  bmp.setOversampling(4); // select resolution of the measurements
  pinMode(10, OUTPUT); 
  
  // Creates file on the SD card and closes it instantly
  elevator_data = SD.open("eledatatr.csv", FILE_WRITE);
  elevator_data.close();
  
  echo_time = sensor.ping();
  dist_max = sensor.convert_cm(echo_time)-20;
  if(dist_max < 250)
    dist_max = 250;
  Serial.print("dist_max = ");
  Serial.println(dist_max);
  
  // If the file wasn't created, stop there
  if(!SD.exists("eledatatr.csv"))
  {
    Serial.println("Failed to create 'eledatatr.csv' file. Stopping now...");
    for(;;);
  }
  // Start 
  Serial.println("starting");
}

void loop()
{
  delay(50); // Send ping about 20 times per second
  
  // Upload values on SD card every 20 seconds
  if(millis() - uptime > UPDATE_TIME)
  {
    write_file(pressure, temperature, uptime);
    
      Serial.println("\n==============================");
      File myFile = SD.open("eledatatr.csv");
      if (myFile) 
      {
        Serial.println("eledatatr.csv:");
  
        // read from the file until there's nothing else in it:
        while (myFile.available()) 
        {
          Serial.write(myFile.read());
        }
        // close the file:
        myFile.close();
      } 
      else 
      {
        // if the file didn't open, print an error:
        Serial.println("error opening eledatatr.csv");
      }
      Serial.println("\n==============================");    
    
    uptime = millis();
  }
  
  // Measure 
    // Pressure/Temperature
    bmpStatus = bmp.startMeasurment();
    if (bmpStatus == 0) 
    {
      return;
    }
    delay(bmpStatus); //Durée de la mesure
    bmpStatus = bmp.getTemperatureAndPressure(temperature, pressure);
    
    // Ultrasound
    unsigned int echo_time = sensor.ping(); // Get time pings takes to come back in micro seconds
    distance = sensor.convert_cm(echo_time); // Converts time to distance in cm
  
  // Update distance value
  state = ppl_counter(state, distance, dist_max);
}

// Manages people counter
int ppl_counter(int temp_state, int temp_dist, int dist_to_ppl)
{
  // No one in front of sensor yet
  if(temp_state == 0)
  {
    // Person in front of device, move to next step
    if(temp_dist < dist_to_ppl)
      return 1; 
    // No one in front of the device, wait for a person to arrive
    else
      return 0;
  }
  
  // Someone has been detected
  if(temp_state == 1)
  {
    // Nobody in front of device anymore, add 1 person to counter and wait for another person
    if(temp_dist >= dist_to_ppl)
    {
      update_ppl();
      return 0;
    }
    // Person still in front of device, wait for him to leave
    else
      return 1;
  }
}

// Writes new values on the SD card
void write_file(int pressure, int temperature, unsigned long uptime)
{
  elevator_data = SD.open("eledatatr.csv", FILE_WRITE);
  elevator_data.print(uptime);
  elevator_data.print(";");
  elevator_data.print(nb_people);
  elevator_data.print(";");
  elevator_data.print(pressure);
  elevator_data.print(";");
  elevator_data.print(temperature);
  elevator_data.println(";");
  elevator_data.close();
}

// Update the people counter
void update_ppl()
{
  nb_people += 1;
  Serial.print("One more person! Total of people :");
  Serial.println(nb_people);
}
