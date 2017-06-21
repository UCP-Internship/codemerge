#include <SPI.h>
#include <WiFi101.h>
#include <Wire.h>
#include <BMP280.h>

BMP280 bmp;

int nb_people=3;
int distance;
int dist_mur;
int state = 0;

double temp, pressure;
char bmpStatus;

//senseBox ID
#define SENSEBOX_ID "59492170a4ad5900113d4d5c"

//Sensor IDs
#define SENSOR1_ID "594939efa4ad5900113e5300" // Temperature
#define SENSOR2_ID "59492170a4ad5900113d4d60" // Presence
#define SENSOR3_ID "59492170a4ad5900113d4d5f" // Pressure

IPAddress ip(10, 10, 4, 221);
IPAddress dns(10, 10, 4, 251);
IPAddress gateway(10, 10, 4, 250);
IPAddress subnet(255, 255, 255, 0);

const char *server = "ingress.opensensemap.org";
const int trigPin = 8;
const int echoPin = 9;


//Ethernet-Parameter
IPAddress ip(10, 10, 4, 170);
IPAddress dns(10, 10, 4, 251);
IPAddress gateway(10, 10, 4, 250);
IPAddress subnet(255, 255, 255, 0);

char *server = "ingress.opensensemap.org" ;
WiFiClient client;

char ssid[] = "etis-midi";     //  your network SSID (name)
char pass[] = "%midi564";  // your network password
int status = WL_IDLE_STATUS;     // the WiFi radio's status

//Messparameter
int postingInterval = 0; //Uploadintervall in Millisekunden


float temperature, humidity, sound_lvl;


void setup () {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  if(!bmp.begin())
  {
    Serial.println("BMP init failed");
  }
  bmp.setOversampling(4); // select resolution of the measurements
  
  WiFi.config(ip, dns, gateway, subnet);
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 2 seconds for connection:
    delay(2000);
  } 
  dist_mur = getDistance();  
  Serial.println("done!");
  delay(1000);
  Serial.println("Starting loop.");
 
}

void loop () {

while(postingInterval < 67 && postingInterval > 0)
{
  distance = getDistance();
  if(state == 0)
  {
    if(distance < (dist_mur - 40))
      state = 1;
  }
  if(state == 1)
  {
    if(distance >= (dist_mur - 40))
    {
      nb_people ++;
      state = 0;
    }
  }
  postingInterval ++;
}
   bmpStatus = bmp.getTemperatureAndPressure(temp, pressure);

  postFloatValue(nb_people, 1, SENSOR2_ID );
  postFloatValue(temp, 1, SENSOR1_ID);
  postFloatValue(pressure, 1, SENSOR3_ID);
  postingInterval = 1;


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

void waitForServerResponse () {
  boolean repeat = true;
  Serial.println("Waiting...");
  do {
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
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

int getDistance()
{
  unsigned long duration;
  int dist;
  delay(250);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);  
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  dist = duration*0.034/2;  
  return dist;
}

void sleep(unsigned long ms) {            // ms: duration
  unsigned long start = millis();         // start: timestamp
  for (;;) {
    unsigned long now = millis();         // now: timestamp
    unsigned long elapsed = now - start;  // elapsed: duration
    if (elapsed >= ms)                    // comparing durations: OK
      return;
  }
}

