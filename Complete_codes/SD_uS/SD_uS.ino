// SD card
#include <SPI.h>
#include <SD.h>

// humidity / temperature
#include <Wire.h>
#include <HDC100X.h>

// Ultrasound sensor
#include <NewPing.h>

#define MAX_DISTANCE 350 // Sensor doesn't detect farther than 3.50 m
#define TRIGGER_PIN  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     9   // Arduino pin tied to echo pin on the ultrasonic sensor.

// Declare the Us sensor
NewPing sensor(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Declare the humidity/temperature sensor
HDC100X hdc(0x43);

// Declare the file on the SD card
File US_data;

// Variables
  // Ultrasound
  int nb_people = 0; // People counter
  int distance; // Distance to sensor
  unsigned int echo_time; // Time it takes to the signal to come back to the sensor
  boolean state = 0; // What is happening in front of the sensor (no one / someone)
  
  // Humidity/Temperature
  float humidity, temperature;
  
  // Microphone
  int mic = A0;
  long micVal = 0;
  long sound_lvl;

void setup()
{
  Serial.begin(115200);
  while(!Serial){;}
  
  // Initialize the humidity/temperature sensor
  hdc.begin(HDC100X_TEMP_HUMI, HDC100X_14BIT, DISABLE);
  
  // Initialize the SD card
  if(!SD.begin(4))
  {
    Serial.println("Failed to initialize. Stopping now...");
    for(;;);
  }
  
  pinMode(10, OUTPUT); 
  
  pinMode(mic, INPUT);
  
  // Create files on the SD card and close it instantly
  US_data = SD.open("nb_ppl.txt", FILE_WRITE);
  US_data.close();
  if(!SD.exists("nb_ppl.txt"))
  {
    Serial.println("Failed to create 'nb_ppl.txt' file. Stopping now...");
    for(;;);
  }
  // Start 
  Serial.println("starting");
}

void loop()
{
  delay(50); // Send ping about 20 times per second
  
  // Measure 
    // Humidity/Temperature
    humidity = hdc.getHumi();
    temperature = hdc.getTemp();
    
    // Ultrasound
    unsigned int echo_time = sensor.ping(); // Get time pings takes to come back in micro seconds
    distance = sensor.convert_cm(echo_time); // Converts time to distance in cm
    
    // Microphone 
    sound_lvl = getMicVal();
  
  // Printing part
  Serial.print("distance = ");
  Serial.print(distance);
  Serial.println(" cm");
  
  Serial.print("temperature = ");
  Serial.print(temperature);
  Serial.println(" Celsius");
  
  Serial.print("Humidity = ");
  Serial.print(humidity);
  Serial.println(" %");
  
  Serial.print("Sound level = ");
  Serial.println(sound_lvl);
  
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
  SD.remove("nb_ppl.txt");
  US_data = SD.open("nb_ppl.txt", FILE_WRITE);
  US_data.println(nb_people);
  US_data.close();
  state == 0;
  Serial.print("SD card updated, one more person! Total of people :");
  Serial.println(*ppl);
}

// Get value of sound
long getMicVal() 
{
  int period = 3; // Averaging three values ​​in order to catch any 'outliers'
  int correction_value = 510;
  for (int i = 0; i < period; i++) 
  {
    // Computes the absolute value of the value to intercept negative deflections
    micVal = micVal + abs (analogRead(mic) - correction_value);
    delay(5);
  }
  micVal = constrain(abs(micVal / period), 1, 500);
  return (micVal);
}
