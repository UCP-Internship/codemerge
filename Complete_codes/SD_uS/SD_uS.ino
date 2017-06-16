// SD card
#include <SPI.h>
#include <SD.h>

// Ultrasound sensor
#include <NewPing.h>

#define MAX_DISTANCE 300 // Sensor doesn't detect farther than 3 m
#define TRIGGER_PIN  11  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     10  // Arduino pin tied to echo pin on the ultrasonic sensor.

// Declare the sensor
NewPing sensor(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Declare the file on the SD card
File US_data;

// Variables
int nb_people = 0; // People counter
int distance; // Distance to sensor
unsigned int echo_time; // Time it takes to the signal to come back to the sensor
int state = 0; // What is happening in front of the sensor (no one / someone)

void setup()
{
  Serial.begin(115200);
  
  // Initialize the SD card
  if(!SD.begin(4))
  {
    Serial.println("Failed to initialize. Stopping now");
    // stop there
    for(;;);
  }
  
  // Create file on the SD card and close it instantly
  US_data = SD.open("nb_ppl.txt", FILE_WRITE);
  US_data.close();
  if(!SD.exists("nb_ppl.txt"))
  {
    Serial.println("Failed to create 'nb_ppl.txt' file. Stopping now...");
    for(;;);
  }
}

void loop()
{
  delay(50); // Send ping around 20 times per second
  echo_time = sensor.ping(); // Get time pings takes to come back in micro seconds
  distance = sensor.convert_cm(echo_time); // Converts time to distance in cm
  Serial.println(distance);
  Serial.println(echo_time);
  state = ppl_counter(state, distance, nb_people);
  
  // If the sensor is stuck, reset it.
  if(echo_time == 0 && digitalRead(ECHO_PIN) == HIGH) 
  {
    pinMode(ECHO_PIN, OUTPUT);
    digitalWrite(ECHO_PIN, LOW);
    delay(100);
    pinMode(ECHO_PIN, INPUT);
  }
}

// Manages people counter
int ppl_counter(int temp_state, int temp_dist, int nb_people)
{
  // No one in front of sensor yet
  if(state == 0)
  {
    // Person in front of device, move to next step
    if(distance < 100)
      return 1; 
    // No one in front of the device, for a person to arrive
    else
      return 0;
  }
  
  // Someone has been detected
  if(state == 1)
  {
    // Nobody in front of device anymore, add 1 person to counter and wait for another person
    if(distance >= 100)
    {
      write_file(&nb_people);
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
  *ppl += 1;
  US_data = SD.open("nb_ppl.txt", FILE_WRITE);
  US_data.println(*ppl);
  US_data.close();
  Serial.print("SD card updated, one more person! Total of people :");
  Serial.println(*ppl);
}
