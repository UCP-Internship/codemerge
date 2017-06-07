//int port_mic = A0;
long value = 1;

void setup()
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
}

/*long get_value()
{
  value = analogRead(A0);
  return value;
}*/

void loop()
{
    Serial.println(value);
  value = analogRead(A0);

}


