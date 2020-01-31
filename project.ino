#include <Ticker.h>  
#include "DHTesp.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
const char* ssid = "COSMOTE-D2220E"; // Enter the SSID of your WiFi Network.
const char* password = "gKeNRP7XSqc5HqXg";// Enter the Password of your WiFi Network.
char server[] = "mail.smtp2go.com"; // The SMTP Server


LiquidCrystal_I2C  lcd(0x27, 2, 1, 0, 4, 5, 6, 7);
DHTesp dht;


int echoPin = 13;
int trigPin = 12;
int t1 = 0, t2 = 0;
float t = 0;
float avg = 0;
int sec_counter = 1;
int Wflag = 0;
int  AVflag;

WiFiClient espClient;
void wifi_setup()
{
  Serial.begin(115200);
  delay(10);
  Serial.println("");
  Serial.println("");
  Serial.print("Connecting To: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("*");
  }
  Serial.println("");
  Serial.println("WiFi Connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}
Ticker blinker;
int changeState()
{
  t1++;
}

void setup()
{ pinMode(D6, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  lcd.begin (16, 2);
  lcd.setBacklightPin(3, POSITIVE);
  lcd.setBacklight(HIGH);
  Serial.begin(115200);
  blinker.attach(5, changeState);
}

void loop()
{
  int counter = 0;
  int sec_counter = 1;
  int time=0;;
  float avg = 0;
  float sum = 0;
  int flag1 = 0, flag2 = 0;
  double start_time = 0;
  dht.setup(D5, DHTesp::DHT11);
  for (;;) {
    if (t1 != t2) {
      t2 = t1;
      t = dht.getTemperature();
      counter++;
      sum = sum + t;
      Serial.println(t);
    }
    if (t > 30) {

      digitalWrite(D3, HIGH);
      digitalWrite(D4, LOW);
      lcd.clear();
      lcd.print("Temp too high");
      flag2 = 1;
      if (Wflag == 0) {

        byte ret = sendEmail();
        Wflag=1;
      }

    } else if (t < -10) {
      digitalWrite(D3, LOW);
      digitalWrite(D4, HIGH);

      lcd.clear();
      lcd.print("Temp too low");
      flag2 = 1;
      if (Wflag == 0) {

        byte ret = sendEmail();
 
         Wflag=1;
      }

    } else {
      if (flag2 == 1) {
        lcd.clear();
        flag2 = 0;
         Wflag=0;
      }

      digitalWrite(D3, LOW);
      digitalWrite(D4, LOW);
    }

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    time = pulseIn(echoPin, HIGH);

    if (time < 3000) {
      if (flag1 == 0) {
        flag1 = 1;
        start_time = millis();
        Serial.println("open screen");
        lcd.home();
        lcd.print("Temperature=");
        lcd.print(t);
        lcd.setCursor(0, 1);
        lcd.print("Average=");
        lcd.print(avg);
      }
    }

    if (flag1 == 1 && millis() - start_time > 10000) {
      Serial.println("close screen");
      flag1 = 0;
      lcd.clear();
      //svinei i othoni
    }

    if (counter == 24) {
      counter = 0;
      avg = sum / 24;
      sum = 0;
      AVflag=1;
      if (sec_counter=6)
      {
        sec_counter = 0;
      }
      else
      {
        byte ret = sendEmail(); 
      }
      byte ret = sendEmail();
      sec_counter++;
      AVflag=0;
      if (flag1 == 0) {
        flag1 = 1;
        start_time = millis();
        Serial.println("open screen");
        lcd.home();
        lcd.print("Average=");
        lcd.print(avg);
      }
    }
  }
}

byte sendEmail()
{
  if (espClient.connect(server, 2525) == 1)
  {
    Serial.println(F("connected"));
  }
  else
  {
    Serial.println(F("connection failed"));
    return 0;
  }
  if (!emailResp())
    return 0;
  
  Serial.println(F("Sending EHLO"));
  espClient.println("EHLO www.example.com");
  if (!emailResp())
    return 0;
  
  Serial.println(F("Sending auth login"));
  espClient.println("AUTH LOGIN");
  if (!emailResp())
    return 0;
 
  Serial.println(F("Sending User"));
 
  
  espClient.println("eXBhdGlhd3lmeUBnbWFpbC5jb20="); //base64, ASCII encoded Username
  if (!emailResp())
    return 0;
  
  Serial.println(F("Sending Password"));
  
  espClient.println("elByamJwd1V6eXcx");//base64, ASCII encoded Password
  if (!emailResp())
    return 0;
  
  Serial.println(F("Sending From"));
  
  espClient.println(F("MAIL From: ypatiawyfy@gmail.com"));
  if (!emailResp())
    return 0;
  
  Serial.println(F("Sending To"));
  espClient.println(F("RCPT To: ypatiarduino@gmail.com"));
  if (!emailResp())
    return 0;
  
  Serial.println(F("Sending DATA"));
  espClient.println(F("DATA"));
  if (!emailResp())
    return 0;
  Serial.println(F("Sending email"));
 
  espClient.println(F("To:  ypatiarduino@gmail.com"));
 
  espClient.println(F("From: ypatiawyfy@gmail.com"));
  espClient.println(F("Subject: ESP8266 test e-mail\r\n"));
  if (AVflag == 1)
  {
    espClient.println(F("The average temperature is:"));
    espClient.println(avg);
    espClient.println(F("."));
    espClient.println(F("."));
  }
  else {
    espClient.println(F("Warning!The temperature is:"));
    espClient.println(t);
    espClient.println(F("."));

  }
  if (!emailResp())
    return 0;
  Serial.println(F("Sending QUIT"));
  espClient.println(F("QUIT"));
  if (!emailResp())
    return 0;
  espClient.stop();
  Serial.println(F("disconnected"));
  return 1;
}

byte emailResp()
{
  byte responseCode;
  byte readByte;
  int loopCount = 0;

  while (!espClient.available())
  {
    delay(1);
    loopCount++;
    // Wait for 20 seconds and if nothing is received, stop.
    if (loopCount > 20000)
    {
      espClient.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  responseCode = espClient.peek();
  while (espClient.available())
  {
    readByte = espClient.read();
    Serial.write(readByte);
  }

  if (responseCode >= '4')
  {
    return 0;
  }
  return 1;
}
