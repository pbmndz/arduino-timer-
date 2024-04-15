#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Countimer.h" //https://github.com/inflop/Countimer

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#include <EEPROM.h>
Countimer tdown;

#define bt_set    A3
#define bt_up     A2
#define bt_down   A1
#define bt_start  A0

int time_s = 0;
int time_m = 0;
int time_h = 0;

int set = 0;
int flag1 = 0, flag2 = 0;


int buzzer = 6;

void setup() {
  Serial.begin(9600);
  pinMode(bt_set,   INPUT);
  pinMode(bt_up,    INPUT);
  pinMode(bt_down,  INPUT);
  pinMode(bt_start, INPUT);

  pinMode(buzzer, OUTPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

  display.display(); // Show initial display buffer contents on the screen

  tdown.setInterval(print_time, 999);
  eeprom_read();
}

void print_time() {
  time_s = time_s - 1;
  if (time_s < 0) {time_s = 59; time_m = time_m - 1;}
  if (time_m < 0) {time_m = 59; time_h = time_h - 1;}
}

void tdownComplete() {
  Serial.print("ok");
}

void loop() {
  tdown.run();

  int analogSet = analogRead(bt_set);
  int analogUp = analogRead(bt_up);
  int analogDown = analogRead(bt_down);
  int analogStart = analogRead(bt_start);

  if (analogSet >= 700 && analogSet <= 880) {
    if (flag1 == 0 && flag2 == 0) {
      flag1 = 1;
      set = set + 1;
      if (set > 3) {set = 0;}
      delay(100);
    }
  }
  else {flag1 = 0;}

  if (analogUp >= 700 && analogUp <= 880) {
    if (set == 0) {tdown.start(); flag2 = 1;}
    if (set == 1) {time_s++;}
    if (set == 2) {time_m++;}
    if (set == 3) {time_h++;}
    if (time_s > 59) {time_s = 0;}
    if (time_m > 59) {time_m = 0;}
    if (time_h > 99) {time_h = 0;}
    if (set > 0) {eeprom_write();}
    delay(200);
  }

  if (analogDown >= 700 && analogDown <= 880) {
    if (set == 0) {tdown.stop(); flag2 = 0;}
    if (set == 1) {time_s--;}
    if (set == 2) {time_m--;}
    if (set == 3) {time_h--;}
    if (time_s < 0) {time_s = 59;}
    if (time_m < 0) {time_m = 59;}
    if (time_h < 0) {time_h = 99;}
    if (set > 0) {eeprom_write();}
    delay(200);
  }

  if (analogStart >= 700 && analogStart <= 880) {
    flag2 = 1;
    eeprom_read();
    tdown.restart();
    tdown.start();
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(50, 10);
  
  if (set == 0) {display.println("Timer");}
  display.setCursor(30, 10);
  if (set == 1) {
    display.println("Set Timer SS");}
  if (set == 2) {display.println("Set Timer MM");}
  if (set == 3) {display.println("Set Timer HH");}

//to center
  display.setCursor(40, 20);
  display.setTextSize(1);  
  if (time_h <= 9) {display.print("0");}
  display.print(time_h);
  display.print(":");
  if (time_m <= 9) {display.print("0");}
  display.print(time_m);
  display.print(":");
  if (time_s <= 9) {display.print("0");}
  display.print(time_s);
  display.display();
  delay(1);

  if (time_s == 0 && time_m == 0 && time_h == 0 && flag2 == 1) {
    flag2 = 0;
    tdown.stop();
    
    digitalWrite(buzzer, HIGH);
    delay(300);
    digitalWrite(buzzer, LOW);
    delay(200);
    digitalWrite(buzzer, HIGH);
    delay(300);
    digitalWrite(buzzer, LOW);
    delay(200);
    digitalWrite(buzzer, HIGH);
    delay(300);
    digitalWrite(buzzer, LOW);
   
  }

}

void eeprom_write() {
  EEPROM.write(1, time_s);
  EEPROM.write(2, time_m);
  EEPROM.write(3, time_h);
}

void eeprom_read() {
  time_s =  EEPROM.read(1);
  time_m =  EEPROM.read(2);
  time_h =  EEPROM.read(3);
}
