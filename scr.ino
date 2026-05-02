#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN,RST_PIN);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH,SCREEN_HEIGHT,&Wire,-1);

#define BTN 2
#define GREEN_LED 3
#define RED_LED 4
#define YELLOW_LED 5
#define BUZZER 6

bool shelfSelected=false;
String currentShelf="";

String CHEM_SHELF="5A3F91C7";
String ELEC_SHELF="7D2C4B8E";

String chemistry[]={
	"C91A3F7B",
	"F3B81A2D",
	"9C7E31B0",
	"A8D21C9E",
	"D4A9F2C8"
};

String electronics[]={
	"E5B19D3A",
	"B8A3D7F2",
	"7F2C8E91",
	"1C9F4A6D",
	"D92B71E8"
};

void setup(){
	Serial.begin(9600);
	SPI.begin();
	rfid.PCD_Init();

	pinMode(BTN,INPUT_PULLUP);
	pinMode(GREEN_LED,OUTPUT);
	pinMode(RED_LED,OUTPUT);
	pinMode(YELLOW_LED,OUTPUT);
	pinMode(BUZZER,OUTPUT);

	display.begin(SSD1306_SWITCHCAPVCC,0x3C);
	display.clearDisplay();

	showMessage("ShelfCheck Ready");
}

void loop(){

	if(digitalRead(BTN)==LOW){
		delay(300);
		shelfSelected=true;
		currentShelf="";
		showMessage("Scan Shelf Tag");
		digitalWrite(YELLOW_LED,HIGH);
	}

	if(!rfid.PICC_IsNewCardPresent()||!rfid.PICC_ReadCardSerial()){
		return;
	}

	String uid=getUID();
	Serial.println(uid);

	if(shelfSelected){
		currentShelf=uid;
		shelfSelected=false;
		digitalWrite(YELLOW_LED,LOW);
		showMessage("Shelf Selected");
		delay(1000);
		return;
	}

	bool correct=checkBook(uid);

	if(correct){
		digitalWrite(GREEN_LED,HIGH);
		digitalWrite(RED_LED,LOW);
		noTone(BUZZER);
		showMessage("Correct Book");
	}
	else{
		digitalWrite(GREEN_LED,LOW);
		digitalWrite(RED_LED,HIGH);
		tone(BUZZER,1000);
		showMessage("MISPLACED BOOK");
	}

	delay(1500);
	resetOutputs();
	rfid.PICC_HaltA();
}

String getUID(){
	String uid="";
	for(byte i=0;i<rfid.uid.size;i++){
		if(rfid.uid.uidByte[i]<0x10) uid+="0";
		uid+=String(rfid.uid.uidByte[i],HEX);
	}
	uid.toUpperCase();
	return uid;
}

bool checkBook(String uid){

	if(currentShelf==CHEM_SHELF){
		for(String b:chemistry){
			if(uid==b) return true;
		}
	}

	if(currentShelf==ELEC_SHELF){
		for(String b:electronics){
			if(uid==b) return true;
		}
	}

	return false;
}

void showMessage(String msg){
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.setCursor(0,10);
	display.println(msg);
	display.display();
}

void resetOutputs(){
	digitalWrite(GREEN_LED,LOW);
	digitalWrite(RED_LED,LOW);
	noTone(BUZZER);
}