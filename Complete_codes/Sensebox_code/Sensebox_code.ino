#include <HDC100X.h>
#include <SPI.h>
#include <WiFi101.h>
#include <Wire.h>
#include <SD.h>
#include <NewPing.h>


 File US_data;

int nb_people = 0; // People counter
int distance; // Distance to sensor
unsigned int echo_time; // Time it takes to the signal to come back to the sensor
boolean state = 0; // What is happening in front of the sensor (no one / someone)

HDC100X hdc(0x43);

//senseBox ID
#define SENSEBOX_ID "5947c6c8a4ad5900112f31e3"

//Sensor IDs
#define SENSOR1_ID "5947c6c8a4ad5900112f31e7" // Attendance
#define SENSOR2_ID "5947c6c8a4ad5900112f31e6" // Sound
#define SENSOR3_ID "5947c6c8a4ad5900112f31e5" // Humidity
#define SENSOR4_ID "5947c6c8a4ad5900112f31e4" // Temperature
#define MAX_DISTANCE 350 // Sensor doesn't detect farther than 3.50 m
#define TRIGGER_PIN  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN 9 // Arduino pin tied to echo pin on the ultrasonic sensor.

NewPing sensor(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

//Ethernet-Parameter
IPAddress server(52,57,90,92);
WiFiClient client;

char ssid[] = "etis-midi";     //  your network SSID (name)
char pass[] = "%midi564";  // your network password
int status = WL_IDLE_STATUS;     // the WiFi radio's status

//Messparameter
int postingInterval = 0; //Uploadintervall in Millisekunden

// Microphone
int mic = A0;
long micVal = 0;

float temperature, humidity, sound_lvl;


void setup () {
  Serial.begin(115200);
   if(!SD.begin(4))
  {
    Serial.println("Failed to initialize. Stopping now...");
    for(;;);
  }

  pinMode(10, OUTPUT); 
  pinMode(mic, INPUT);
  US_data = SD.open("nb_ppl.txt", FILE_WRITE);
  US_data.close();
  if(!SD.exists("nb_ppl.txt"))
  {
    Serial.println("Failed to create 'nb_ppl.txt' file. Stopping now...");
    for(;;);
  }
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(2000);
  }   
  Serial.println("done!");
  delay(1000);
  Serial.println("Starting loop.");

  hdc.begin(HDC100X_TEMP_HUMI, HDC100X_14BIT, DISABLE); 
}

void loop () {
  /*
     Hier Sensoren auslesen und nacheinerander über postFloatValue(...) hochladen. Beispiel:

     float temperature = sensor.readTemperature();
     postFloatValue(temperature, 1, temperatureSensorID);
  */

  if(postingInterval < 6480 )
  {
  float temperature = hdc.getTemp();
  float humidity = hdc.getHumi();
  
   // Ultrasound
  unsigned int echo_time = sensor.ping(); // Get time pings takes to come back in micro seconds
  distance = sensor.convert_cm(echo_time); // Converts time to distance in cm
  
  
  float sound_lvl;
  
    // Microphone 
  sound_lvl = getMicVal();
  postingInterval += 1;
  state = ppl_counter(state, distance, nb_people);
  }
  else
  {
      postFloatValue(sound_lvl, 1, SENSOR2_ID);
      postFloatValue(nb_people, 1, SENSOR1_ID);
      postFloatValue(temperature, 1, SENSOR4_ID);
      postFloatValue(humidity, 1, SENSOR3_ID); 
      postingInterval -= 6480;
  }
}

void postFloatValue (float measurement, int digits, String sensorId) {
  //Float zu String konvertieren
  char obs[10];
  dtostrf(measurement, 5, digits, obs);
  //Json erstellen
  String jsonValue = "{\"value\":";
  jsonValue += obs;
  jsonValue += "}";
  //Mit OSeM Server verbinden und POST Operation durchführen
  Serial.println("-------------------------------------");
  Serial.print("Connectingto OSeM Server...");
  if (client.connect(server, 80)) {
    Serial.println("connected!");
    Serial.println("-------------------------------------");
    //HTTP Header aufbauen
    client.print("POST /boxes/"); client.print(SENSEBOX_ID); client.print("/"); client.print(sensorId); client.println(" HTTP/1.1");
    client.print("Host:");
    client.println(server);
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.print("Content-Length: "); client.println(jsonValue.length());
    client.println();
    //Daten senden
    client.println(jsonValue);
  } else {
    Serial.println("failed!");
    Serial.println("-------------------------------------");
  }
  //Antwort von Server im seriellen Monitor anzeigen
  waitForServerResponse();
}

void waitForServerResponse () {
  //Ankommende Bytes ausgeben
  boolean repeat = true;
  do {
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
    //Verbindung beenden
    if (!client.connected()) {
      Serial.println();
      Serial.println("--------------");
      Serial.println("Disconnecting.");
      Serial.println("--------------");
      client.stop();
      repeat = false;
    }
  } while (repeat);
}

// millis() rollover fix - http://arduino.stackexchange.com/questions/12587/how-can-i-handle-the-millis-rollover
void sleep(unsigned long ms) {            // ms: duration
  unsigned long start = millis();         // start: timestamp
  for (;;) {
    unsigned long now = millis();         // now: timestamp
    unsigned long elapsed = now - start;  // elapsed: duration
    if (elapsed >= ms)                    // comparing durations: OK
      return;
  }
}

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
