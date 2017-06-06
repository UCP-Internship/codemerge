int valeur2, valeur1;

void setup()
{
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
}

void loop()
{
  valeur1 = analogRead(A0);
  Serial.println(valeur1);
  valeur2 = analogRead(A1);
  Serial.println(valeur2);
}
