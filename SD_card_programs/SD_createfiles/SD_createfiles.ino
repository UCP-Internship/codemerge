#include <SPI.h>
#include <SD.h>

// Create a file
File example;
int reponse;

void setup()
{
  Serial.begin(9600);
  
  // wait till the monitor is opened
  while(!Serial)
  {
    ;
  }
  
  // Initialize the SD card
  if(!SD.begin(4))
  {
    Serial.println("Failed to initialize");
    // stop there
    for(;;);
  }
  
  // Check the presence of example.txt
  if(SD.exists("example.txt"))
  {
    Serial.println("example.txt already exists.");
  }
  else
  {
    Serial.println("example.txt doesn't exist yet.");
  }
  
  Serial.println("Creating example.txt");
  
  // Creates example.txt file and closes it immediately
  example = SD.open("example.txt", FILE_WRITE);
  example.close();
  
  if(SD.exists("example.txt"))
  {
    Serial.println("example.txt has been created.");
  }
  else
  {
    Serial.println("Error creating example.txt file");
  }
  // Deletes example.txt
  SD.remove("example.txt");
  if(!SD.exists("example.txt"))
  {
    Serial.println("example.txt has been deleted.");
  }
}

void loop()
{
  
}
