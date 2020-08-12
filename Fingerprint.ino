#include <Adafruit_Fingerprint.h>
#if defined(__AVR__) || defined(ESP8266)
SoftwareSerial mySerial(2, 3); //D2=Rx,D3=Tx;
#else
#define mySerial Serial1
#endif
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;
#define Get_ID_Btn 6  //Nut bam nhap ID
int sta; //sta=0 Nhập, sta=1 Quét
void setup()  
{  
  pinMode(6,INPUT_PULLUP);
  pinMode(7,OUTPUT);
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(50);
  finger.begin(57600); 
  if (finger.verifyPassword()) {
    Serial.println("Đã kết nối Sensor !");
  } else {
    Serial.println("Không tìm thấy thiết bị :(");
    while (1) { delay(1); }
  }
    finger.getParameters(); //Lấy thông tin từ sensor 
}
uint8_t readnumber(void) {
  uint8_t num = 0;  
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}
void Add_ID_Finger(){         // Thêm ID
       Serial.println("Mời bạn đăng ký ID:");
       id = readnumber();
       if (id == 0) 
       {  // ID #0 not allowed, try again!
        return;
       }
       Serial.print("Đang quét vân tay #");
       Serial.println(id);
       while (! ID_Initialization() );
}
uint8_t ID_Initialization() {
  int p = -1;
//  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Đang lấy hình ảnh");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Kết nối bị gián đoạn");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Ảnh vân tay bị lỗi");
      break;
    default:
      Serial.println("Lỗi");
      break;
    }
  }
  delay(500);
  // OK success!
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Đã quét vân tay");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Ảnh mờ");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Kết nối bị gián đoạn");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Không tìm thấy vân tay");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Không tìm thấy vân tay");
      return p;
    default:
      Serial.println("Lỗi");
      return p;
  } 
  delay(500);
  Serial.println("Mời thả tay ra");
  delay(500);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;
  Serial.println("Xác nhận lại vân tay");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Đang quét hình ảnh");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Lỗi kết nối");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Ảnh lỗi");
      break;
    default:
      Serial.println("Lỗi");
      break;
    }
  }
  delay(100);
  // OK success!
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Đã quét vân tay");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Ảnh mờ");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Lỗi kết nối");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Không tìm thấy vân tay");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
}
  // OK converted!
  Serial.print("Khởi tạo ID #");  Serial.println(id);
  delay(100);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) 
  {
    Serial.println("Xác nhận vân tay!");
  } 
//  else if (p == FINGERPRINT_PACKETRECIEVEERR) 
//  {
//    Serial.println("Lỗi kết nối");
//    return p;
//  } 
  else if (p == FINGERPRINT_ENROLLMISMATCH) 
  {
    Serial.println("Vân tay không trùng khớp\nMời nhập lại");
    return p;
  } 
  else 
  {
    Serial.println("Lỗi");
    return p;
  }   
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) 
  {
    Serial.println("Đã lưu");
  } 
  else if (p == FINGERPRINT_PACKETRECIEVEERR) 
  {
    Serial.println("Lỗi kết nối");
    return p;
  } 
  else if (p == FINGERPRINT_BADLOCATION) 
  {
    Serial.println("Không thể lưu ID");
    return p;
  } 
  else 
  {
    Serial.println("Lỗi");
    return p;
  } 
  return true;
}
void loop()                     
{
  if(digitalRead(Get_ID_Btn)==0)
  {
    delay(10);
    {
      if(digitalRead(Get_ID_Btn)==0)   Add_ID_Finger();
    }
  }
  Scan_Finger_ID();
  delay(500);
}
uint8_t Scan_Finger_ID() {                
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Đang quét");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("Không tìm thấy vân tay");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Lỗi kết nối");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Lỗi ảnh");
      return p;
    default:
      Serial.println("Lỗi");
      return p;
  }
  // OK success!
  p = finger.image2Tz();      //Convert ảnh vân tay
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Lỗi kết nối");
      return p;
    default:
      Serial.println("Lỗi");
      return p;
  }
  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) 
  {
    Serial.println("Xác nhận");
    Serial.print("Xin chào ID #"); Serial.print(finger.fingerID); 
    digitalWrite(7,HIGH);
  } 
  else if (p == FINGERPRINT_PACKETRECIEVEERR) 
  {
    Serial.println("Lỗi kết nối");
    return p;
  } 
  else if (p == FINGERPRINT_NOTFOUND) 
  {
    Serial.println("Vân tay không trùng khớp");
    return p;
  } 
  else 
  {
    Serial.println("Lỗi");
    return p;
  } 
  //Serial.print("Found ID #"); Serial.print(finger.fingerID);   
  //Serial.print(" with confidence of "); Serial.println(finger.confidence); 
  return finger.fingerID;
}
