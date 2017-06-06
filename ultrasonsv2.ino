#define trig 11
#define echo 12
int time, distance, state;

void setup()
{
  Serial.begin(9600);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

void loop()
{
  digitalWrite(trig, HIGH);
  delay(15);
  digitalWrite(trig, LOW);
  time = pulseIn(echo, HIGH, 1000);
  distance = 0.17 * time;
  Serial.println(distance);
}
