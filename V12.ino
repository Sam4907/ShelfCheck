#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN,RST_PIN);

#define BUTTON 4
#define RED_LED 5
#define YELLOW_LED 7
#define GREEN_LED 6

#define MODE_SCAN 0
#define MODE_TECH 1
#define MODE_LITERATURE 2
#define MODE_REVIEW 3

int mode=0;

String shelfLITERATURE="E253F904";
String shelfTech="BC141307";

String LITERATUREUIDs[5]={"C695B501","C1503A1D","5387B534","B1BBD21D","B104981D"};
String LITERATURENames[5]={"Murder on the Orient Express","Crime and Punishment","The Great Gatsby","The Hound of the Baskervilles","The Time Machine"};

String techUIDs[5]={"C1D02B07","53643D34","90DB2226","33838033","906ED526"};
String techNames[5]={"Clean Code","Computer Networks","Operating System Concepts","Deep Learning","Code: Hardware & Software"};

String histBook[5];
String histShelf[5];
String histResult[5];
int histIndex=0;

String activeShelf="NONE";

bool reportPrinted=false;

void resetLEDs(){
  digitalWrite(RED_LED,LOW);
  digitalWrite(YELLOW_LED,LOW);
  digitalWrite(GREEN_LED,LOW);
}

void printLine(){
  Serial.println("------------------------------");
}

String getCorrectShelf(String uid){
  for(int i=0;i<5;i++){
    if(uid==LITERATUREUIDs[i])return "LITERATURE";
    if(uid==techUIDs[i])return "TECH";
  }
  return "UNKNOWN";
}

void addHistory(String book,String shelf,String result){
  histBook[histIndex]=book;
  histShelf[histIndex]=shelf;
  histResult[histIndex]=result;
  histIndex=(histIndex+1)%5;
}

void setup(){
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(BUTTON,INPUT_PULLUP);
  pinMode(RED_LED,OUTPUT);
  pinMode(YELLOW_LED,OUTPUT);
  pinMode(GREEN_LED,OUTPUT);

  Serial.println("ShelfCheck Ready");
  printLine();
}

void loop(){

  if(digitalRead(BUTTON)==LOW){
    mode++;
    if(mode>3)mode=0;

    delay(300);
    resetLEDs();
    reportPrinted=false;

    Serial.print("MODE: ");

    if(mode==MODE_SCAN){
      Serial.println("SCAN");
      activeShelf="NONE";
    }
    else if(mode==MODE_TECH){
      Serial.println("TECH MODE");
      activeShelf="TECH";
      digitalWrite(YELLOW_LED,HIGH);
    }
    else if(mode==MODE_LITERATURE){
      Serial.println("LITERATURE MODE");
      activeShelf="LITERATURE";
      digitalWrite(YELLOW_LED,HIGH);
    }
    else if(mode==MODE_REVIEW){
      Serial.println("REVIEW MODE");
    }

    printLine();
  }

  if(mode==MODE_REVIEW){

    if(!reportPrinted){
      Serial.println("=======================");
      Serial.println("SHELF REPORT ");
      Serial.println("==============================");

      int count=1;
      int idx=histIndex;

      for(int i=0;i<5;i++){
        idx--;
        if(idx<0)idx=4;

        if(histBook[idx]!=""){
          Serial.print(count);
          Serial.print(". ");
          Serial.print(histBook[idx]);
          Serial.print(" | ");
          Serial.print(histShelf[idx]);
          Serial.print(" | ");
          Serial.println(histResult[idx]);
          count++;
        }
      }

      Serial.println("------------------------------");
      reportPrinted=true;
    }

    static bool blink=false;
    blink=!blink;
    digitalWrite(YELLOW_LED,blink);

    delay(500);
    return;
  }

  if(!rfid.PICC_IsNewCardPresent())return;
  if(!rfid.PICC_ReadCardSerial())return;

  String uid="";
  for(byte i=0;i<rfid.uid.size;i++){
    if(rfid.uid.uidByte[i]<0x10)uid+="0";
    uid+=String(rfid.uid.uidByte[i],HEX);
  }
  uid.toUpperCase();

  resetLEDs();

  String shelf=activeShelf;

  if(mode==MODE_SCAN){

    if(uid==shelfLITERATURE){
      if(activeShelf!="LITERATURE"){
        activeShelf="LITERATURE";
        Serial.println("Shelf: LITERATURE");
        printLine();
        digitalWrite(YELLOW_LED,HIGH);
      }
      delay(300);
      return;
    }

    if(uid==shelfTech){
      if(activeShelf!="TECH"){
        activeShelf="TECH";
        Serial.println("Shelf: TECH");
        printLine();
        digitalWrite(YELLOW_LED,HIGH);
      }
      delay(300);
      return;
    }

    shelf=activeShelf;
  }

  if(shelf=="NONE"){
    Serial.println("Select shelf first");
    printLine();
    digitalWrite(RED_LED,HIGH);
    delay(500);
    resetLEDs();
    return;
  }

  String correctShelf=getCorrectShelf(uid);
  String bookName="UNKNOWN";

  if(correctShelf=="LITERATURE"){
    for(int i=0;i<5;i++){
      if(uid==LITERATUREUIDs[i]){
        bookName=LITERATURENames[i];
        break;
      }
    }
  }
  else if(correctShelf=="TECH"){
    for(int i=0;i<5;i++){
      if(uid==techUIDs[i]){
        bookName=techNames[i];
        break;
      }
    }
  }

  if(bookName!="UNKNOWN"){

    if(correctShelf==shelf){
      digitalWrite(GREEN_LED,HIGH);

      Serial.print("BOOK        : ");
      Serial.println(bookName);
      Serial.println("STATUS      : CORRECT");

      addHistory(bookName,shelf,"CORRECT");
    }
    else{
      digitalWrite(RED_LED,HIGH);

      Serial.print("BOOK        : ");
      Serial.println(bookName);
      Serial.println("STATUS      : MISPLACED");

      Serial.print("CORRECT SHELF: ");
      Serial.println(correctShelf);

      addHistory(bookName,correctShelf,"MISPLACED");
    }

  }
  else{
    digitalWrite(RED_LED,HIGH);

    Serial.println("BOOK        : UNKNOWN TAG");
    Serial.println("STATUS      : MISPLACED");

    Serial.print("EXPECTED     : ");
    Serial.println(shelf);

    addHistory("UNKNOWN",shelf,"MISPLACED");
  }

  printLine();
  delay(800);

  resetLEDs();
  digitalWrite(YELLOW_LED,HIGH);

  rfid.PICC_HaltA();
}