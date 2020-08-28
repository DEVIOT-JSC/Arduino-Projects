#include <Servo.h>
Servo myFinger;
int ServoPin=9;
int flex;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myFinger.attach(ServoPin);
}
void loop() {
  // put your main code here, to run repeatedly:
  int value=analogRead(A0);
  Serial.println(value);
  flex=map(value,500,1000,0,90); 
  myFinger.write(flex);
  delay(50);
}
