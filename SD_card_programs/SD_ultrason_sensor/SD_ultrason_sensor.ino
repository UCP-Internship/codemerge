#include <SD.h>
#include <SPI.h>

File nb_ppl;

const int trigPin = 9;
const int echoPin = 10;

float distance;
int state = 0;
int nb_people;

void setup() 
{
  while(!Serial){;}
  // Check initialization statue and stop if it's 0
  if(!SD.begin(4))
  {
    Serial.println("Failed to initialize SD card");
    for(;;);
  }
  
  // define the signals of the ultrasound device
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  Serial.begin(9600);
}

void loop() 
{
  delay(500);
  
  distance = getDistance();
  
  // Display the distance to the sensor
  //Serial.print("Distance: ");
  //Serial.println(distance);
  
  // If there isn't anyone in front of the sensor
  if(state == 0)
  {  
    if(distance < 70 && distance > 0)
    {
      state ++;
    }
  }
  // There's someone in front of the sensor
  if(state == 1)
  {
    distance = getDistance();
    
    // Change state if there isn't anyone in front of the sensor anymore
    if(distance > 70)
    {
      state = 2;
    }
    
    // Display distance to person
    //Serial.print("Distance: ");
    //Serial.println(distance);
  }
  if(state == 2)
  {
    // Add people 
    nb_people ++;
  
    // No one in front of sensor anymore
    //state = 0;
  
    // Display the new number
    Serial.print("Number of people: ");
    Serial.println(nb_people);
  
    // Write the value in a txt file on the SD card then close it
    nb_ppl = SD.open("nb_people.txt");
    nb_ppl.println(nb_people); 
    nb_ppl.close();
    
    // No one in front of sensor anymore
    state = 0;
  }
  
}

int getDistance()
{
  float temp_distance, duration;
  
  // Activate sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Get the duration it takes for the waves to come back
  duration = pulseIn(echoPin, HIGH);
  
  // Arbitrary calculation to get distance value
  temp_distance = duration*0.034/2;
  return temp_distance;
}
