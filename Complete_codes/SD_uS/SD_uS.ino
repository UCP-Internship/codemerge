// SD card
#include <SPI.h>
#include <SD.h>

// Pressure / temperature
#include <Wire.h>
#include <BMP280.h>

// Ultrasound sensor
#include <NewPing.h>

#define MAX_DISTANCE 350 // Sensor doesn't detect farther than 3.50 m
#define TRIGGER_PIN  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     9   // Arduino pin tied to echo pin on the ultrasonic sensor.

// Declare the Us sensor
NewPing sensor(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Declare the pressure / temp sensor
BMP280 bmp;

// Declare the file on the SD card
File US_data;

// Variables
  // Ultrasound
  int nb_people = 0; // People counter
  int distance; // Distance to sensor
  unsigned int echo_time; // Time it takes to the signal to come back to the sensor
  boolean state = 0; // What is happening in front of the sensor (no one / someone)
  
  // Pressure / temperature
  double temp, pressure;
  char bmpStatus;

void setup()
{
  Serial.begin(115200);
  while(!Serial){;}
  
  // Initialize the pressure/temperature sensor
  if(!bmp.begin())
  {
    Serial.println("BMP init failed");
  }
  bmp.setOversampling(4); // resolution of measurements
  
  // Initialize the SD card
  if(!SD.begin(4))
  {
    Serial.println("Failed to initialize. Stopping now");
    // stop there
    for(;;);
  }
  pinMode(10, OUTPUT);
  // Create file on the SD card and close it instantly
  US_data = SD.open("nb_ppl.txt", FILE_WRITE);
  US_data.close();
  if(!SD.exists("nb_ppl.txt"))
  {
    Serial.println("Failed to create 'nb_ppl.txt' file. Stopping now...");
    for(;;);
  }
  Serial.println("starting");
}

void loop()
{
  delay(50); // Send ping about 20 times per second
  
  bmpStatus = bmp.startMeasurment();
  if (bmpStatus == 0) 
  {
    Serial.println(bmp.getError());
    return;
  }
  
  unsigned int echo_time = sensor.ping(); // Get time pings takes to come back in micro seconds
  distance = sensor.convert_cm(echo_time); // Converts time to distance in cm
  
  // Printing part
  Serial.print("distance = ");
  Serial.println(distance);
  
  // Update distance value and write on SD card if needed
  state = ppl_counter(state, distance, &nb_people);
}

// Manages people counter
int ppl_counter(int temp_state, int temp_dist, int *nb_people)
{
  // No one in front of sensor yet
  if(state == 0)
  {
    // Person in front of device, move to next step
    if(distance < 100)
      return 1; 
    // No one in front of the device, wait for a person to arrive
    else
      return 0;
  }
  
  // Someone has been detected
  if(state == 1)
  {
    // Nobody in front of device anymore, add 1 person to counter and wait for another person
    if(distance >= 100)
    {
      write_file(nb_people);
      return 0;
    }
    // Person still in front of device, wait for him to leave
    else
      return 1;
  }
}

// Changes nb_people value and writes new value on the SD card
void write_file(int *ppl)
{
  (*ppl) += 1;
  US_data = SD.open("nb_ppl.txt", FILE_WRITE);
  US_data.println(nb_people);
  US_data.close();
  SD.remove("nb_ppl.txt");
  state == 0;
  Serial.print("SD card updated, one more person! Total of people :");
  Serial.println(*ppl);
}
