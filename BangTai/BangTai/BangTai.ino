#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#define VALUE_SENSOR_DETECTED 0// khi co vat
//s0=1
//s1=0
#define s2 5
#define s3 6
#define pinSensorMau 4
//
const byte pinSenSor = 2;//cam bien tiem can
const byte pinMotor=A0;//chân rơle
int cnt = 0;
Servo khopQuay, khopVuon, khopNang,kep;
LiquidCrystal_I2C lcd(0x27, 16, 2);  
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("DO AN TOT NGHIEP");
  lcd.setCursor(0, 1);
  lcd.print("So Luong: 0");
  pinMode(pinSenSor,INPUT_PULLUP);
  pinMode(pinMotor, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  motor_start();
  khopQuay.attach(11, 500, 2400);
  khopVuon.attach(10, 500, 2400);
  khopNang.attach(9, 500, 2400);
  kep.attach(8, 650, 2350);
  khopQuay.write(180);
  khopVuon.write(150);
  khopNang.write(170);
  kep.write(60);
}

// the loop function runs over and over again until power down or reset
void loop() {
  if (digitalRead(pinSenSor) == VALUE_SENSOR_DETECTED)
  {
    cnt++;
    motor_stop();
    gapVat(getMau());
    motor_start();
  }
}
void hienThi()
{
  lcd.setCursor(0,1);
  lcd.print("So Luong: "+String(cnt));
}
int getMau()
{
 // int RTB=0;
    delay(100);
    digitalWrite(s2, LOW);
    digitalWrite(s3, LOW);
    unsigned int R = pulseIn(pinSensorMau, LOW);
   // RTB+=R;
    delay(100);
    digitalWrite(s2, HIGH);
    digitalWrite(s3, HIGH);
    unsigned int G = pulseIn(pinSensorMau, LOW);
    delay(100);
    digitalWrite(s2, LOW);
    digitalWrite(s3, HIGH);
    unsigned int B = pulseIn(pinSensorMau, LOW);
 // Serial.println(String(R)+"-"+String(G)+"-"+String(B));
  Serial.println(R);
  if(R>=115) return 0;//xam
  return 1;
}
void gapVat(int mau)
{
  //mover(180, 150, 170, 180,10);
  mover(180, 150, 151 , 60,30);//vuon xuong
  delay(200);
  mover(180, 176, 151, 60,30);//vuon ra
  delay(200);
  mover(180, 176, 151, 10,30);//kep vat
  delay(200);
  mover(180, 155, 151, 10,40);// lùi ve
  delay(200);
  mover(180, 155, 175, 10,20);//nhac len
  hienThi();
  while (digitalRead(pinSenSor) == VALUE_SENSOR_DETECTED);
  motor_start();
  int goc;
  if(mau==MAU1) goc=30;
  else  goc=0;
  mover(goc, 155, 175, 10,5);//quay ra de tha vat
  delayQuetCB(200);
  mover(goc, 155, 140, 60,20);
  delayQuetCB(200);
  mover(goc, 155, 140, 60,20);//nha vat
  delayQuetCB(200);
  mover(goc, 150, 175, 60, 30);//len
  delayQuetCB(200);
  mover(180, 150, 175, 60,5);//quay ve
}
void mover(int gocQuay, int gocVuon, int gocNang, int gocKep,int td)
{
  int preGocQuay = khopQuay.read();
  int preGocVuon = khopVuon.read();
  int preGocNang = khopNang.read();
  int preGocKep = kep.read();

  quay(khopQuay,preGocQuay,gocQuay,td);
  quay(khopVuon, preGocVuon, gocVuon,td);
  quay(khopNang, preGocNang, gocNang,td);
  //quay(kep, preGocKep, gocKep,td);
  kep.write(gocKep);
}
void quay(Servo sv,int preGoc,int destGoc,int td)
{
  if (preGoc < destGoc)
  {
    while (preGoc!=destGoc)
    {
      preGoc++;
      sv.write(preGoc);
      delayQuetCB(td);
    }
  }
  else if (preGoc > destGoc)
  {
    while (preGoc != destGoc)
    {
      preGoc--;
      sv.write(preGoc);
      delayQuetCB(td);
    }
  }
}
void delayQuetCB(int ms)
{
  for (int i = 0; i < ms; i++)
  {
    if (digitalRead(pinSenSor) == VALUE_SENSOR_DETECTED)
    {
      motor_stop();
    }
    delay(1);
  }
}
void motor_start()
{
  digitalWrite(pinMotor, HIGH);
}
void motor_stop()
{
  digitalWrite(pinMotor, LOW);

}
 
