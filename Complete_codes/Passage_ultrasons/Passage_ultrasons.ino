const int trigPin = 9;
const int echoPin = 10;

long duration;
int distance;
int state = 0;
int nb_people;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  delay(400);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2;

  Serial.print("Distance: ");
  Serial.println(distance);
  if(state == 0){  
     if(distance < 70 && distance > 0){
      state ++;
    }
  }

  if(state == 1){
      while(distance < 70){
         digitalWrite(trigPin, LOW);
         delayMicroseconds(2);
         digitalWrite(trigPin, HIGH);
         delayMicroseconds(10);
         digitalWrite(trigPin, LOW);
 
         duration = pulseIn(echoPin, HIGH);
         distance = duration*0.034/2;

         Serial.print("Distance: ");
         Serial.println(distance);
      }
      nb_people ++;
      state --;
      Serial.print("Number of people: ");
      Serial.println(nb_people);
  }
  
}

