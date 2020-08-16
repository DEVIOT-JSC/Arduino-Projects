#include <SPI.h>
#include <WiFi.h>
#include "RFID.h"
#include <LiquidCrystal_I2C.h>
#include "FirebaseESP32.h"
#include <ArduinoJson.h>
#include <NTPClient.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
#define SS_PIN 5
#define RST_PIN 4
#define GREEN_LED 2
#define IN_LED 12
#define OUT_LED 13
#define DOOR_LOCK 14
RFID rfid(SS_PIN, RST_PIN);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
String formattedDate;
String timeStamp;
FirebaseData firebaseData;
FirebaseData firebaseDataAdd;
const char* host = ""; uint32_t timeRespond = 0; uint32_t timeRespondNew = 0;
const int httpsPort = 443;
unsigned char reading_card[5]; //for reading card
unsigned char master[5] = {190, 9, 188, 121, 114};
String masters = "190-9-188-121-114";
String trangThai = "in"; int readcardlen ;
unsigned char i; int type = 0; String keyCheck, valueCheck, keyAdd, valueAdd, keyTime, valueTime, key, value, valueName, keyName, keyFID, valueFID = "";
String sumkey = "", sumlock = "", sumname = "", readcardold = ""; int foundID = 0;
//#define FIREBASE_HOST "deviot-may-cham-cong.firebaseio.com"
#define FIREBASE_HOST "deviot-test.firebaseio.com"
//#define FIREBASE_AUTH "Q5HQTzdzJmxFQ9sRisCxRDoVKnK5QWs3moq6EGSj"
#define FIREBASE_AUTH "rpwrrE8s1g6cOALnjzBtmuJfOhI4uElrEMf3iLaI"

const char* ssid = "P1101";
const char* password = "ampm1234";
//const char* ssid = "Phenikaa 167 HN";
//const char* password = "phenikaa2018";
uint64_t openGateMillis = 0;
FirebaseJson json; bool flag;
FirebaseJson jsonResponding;
String jsonStr = ""; String readcard = "";
FirebaseJsonData jsonObj;
bool CheckAdd = false;
bool CheckCard = false;
bool InsertCard = false;
bool CheckNumCard = false;
bool CheckLock = false;
bool Checkfinger = false;
typedef struct {
  String ID;
  String employ;
  String LockStat ;
} Infor_Employ;
void LcdClearAndPrint(String text, int raw, int col)
{
  lcd.clear();
  lcd.setCursor(raw, col);
  lcd.print(text);
}
String SumKey(String src) {
  String sum;
  sum = sum + src;
  for (int c = 0; c < 20 - src.length(); c++)
  {
    sum += '!';
  }
  return sum;
}
void LcdPrint(String text, int raw, int col)
{
  lcd.setCursor(raw, col);
  lcd.print(text);
}
void printResultTest(FirebaseData &data);
void HandleFireBaseID() {
  Serial.println("Get JSON ID...");
  if (Firebase.get(firebaseData, "/employees"))
  {
    if (firebaseData.dataType() == "json")
    {
      jsonStr = firebaseData.jsonString();
      printResultTest(firebaseData);
      Serial.println();
      FirebaseJson &json = firebaseData.jsonObject();
      //Print all object data
      Serial.println("Pretty printed JSON data:");
      String jsonStr2;
      json.toString(jsonStr2, true);
      size_t len = json.iteratorBegin();
      int type = 0;
      for (size_t m = 0; m < len; m++) {
        json.iteratorGet(m, type, key, value);
        if (m % 10 == 0) {
          Serial.println(key); Serial.println(key.length());
          sumkey += SumKey(key);
          Serial.println(sumkey);
        }
      } json.iteratorEnd();
    }
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();
  }
}
void DataFbLock() {
  Serial.println("Get JSON Lock...");
  if (Firebase.get(firebaseData, "/employees"))
  {
    if (firebaseData.dataType() == "json")
    {
      jsonStr = firebaseData.jsonString();
      Serial.println();
      FirebaseJson &json = firebaseData.jsonObject();
      //Print all object data
      Serial.println("Pretty printed JSON data:");
      String jsonStr2;
      json.toString(jsonStr2, true);
      size_t len = json.iteratorBegin();
      int type = 0;
      for (size_t m = 0; m < len; m++) {
        json.iteratorGet(m, type, keyCheck, valueCheck);
        if (m % 10 == 6) {
          Serial.println(keyCheck); Serial.println(keyCheck.length());
          if (valueCheck == "true") valueCheck = "true!";
          sumlock += valueCheck; Serial.println(sumlock);
        }
      } json.iteratorEnd();
    }
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();
  }
}
void DataFbName() {
  Serial.println("Get JSON ...");
  if (Firebase.get(firebaseData, "/employees"))
  {
    if (firebaseData.dataType() == "json")
    {
      jsonStr = firebaseData.jsonString();
      Serial.println();
      FirebaseJson &json = firebaseData.jsonObject();
      //Print all object data
      Serial.println("Pretty printed JSON data:");
      String jsonStr2;
      json.toString(jsonStr2, true);
      size_t len = json.iteratorBegin();
      int type = 0;
      for (size_t m = 0; m < len; m++) {
        json.iteratorGet(m, type, keyName, valueName);
        if (m % 10 == 8) {
          Serial.println(valueName); Serial.println(valueName.length());
          int lenName = valueName.length();
          for (int c = 0; c < (16 - lenName) / 2; c++)
          {
            sumname += ' ';
          }
          sumname += valueName;
          for (int c = 0; c < (16 - lenName) / 2; c++)
          {
            sumname += ' ';
          }
          if(lenName%2==1) sumname+=" ";
          Serial.println(sumname);
          Serial.println(sumname.length());
        }
      } json.iteratorEnd();
    }
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();
  }
}
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println();
  }
}
void FormatName() {

}
void FormatFID(uint8_t fid) {

}

void setup() {
  Serial.begin(115200);
  // FINGER
  Serial2.begin(115200);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(50);
  finger.begin(57600);
  // FINGER
  lcd.init();
  lcd.backlight();
  pinMode(GREEN_LED, OUTPUT);
  pinMode(IN_LED, OUTPUT);
  pinMode(OUT_LED, OUTPUT);
  pinMode(BTN_IN.PIN, INPUT_PULLUP);
  pinMode(BTN_OUT.PIN, INPUT_PULLUP);
  LcdClearAndPrint("Connecting to...", 0, 0);
  LcdPrint(ssid, 0, 1);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  while (!Serial);
  SPI.begin(); rfid.init(); delay(4);
  LcdClearAndPrint(" IN  ", 5, 0);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setMaxRetry(firebaseData, 3);
  Firebase.setMaxErrorQueue(firebaseData, 30);
  timeClient.begin();
  timeClient.setTimeOffset(25200);
  timeClient.update();
  Firebase.deleteNode(firebaseData, "/addMember");
  Serial.println(timeClient.getFormattedTime());
  Serial.println(timeClient.getFormattedTimeDis());
  Serial.println(timeClient.getFormattedDate());
  HandleFireBaseID(); DataFbName();
  CloseGate();
  if (finger.verifyPassword()) {
    Serial.println("Đã kết nối Sensor !");
  } else {
    Serial.println("Không tìm thấy thiết bị :(");
    while (1) {
      delay(1);
    }
  }
  finger.getParameters(); //Lấy thông tin từ sensor
}

String charToString(unsigned char *Arr) {
  String string;
  for (int i = 0; i < sizeof(Arr) + 1; i++)
  {
    if (i < sizeof(Arr)) {
      string += Arr[i];
      string += '-';
    }
    else string += Arr[sizeof(Arr)];
  } return string;
}
void allow()
{ Serial.println("Access accept!");
  digitalWrite(GREEN_LED, HIGH);
}
void denied()
{ LcdClearAndPrint("Sorry!", 4, 0);
  LcdPrint("Card Not Exist", 0, 1);
  Serial.println("Access denied!");
  delay(3000);
}

void CloseGate() {
  openGateMillis = 0;
  LcdClearAndPrint("     IN  ", 0, 0);
}
void loop() {
  Scan_Finger_ID();
  Firebase.enableClassicRequest(firebaseData, true);
  Firebase.enableClassicRequest(firebaseDataAdd, true);
  CheckAdd = false;
  if (Firebase.get(firebaseDataAdd, "/addMember"))
  {
    FirebaseJson &jsonAdd = firebaseDataAdd.jsonObject();
    size_t len = jsonAdd.iteratorBegin();
    for (size_t j = 0; j < len; j++) {
      jsonAdd.iteratorGet(j, type, keyAdd, valueAdd);
      if (j % 1 == 0) {
        if (valueAdd == "1" && keyAdd == "appRequest") {
          Serial.println(keyAdd); Serial.println(valueAdd);
          LcdPrint("Insert Card...", 0, 0);
          Serial.println("GETDATA : TRUE"); delay(35);
          CheckAdd = true;
        }
        else {
          CheckAdd = false;
        }
      }
    }
  } else {
    CheckAdd = false;
  }
  if (digitalRead(BTN_OUT.PIN) == 0) {
    delay(20);
    if (digitalRead(BTN_OUT.PIN) == 0)
    { trangThai = "out";
      LcdClearAndPrint("OUT", 6, 0);
      digitalWrite(OUT_LED, 1); digitalWrite(IN_LED, 0);
      while (digitalRead(BTN_OUT.PIN) == 0);
    }
  }
  if (!rfid.isCard()) {
    CheckCard = false;
    // Serial.println("No Card");
  } else {
    CheckCard = true;
    // Serial.println("Ready... Card");
  }
  if (!rfid.readCardSerial()) {
    CheckNumCard = false;
    //Serial.println("No Read Card");
  } else {
    CheckNumCard = true;
    //Serial.println("Read Card");
  }
  if (CheckAdd == false && CheckCard == false && CheckNumCard == false) {

  }
  else if (CheckAdd == false  && CheckCard == true && CheckNumCard == true)
  {
    LcdClearAndPrint("IN", 6, 0);
    digitalWrite(IN_LED, 1); digitalWrite(OUT_LED, 0);
    /* Reading card */
    ReadingCard(); int found = 0;
    for (foundID = 0; foundID < sumkey.length() / 20; foundID++)
    {
      if (readcard == sumkey.substring(foundID * 20, foundID * 20 + 20))
      {
        found = 1;
        break;
      }
    }
    if ( found != 1) {
      InsertCard = false;
      Serial.println("FALSE");
      LcdClearAndPrint("CARD NOT EXIST", 1, 0);
      delay(300);
      LcdClearAndPrint("IN", 6, 0);
    }
    else {
      InsertCard = true;
      LcdClearAndPrint("Please wait...", 0, 0);
      sumlock = "";
      DataFbLock();
      if (sumlock.substring(foundID * 5, foundID * 5 + 5) == "true!") CheckLock = true;
      else CheckLock = false;
      rfid.halt();
      if (CheckLock == true) {
        LockCard();
      }
      else {
        if (InsertCard == true) {
          allow();
          LcdClearAndPrint(sumname.substring(foundID*16,foundID*16+16), 0, 1);
          if (trangThai == "in") {
            json.add("timeEnter", (timeClient.getFormattedTimeDis()).c_str());
            LcdPrint((timeClient.getFormattedTimeDis()).c_str(), 4, 0);
          }
          else if (trangThai == "out") {
            LcdPrint((timeClient.getFormattedTimeDis()).c_str(), 4, 0);
            json.add("timeExit", (timeClient.getFormattedTimeDis()).c_str());
          }
          Firebase.setJSON(firebaseData, "/diemdanh/" + timeClient.getFormattedDate() + "/" + readcardold, json);
          delay(3000);
          digitalWrite(GREEN_LED, LOW);
        }
        else {
          denied();
        }
      }
      CheckLock = false;
      InsertCard == false;
      CheckCard = false;
      digitalWrite(IN_LED, 0); digitalWrite(OUT_LED, 0);
      LcdClearAndPrint("IN", 6, 0);
      return;
    }
  }
  if (CheckAdd == true && CheckCard == false) {
    timeRespondNew = millis();
  }
  else if (CheckAdd == true && CheckCard == true) {
    ReadingCard();
    Firebase.get(firebaseDataAdd, "/addMember");
    FirebaseJson &jsonAdd = firebaseDataAdd.jsonObject();
    size_t len = jsonAdd.iteratorBegin();
    for (size_t j = 0; j < len; j++) {
      jsonAdd.iteratorGet(j, type, keyAdd, valueAdd);
      if (j % 1 == 0) {
        Serial.println(keyAdd); Serial.println(valueAdd);
        if (valueAdd == "1" && keyAdd == "appRequest") {
          jsonAdd.iteratorEnd(); int count;

          for (foundID = 0; foundID < sumkey.length() / 20; foundID++)
          {
            if (readcard == sumkey.substring(foundID * 20, foundID * 20 + 20))
            {
              CheckLock = true;
              break;
            }
          }
          //if (valueCheck == "true") CheckLock = true;
          if (CheckLock == true) LockCard();
          else {
            if (InsertCard == true) {
              CardExist();  // The da ton tai
              LocalMonitor();
              break;
              return;
            }
            else {
              Serial.println("Timer new : " + (String)timeRespondNew);
              CardNotExist();
            }// The chua ton tai
            break;
          }
        }
      }
    }
    CheckLock = false;
  }
  Firebase.get(firebaseDataAdd, "/addMember");
  FirebaseJson &jsonAddNew = firebaseDataAdd.jsonObject();
  size_t lennew = jsonAddNew.iteratorBegin();
  for (size_t j = 0; j < lennew; j++) {
    jsonAddNew.iteratorGet(j, type, keyAdd, valueAdd);
    if (j % 1 == 0) {
      if ((unsigned long)(millis() - timeRespondNew) > 100000 && InsertCard == false && flag == 1) {
        Serial.println("Timer : " + (String)timeRespondNew);
        Serial.println("Timer : " + (String)millis());
        flag = 0;
        LcdClearAndPrint("TIME OUT .... ", 0, 0);
        CheckAdd == false;
        Firebase.deleteNode(firebaseData, "/addMember");
        delay(3000);
        LocalMonitor();
      }
      else if (valueAdd == "2" && keyAdd == "appRequest" && InsertCard == false) {
        Serial.println("GETDATA : DONE");
        CheckAdd = false;
        LcdClearAndPrint(" SUCCESS ! ", 1, 0);
        Firebase.deleteNode(firebaseData, "/addMember");
        delay(2000);
        LocalMonitor();
        sumkey = ""; sumlock = ""; sumname = "";
        HandleFireBaseID(); DataFbName(); DataFbLock();
      }
    }
  }
}

void LockCard() {
  LcdClearAndPrint("YOUR CARD", 0, 0);
  LcdPrint("HAS BEEN LOCKED", 0, 1);
  delay(2000);
}
void ReadingCard() {
  Serial.println(" ");
  Serial.println("Card found");
  Serial.println("Cardnumber:");
  for (i = 0; i < 5; i++)
  {
    Serial.print(rfid.serNum[i]);
    Serial.print(" ");
    reading_card[i] = rfid.serNum[i];
  }
  Serial.println();
  readcard = charToString(reading_card);
  readcardold = readcard;
  readcardlen = readcard.length();
  for (int c = 0; c < 20 - readcardlen; c++) {
    readcard += '!';
  }
  Serial.print(readcard + "  " + readcard.length()); Serial.println();
}
void LocalMonitor()
{
  InsertCard = false;
  CheckAdd = false;
  CheckCard = false;
  LcdClearAndPrint("   IN", 2, 0);
  Firebase.deleteNode(firebaseData, "/addMember");
}
void CardExist()
{
  jsonResponding.add("appResponse", "0");
  Firebase.setJSON(firebaseDataAdd, "/addMember", jsonResponding);
  LcdClearAndPrint("CARD EXISTED", 1, 0);
  Serial.println("Card Exist!");
  delay(3000);
}
void CardNotExist() {
  flag = 1;
  timeRespond = millis();
  Serial.println("Timer : " + (String)timeRespond);
  jsonResponding.add("appResponse", "1");
  jsonResponding.add("numCard", readcard);
  Firebase.setJSON(firebaseDataAdd, "/addMember", jsonResponding);
  LcdClearAndPrint("CREAT ACCOUNT.. ", 0, 0);
  Serial.println("CARD NOT EXIST!");
  delay(1000);
}
uint8_t readnumber(void) {
  uint8_t num = 0;
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}
void Add_ID_Finger() {        // Thêm ID
  Serial.println("Mời bạn đăng ký ID:");
  id = readnumber();
  if (id == 0)
  { // ID #0 not allowed, try again!
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
void printResultTest(FirebaseData & data)
{
  if (data.dataType() == "int")
    Serial.println(data.intData());
  else if (data.dataType() == "float")
    Serial.println(data.floatData(), 5);
  else if (data.dataType() == "double")
    printf("%.9lf\n", data.doubleData());
  else if (data.dataType() == "boolean")
    Serial.println(data.boolData() == 1 ? "true" : "false");
  else if (data.dataType() == "string")
    Serial.println(data.stringData());
  else if (data.dataType() == "json")
  {
    Serial.println();
    FirebaseJson &json = data.jsonObject();
    //Print all object data
    Serial.println("Pretty printed JSON data:");
    String jsonStr;
    json.toString(jsonStr, true);
    Serial.println(jsonStr);
    Serial.println();
    Serial.println("Iterate JSON data:");
    Serial.println();
    size_t len = json.iteratorBegin();
    String key, value = "";
    int type = 0;
    for (size_t i = 0; i < len; i++)
    {
      json.iteratorGet(i, type, key, value);
      Serial.print(i);
      Serial.print(", ");
      Serial.print("Type: ");
      Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
      if (type == FirebaseJson::JSON_OBJECT)
      {
        Serial.print(", Key: ");
        Serial.print(key);
      }
      Serial.print(", Value: ");
      Serial.println(value);
    }
    json.iteratorEnd();
  }
  else if (data.dataType() == "array")
  {
    Serial.println();
    //get array data from FirebaseData using FirebaseJsonArray object
    FirebaseJsonArray &arr = data.jsonArray();
    //Print all array values
    Serial.println("Pretty printed Array:");
    String arrStr;
    arr.toString(arrStr, true);
    Serial.println(arrStr);
    Serial.println();
    Serial.println("Iterate array values:");
    Serial.println();
    for (size_t i = 0; i < arr.size(); i++)
    {
      Serial.print(i);
      Serial.print(", Value: ");

      FirebaseJsonData &jsonData = data.jsonData();
      //Get the result data from FirebaseJsonArray object
      arr.get(jsonData, i);
      if (jsonData.typeNum == FirebaseJson::JSON_BOOL)
        Serial.println(jsonData.boolValue ? "true" : "false");
      else if (jsonData.typeNum == FirebaseJson::JSON_INT)
        Serial.println(jsonData.intValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_FLOAT)
        Serial.println(jsonData.floatValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_DOUBLE)
        printf("%.9lf\n", jsonData.doubleValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_STRING ||
               jsonData.typeNum == FirebaseJson::JSON_NULL ||
               jsonData.typeNum == FirebaseJson::JSON_OBJECT ||
               jsonData.typeNum == FirebaseJson::JSON_ARRAY)
        Serial.println(jsonData.stringValue);
    }
  }
  else
  {
    Serial.println(data.payload());
  }
}
