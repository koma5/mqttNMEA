void setup() {
  Serial.begin(9600);
}

void loop() {}


void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    Serial.write(inChar);    
  }
}


