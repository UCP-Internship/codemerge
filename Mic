int mic = A0;
long micVal = 0; // Stores the value of the sound collected by the microphone

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(mic, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(getMicVal());
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
