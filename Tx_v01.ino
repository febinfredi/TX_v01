#include <LiquidCrystal.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);  //(rs, enable, d4, d5, d6, d7)
RF24 radio(7, 8);                     // CE, CSN

const byte address[6] = "00001";      // nrf communication address

const int joyl_x = A0;                //left joystick x axis
const int joyl_y = A1;                //left joystick y axis
const int joyr_x = A6;                //right joystick x axis
const int joyr_y = A7;                //right joystick y axis
const int pot1 = A2;                  //potentiometer 1
const int pot2 = A3;                  //potentiometer 2

struct data_pack {                     //data structure for storing data for transmitting
  byte xval_l;
  byte yval_l;
  byte xval_r;
  byte yval_r;
  byte trim1;
  byte trim2;
};

data_pack data;                         //data structure object "data"

void setup() {
  Serial.begin(115200);

  radio.begin();
  radio.openWritingPipe(address);        //00001
  radio.setAutoAck(false);               //auto acknowledge set to false
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.stopListening();

  lcd.begin(16, 2);                      //init LCD
  lcd.clear();                           //clear LCD screen

  pinMode(joyl_x, INPUT);                //assigning input mode to control pins
  pinMode(joyl_y, INPUT);
  pinMode(joyr_x, INPUT);
  pinMode(joyr_y, INPUT);
  pinMode(pot1, INPUT);
  pinMode(pot2, INPUT);
}

void loop() {

  // mapping values to 0 to 255
  data.xval_l = map(analogRead(joyl_x), 1023, 0, 0, 255);
  data.yval_l = map(analogRead(joyl_y), 521, 1023, 0, 255);
  data.xval_r = map(analogRead(joyr_x), 0, 1023, 0, 255);
  data.yval_r = map(analogRead(joyr_y), 0, 1023, 0, 255);
  data.trim1 = map(analogRead(pot1), 0, 1023, 0, 255);
  data.trim2 = map(analogRead(pot2), 0, 1023, 0, 255);

  //Serial.println(data.xval_l);
  //Serial.println(data.yval_l);
  //Serial.println(data.xval_r);
  //Serial.println(data.yval_r);
  //Serial.println(data.trim1);
  //Serial.println(data.trim2);

  if (radio.write(&data, sizeof(data_pack))) {

    lcd.clear();                          //printing throttle value on LCD
    lcd.setCursor(0, 0);
    int t_perc = (float(data.yval_l) / 255) * 100;
    lcd.print("T:" + String(t_perc));
    //Serial.print("Th: " + String(t_perc) + "%");

    lcd.setCursor(7, 0);                  //printing roll value on LCD
    if (data.xval_r < 119) {
      lcd.print("Rl:L");
      //Serial.print(" Roll: Left ");
    }
    else if (data.xval_r > 122) {
      lcd.print("Rl:R");
      //Serial.print(" Roll: Right ");
    }

    lcd.setCursor(12, 0);                   //printing rudder value on LCD
    if (data.xval_l < 125) {
      lcd.print("Rd:L");
      //Serial.print("Rudder: Left ");
    }
    else if (data.xval_l > 125) {
      lcd.print("Rd:R");
      Serial.print("Rudder: Right ");
    }

    lcd.setCursor(0, 1);                  //printing pitch value on LCD
    if (data.yval_r < 126) {
      lcd.print("P:D");
      //Serial.print("Pch: Down ");
    }
    else if (data.yval_r > 130) {
      lcd.print("P:U");
      //Serial.print("Pch: UP ");
    }

    lcd.setCursor(4, 1);                   //printing trim 1 value on LCD
    int angl1 = map(analogRead(pot1), 0, 1023, -90, 90);
    lcd.print("T1:" + String(angl1 / 10));
    //Serial.print("Trim1:" + String(angl1));

    lcd.setCursor(10, 1);                  //printing trim 2 value on LCD
    int angl2 = map(analogRead(pot2), 0, 1023, -90, 90);
    lcd.print("T1:" + String(angl2 / 10));
    //Serial.println("Trim2:" + String(angl2));
  }

  else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Data send failed");
    //Serial.println("Data send failed");
  }
}
