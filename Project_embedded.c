#include <SoftwareSerial.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include "DHT.h"


ThreeWire myWire(6,9,8); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

SoftwareSerial bluetooth(3, 2);
Servo servo;
LiquidCrystal_I2C lcd(0x27, 16, 2);
int chipSelect = 10;  // CS pin of the SD card module
int led_red = 5;

unsigned long doorOpenedTime = 0;
unsigned long doorClosedTime = 0;
///////////////// Temp
int temp = A3;




//////////////// sound
#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951
#define NOTE_C8 4186
#define NOTE_CS8 4435
#define NOTE_D8 4699
#define NOTE_DS8 4978


int melody[] = {
  NOTE_E5, NOTE_D5, NOTE_FS4, NOTE_GS4,
  NOTE_CS5, NOTE_B4, NOTE_D4, NOTE_E4,
  NOTE_B4, NOTE_A4, NOTE_CS4, NOTE_E4,
  NOTE_A4
};

int durations[] = {
  8, 8, 4, 4,
  8, 8, 4, 4,
  8, 8, 4, 4,
  2
};


//////////////////////

int buzzer = 7;




void setup() {

  pinMode(buzzer, OUTPUT);
  pinMode(led_red, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  // float temperature = dht.readTemperature();  // Read temperature in Celsius
  int tempF = analogRead(temp);
  int tempcc =((tempF-32)*5)/16;
  lcd.print("Temp is ");
  lcd.print(tempcc);
  lcd.print("C ^_^");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("Please connect ");
  delay(20000);
  Serial.begin(9600);
  // dht.begin();
  if (!SD.begin(chipSelect)) {
    Serial.println("SD Card initialization failed!");
    return;
  }
  Serial.println("SD Card initialized successfully.");
  bluetooth.begin(9600);
  servo.attach(4);
}

void loop() {
  // int temperature = dht.readTemperature(); 
  // Serial.println(temperature);
  RtcDateTime now = Rtc.GetDateTime();
  // Check for Bluetooth input
  if (bluetooth.available()) {
    char received = bluetooth.read();
    Serial.print(received);
    if (received == '1') {
      int size = sizeof(durations) / sizeof(int);
      for (int note = 0; note < size; note++) {
        int duration = 1000 / durations[note];
        tone(buzzer, melody[note], duration);
        int pauseBetweenNotes = duration * 1.30;
        delay(pauseBetweenNotes);
        noTone(buzzer);
      }
      delay(200);
      servo.write(120);
      delay(500);
      digitalWrite(led_red, HIGH);
      lcd.setCursor(0, 0);
      lcd.print("The Door is Open");
      lcd.setCursor(0, 1);
      lcd.print("Welcome!! ^_^ ");
      // Open the door
      recordEvent("Door Opened", now);
    } else if (received == '0') {
      servo.write(0);
      delay(1000);
      digitalWrite(led_red, LOW);
      lcd.setCursor(0, 0);
      lcd.print("The Door is Close");
      lcd.setCursor(0, 1);
      lcd.print("See you!! ^_^ ");
      // Close the door
      recordEvent("Door Closed", now);
    }
  }
}

void recordEvent(String event, RtcDateTime eventTime) {
  File dataFile = SD.open("data.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.print(event);
    dataFile.print(" at ");
    dataFile.print(eventTime.Year());
    dataFile.print("-");
    dataFile.print(eventTime.Month());
    dataFile.print("-");
    dataFile.print(eventTime.Day());
    dataFile.print(" ");
    dataFile.print(eventTime.Hour());
    dataFile.print(":");
    dataFile.print(eventTime.Minute());
    dataFile.print(":");
    dataFile.println(eventTime.Second());
    dataFile.close();
    Serial.println("Event recorded in SD card.");
  } else {
    Serial.println("Error opening events.txt!");
  }
}