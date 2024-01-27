//DHT11
#include<Wire.h>
#include <DHT.h>  // library DHT11
DHT dht(D7, DHT11); //Pin, Jenis DHT11
int kipas = D0;
int miss = D8;


//RTC
#include "RTClib.h"
RTC_DS3231 rtc;
char dataHari[7][12] = {"Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu"};
String hari;
int tanggal, bulan, tahun, jam, menit, detik;
float suhu;
int statusPakan = 0;
float i = 0;

//LCD   
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Servo
#include <Servo.h>
Servo servo1;
Servo servo2;

//LOAD CELL
#include <HX711_ADC.h>
const int HX711_dout = D3;   //mcu HX711 sck pin
const int HX711_sck = D4;   //mcu HX711 dout sck
HX711_ADC LoadCell (HX711_dout, HX711_sck);

void setup () {
  Serial.begin(9600);
  lcd.begin();
  dht.begin();

//DHT11
  pinMode(kipas, OUTPUT);
  pinMode(miss, OUTPUT);
  digitalWrite(kipas, HIGH);
  digitalWrite(miss, LOW);

//Servo
  servo1.attach(D5);
  servo1.write(0);
  servo2.attach(D6);
  servo2.write(0);
  
//RTC
  if (! rtc.begin()) {
    Serial.println("RTC Tidak Ditemukan");
    Serial.flush();
    abort();
  }

//Atur Waktu
//  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//  rtc.adjust(DateTime(2022, 12, 27, 21, 50, 00));

//LOAD CELL
  LoadCell.begin();
  LoadCell.start(2000);
  LoadCell.setCalFactor(1084);
}

void loop () {  
//DHT11
  int kelembaban = dht.readHumidity();
  int suhu = dht.readTemperature();

  Serial.print("Kelembaban: "); //menampilkan tulisan Kelembaban di Serial Monitor
  Serial.print(kelembaban); //menampilkan nilai kelembaban
  Serial.print(" Suhu: "); //menampilkan tulisan suhu
  Serial.println(suhu); //menampilkan nilai 

  

//  lcd.setCursor(0, 0);
//  lcd.print("Suhu:");
//  lcd.setCursor(6, 0);
//  lcd.print(suhu);
//  lcd.setCursor(8, 0);
//  lcd.print(".C");

//  lcd.setCursor(0, 1);
//  lcd.print("Kelembaban:");
//  lcd.setCursor(12, 1);
//  lcd.print(kelembaban);
//  lcd.setCursor(14, 1);
//  lcd.print("%");

// lcd.setCursor(0,1);
// lcd.print("Kipas :");
// lcd.setCursor(8,1);

//KONDISI DHT11
  if (suhu > 35)
  {
    digitalWrite(kipas, LOW);
    // lcd.print("OFF");
  }
  else
  {
    digitalWrite(kipas, HIGH);
    // lcd.print("ON");
  }
  if (kelembaban <= 60)
  {
    digitalWrite(miss, HIGH);
    // lcd.print("ON");
  }
  else
  {
    digitalWrite(miss, LOW);
    // lcd.print("OFF");
  }
  
//RTC 
  DateTime now = rtc.now();
  hari    = dataHari[now.dayOfTheWeek()];
  tanggal = now.day(), DEC;
  bulan   = now.month(), DEC;
  tahun   = now.year(), DEC;
  jam     = now.hour(), DEC;
  menit   = now.minute(), DEC;
  detik   = now.second(), DEC;
//suhu    = rtc.getTemperature();
  Serial.println(String() + hari + ", " + tanggal + "-" + bulan + "-" + tahun);
  Serial.println(String() + jam + ":" + menit + ":" + detik);
//Serial.println(String() + "Suhu: " + suhu + " C");
  Serial.println();
  

//LCD
  lcd.setCursor(0,0);
  lcd.print(String() + hari + "," + tanggal + "-" + bulan + "-" + tahun);
  lcd.print(" ");
  lcd.setCursor(0,1);
  lcd.print(String("Pukul: ") + jam + ":" + menit + ":" + detik);
  lcd.print(" ");
//lcd.print(suhu);
//lcd.print(" ");

//KONDISI RTC&LOADCELL
  if (jam == 22 && menit == 17 && statusPakan == 0){
    servo1.write(180);
    Serial.println("status pakan ");
    Serial.println(statusPakan);
    Serial.println("Servo 1 buka");
    float berat = getDataLoadCell();
    if(berat > 20.0){
      Serial.println("berat sudah 20 kg");
       servo1.write(0);
       Serial.println("Servo 1 Tutup.. Tunggu 3 Detik Buka Servo 2");
       delay(5000);
       servo2.write(180);
       Serial.println("Servo 2 buka.. tunggu 3 detik Servo 2 Tutup");
       delay(5000);
       servo2.write(0);
       statusPakan = 1;
       berat = 0;
       Serial.println("Status Pakan = 1");
    }
  }
  if (jam ==22 && menit == 19 && statusPakan == 1){
    servo1.write(180);
    Serial.println("status pakan ");
    Serial.println(statusPakan);
    Serial.println("Servo 1 buka");
    float berat = getDataLoadCell();
    if(berat > 20.0){
      Serial.println("berat sudah 20 kg");
       servo1.write(0);
       Serial.println("Servo 1 Tutup.. Tunggu 3 Detik Buka Servo 2");
       delay(5000);
       servo2.write(180);
       Serial.println("Servo 2 Buka.. Tunggu 3 Detik Servo 2 Tutup");
       delay(5000);
       servo2.write(0);
       statusPakan = 0;
       berat = 0;
       Serial.println("Status Pakan = 1");
    }
  }
  Serial.println(getDataLoadCell());
  delay(1000);
  
}
float getDataLoadCell(){
  LoadCell.update();
  float i = LoadCell.getData();
  if(i<0)(i=0);
  Serial.print("Wight[g]:");
  return i;
}
