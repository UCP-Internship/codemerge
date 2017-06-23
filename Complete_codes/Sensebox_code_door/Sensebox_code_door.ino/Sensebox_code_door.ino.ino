#include <HDC100X.h>
#include <SPI.h>
#include <WiFi101.h>

int nb_people=129;
long duration;
int distance;
int state = 0;
HDC100X hdc(0x43);

//senseBox ID
#define SENSEBOX_ID "5947c6c8a4ad5900112f31e3"

//Sensor IDs
#define SENSOR2_ID "5949219ba4ad5900113d4f3c" // Attendance (door)
#define SENSOR3_ID "5947c6c8a4ad5900112f31e6" // Sound
#define SENSOR4_ID "5947c6c8a4ad5900112f31e5" // Humidity
#define SENSOR5_ID "5947c6c8a4ad5900112f31e4" // Temperature


const int trigPin = 8;
const int echoPin = 9;


//Ethernet-Parameter
/*
IPAddress ip(10, 10, 4, 111);
IPAddress dns(10, 10, 4, 33);
IPAddress gateway(10, 10, 4, 250);
IPAddress subnet(255, 255, 255, 0);
*/

char *server = "ingress.opensensemap.org" ;
WiFiClient client;

char ssid[] = "iPhone de Timothee";     //  your network SSID (name)
char pass[] = "wfvh69thrsuro";  // your network password
int status = WL_IDLE_STATUS;     // the WiFi radio's status

//Messparameter
unsigned int postingInterval = 0; //Uploadintervall in Millisekunden

// Microphone
int mic = A0;
long micVal = 0;

float temperature, humidity, sound_lvl;


void setup () {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(mic, INPUT);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  /*
  WiFi.config(ip, dns, gateway, subnet);
  */

  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 2 seconds for connection:
    delay(2000);
  }   
  Serial.println("done!");
  delay(1000);
  Serial.println("Starting loop.");

  hdc.begin(HDC100X_TEMP_HUMI, HDC100X_14BIT, DISABLE); 
}

void loop () {
unsigned int period_time = 60000;
unsigned int delay_time = 1000;
postingInterval = 0;

  while(postingInterval < period_time)
  {
    
    delay(delay_time);
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);  
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distance = duration*0.034/2;  

    if(state == 0)
    {
      if(distance < 150)
        state = 1;
    }
    if(state == 1)
    {
      if(distance >= 150)
      {
        nb_people ++;
        state = 0;
      }
    }
    postingInterval += delay_time;

  }
  
  temperature = hdc.getTemp();
  humidity = hdc.getHumi();
  sound_lvl = getMicVal() + 50;
/**/
  postFloatValue(sound_lvl, 1, SENSOR3_ID);
/*
  delay(1000);
  postFloatValue(nb_people, 1, SENSOR2_ID );
  delay(1000);
  postFloatValue(temperature, 1, SENSOR5_ID);
  delay(1000);
  postFloatValue(humidity, 1, SENSOR4_ID);
*/
}

void postFloatValue (float measurement, int digits, String sensorId) {
  char obs[10];
  dtostrf(measurement, 5, digits, obs);
  String jsonValue = "{\"value\":";
  jsonValue += obs;
  jsonValue += "}";
  Serial.println("-------------------------------------");
  Serial.print("Connecting to OSeM Server... ");
  if (client.connect(server, 80)) {
    Serial.println("connected!");
    Serial.println("-------------------------------------");
    client.print("POST /boxes/"); client.print(SENSEBOX_ID); client.print("/"); client.print(sensorId); client.println(" HTTP/1.1");
    client.print("Host:");
    client.println(server);
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.print("Content-Length: "); client.println(jsonValue.length());
    client.println();
    client.println(jsonValue);
  } else {
    Serial.println("failed!");
    Serial.println("-------------------------------------");
  }
  waitForServerResponse();
}

void waitForServerResponse()
{
  // if there are incoming bytes from the server, read and print them
  delay(100);
  String response = "";
  char c;
  boolean repeat = true;
  do {
    if (client.available()) c = client.read();
    else repeat = false;
    response += c;
    if (response == "HTTP/1.1 ") response = "";
    if (c == '\n') repeat = false;
  }
  while (repeat);

  Serial.print("Server Response: "); Serial.print(response);

  client.flush();
  client.stop();
}


long getMicVal() {
  int period = 3; // Averaging three values ​​in order to catch any 'outliers'
  int correction_value = 510;
  for (int i = 0; i < period; i++) {
    // Computes the absolute value of the value to intercept negative deflections
    micVal = micVal + abs (analogRead(mic) - correction_value);
    delay(5);
  }
  micVal = constrain(abs(micVal / period), 1, 500);
  return (micVal);
}
