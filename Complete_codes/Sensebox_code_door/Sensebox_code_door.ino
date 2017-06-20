#include <HDC100X.h>
#include <SPI.h>
#include <WiFi101.h>

int nb_people;
long duration;
int distance;
int state = 0;
HDC100X hdc(0x43);

//senseBox ID
#define SENSEBOX_ID "5947c6c8a4ad5900112f31e3"

//Sensor IDs
#define SENSOR1_ID "59491adba4ad5900113d0770" // Attendance (Elevator)
#define SENSOR2_ID "59491adba4ad5900113d076f" // Attendance (door)
#define SENSOR3_ID "5947c6c8a4ad5900112f31e6" // Sound
#define SENSOR4_ID "5947c6c8a4ad5900112f31e5" // Humidity
#define SENSOR5_ID "5947c6c8a4ad5900112f31e4" // Temperature


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

while(postingInterval < 56  && postingInterval > 0)
{
  delay(300);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);  
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2;  

  if(state == 0)
  {
    if(distance < 100)
      state = 1;
  }
  if(state == 1)
  {
    if(distance >= 100)
    {
      nb_people ++;
      state = 0;
    }
  }
  postingInterval ++;

}
  temperature = hdc.getTemp();
  humidity = hdc.getHumi();
  sound_lvl = getMicVal();

  postFloatValue(sound_lvl, 1, SENSOR3_ID);
  //postFloatValue(nb_people, 1, SENSOR2_ID );
  postFloatValue(temperature, 1, SENSOR5_ID);
  postFloatValue(humidity, 1, SENSOR4_ID);
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

void sleep(unsigned long ms) {            // ms: duration
  unsigned long start = millis();         // start: timestamp
  for (;;) {
    unsigned long now = millis();         // now: timestamp
    unsigned long elapsed = now - start;  // elapsed: duration
    if (elapsed >= ms)                    // comparing durations: OK
      return;
  }
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
