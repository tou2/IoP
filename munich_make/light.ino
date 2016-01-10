void setup(){
  Serial.begin(9600);
}

void loop(){
  Serial.println(analogRead(0));
  delay(10000);
}
