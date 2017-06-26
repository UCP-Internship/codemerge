// ###################################
// This is the code for the front door
// ###################################

// SD card
#include <SPI.h>
#include <SD.h>

// humidity / temperature
#include <Wire.h>
#include <HDC100X.h>

// Ultrasound sensor
#include <NewPing.h>

// Constants
#define MAX_DISTANCE 390 // Sensor doesn't detect farther than 3.90 m
#define TRIGGER_PIN  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     9   // Arduino pin tied to echo pin on the ultrasonic sensor.
#define dist_to_ppl 150 // Distance under which it is considered that someone is in front of the sensor
#define MIC_PORT A0  // Microphone port
#define UPDATE_TIME 213 // Write on the SD card every x loop

// Declare the Us sensor
NewPing sensor(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Declare the humidity/temperature sensor
HDC100X hdc(0x43);

// Declare the file on the SD card
File door_data;

// Variables
  // General
  int counter = 0;
  unsigned long uptime = 0;
  
  // Ultrasound
  int nb_people = 0; // People counter
  int distance; // Distance to sensor
  unsigned int echo_time; // Time it takes to the signal to come back to the sensor
  boolean state = 0; // What is happening in front of the sensor (no one / someone)
  
  // Humidity/Temperature
  float humidity, temperature;
  
  // Microphone
  long sound_lvl;

void setup()
{
  //Serial.begin(115200);
  //while(!Serial){;}
  
  // Initialize the humidity/temperature sensor
  hdc.begin(HDC100X_TEMP_HUMI, HDC100X_14BIT, DISABLE);
  
  // Initialize the SD card
  if(!SD.begin(4))
  {
    //Serial.println("Failed to initialize. Stopping now...");
    for(;;);
  }
  
  pinMode(10, OUTPUT); 
  
  pinMode(MIC_PORT, INPUT);
  
  // Creates file on the SD card and closes it instantly
  door_data = SD.open("doordata.txt", FILE_WRITE);
  door_data.close();
  // If the file wasn't created, stop there
  if(!SD.exists("doordata.txt"))
  {
    //Serial.println("Failed to create 'doordata.txt' file. Stopping now...");
    for(;;);
  }
  // Start 
  //Serial.println("starting");
}

void loop()
{
  delay(50); // Send ping about 20 times per second
  
  // Time since the beginning of the program in milliseconds
  uptime = millis();
  
  // Upload values on SD card every 20 seconds
  if(counter == UPDATE_TIME)
  {
    write_file(nb_people, humidity, temperature, sound_lvl, uptime);
  }
  
  // Measure 
    // Humidity/Temperature
    humidity = hdc.getHumi();
    temperature = hdc.getTemp();
    
    // Ultrasound
    unsigned int echo_time = sensor.ping(); // Get time pings takes to come back in micro seconds
    distance = sensor.convert_cm(echo_time); // Converts time to distance in cm
    
    // Microphone
    sound_lvl = getMicVal();
  
  // Update distance value
  state = ppl_counter(state, distance, &nb_people);
  
  // Number of loops done
  counter ++;
}

// Manages people counter
int ppl_counter(int temp_state, int temp_dist, int *nb_people)
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
      update_ppl(nb_people);
      return 0;
    }
    // Person still in front of device, wait for him to leave
    else
      return 1;
  }
}

// Writes new values on the SD card
void write_file(int ppl, int humidity, int temperature, int snd_lvl, unsigned long uptime)
{
  door_data = SD.open("doordata1.csv", FILE_WRITE);
  door_data.print(uptime);
  door_data.print(";");
  door_data.print(ppl);
  door_data.print(";");
  door_data.print(humidity);
  door_data.print(";");
  door_data.print(temperature);
  door_data.print(";");
  door_data.print(snd_lvl);
  door_data.println(";");
  door_data.close();
}

// Update the people counter
void update_ppl(int *nb_ppl)
{
  (*nb_ppl) += 1;
  //Serial.print("One more person! Total of people :");
  //Serial.println(*nb_ppl);
}

// Gets value of sound
long getMicVal() 
{
  long micVal = 0;
  int period = 3; // Averaging three values ​​in order to catch any 'outliers'
  int correction_value = 510;
  for (int i = 0; i < period; i++) 
  {
    // Computes the absolute value of the value to intercept negative deflections
    micVal = micVal + abs (analogRead(MIC_PORT) - correction_value);
    delay(5);
  }
  micVal = constrain(abs(micVal / period), 1, 500);
  return (micVal);
}
