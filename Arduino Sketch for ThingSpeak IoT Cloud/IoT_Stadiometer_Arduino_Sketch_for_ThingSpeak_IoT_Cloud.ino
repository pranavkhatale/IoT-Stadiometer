#include <ESP8266WiFi.h>
int t = 0;
int cloudtimer=11;
int i;
float rawheight;
float heightraw;
float height;

// NodeMCU ESP8266 with HC-SR04
const int trigPin = 12; //D6
const int echoPin = 14; //D5
//define sound velocity in cm/uS
#define SOUND_VELOCITY 0.034

long duration;
float distanceCm;

// NodeMCU with I2C LCD
// D1 >> SCL
// D2 >> SDA
#include <LiquidCrystal_I2C.h>
// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

String apiKey = "";     //  Enter your Write API key from ThingSpeak IoT Analytics Cloud platform.

const char *ssid =  "IoTWiFi1";     // Replace with your Wi-Fi ssid.
const char *pass =  "IoTWiFi1"; // // Replace with your Wi-Fi wpa2 key.
const char *server = "api.thingspeak.com"; //Initializing Server.
WiFiClient client;
 
void setup() 
{
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(D3, OUTPUT); //Buzzer
  pinMode(D4, OUTPUT); //Onboard Blue LED
  pinMode(D8, OUTPUT); //Red LED
  
  // Program written in setup will run only once for connecting to Wi-Fi network.
  Serial.begin(115200); // Selecting the baud rate for Serial Communication.
  delay(500);
  Serial.println("IoT Stadiometer");

  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("      IoT");
  lcd.setCursor(0, 1);
  lcd.print("  Stadiometer");
  delay(2000);
  lcd.clear();
  
  Serial.println("Connecting to ");
  Serial.println(ssid);
  
  lcd.setCursor(0, 0);
  lcd.print("Connecting to");
  lcd.setCursor(0, 1);
  lcd.print("Wi-Fi Network...");
  delay(2000);
  lcd.clear();
  
  WiFi.begin(ssid, pass); // Wi-Fi network searching starts.

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
    }
    
  Serial.println("");
  Serial.println("Wi-Fi connected."); //Prints on serial monitor if Wi-Fi gets connected sucessfully.

  lcd.setCursor(0, 0);
  lcd.print("Connected to");
  lcd.setCursor(0, 1);
  lcd.print("IoT WiFi Network");
  delay(2000);
  lcd.clear();
}
 
void loop() 
{
  if (client.connect(server,80))   // "184.106.153.149" or api.thingspeak.com
  {
      if(t<1)
      {
        digitalWrite(D3, HIGH);   // turn on buzzer
        digitalWrite(D4, HIGH);   // turn on onboard Blue LED
        digitalWrite(D8, HIGH);   // turn on Red LED
        delay(500);
      
        digitalWrite(D3, LOW);    // turn off buzzer
        digitalWrite(D4, LOW);    // turn off onboard Blue LED
        digitalWrite(D8, LOW);   // turn off Red LED
        delay(500);
      
        digitalWrite(D3, HIGH);   // turn on buzzer
        digitalWrite(D4, HIGH);   // turn on onboard Blue LED
        digitalWrite(D8, HIGH);   // turn on Red LED
        delay(500);
      
        digitalWrite(D3, LOW);    // turn off buzzer
        digitalWrite(D4, LOW);    // turn off onboard Blue LED
        digitalWrite(D8, LOW);    // turn off Red LED
        delay(500);
        t=t+1;
      }
       
    lcd.setCursor(0, 0);
    lcd.print("    Reading");
    lcd.setCursor(0, 1);
    lcd.print("    Height");
    delay(500);
    lcd.clear();

    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);

    // Calculate the distance
    distanceCm = duration * SOUND_VELOCITY / 2;

    // Prints the distance on the Serial Monitor
    Serial.print("Distance (cm): ");
    Serial.println(distanceCm);
  
    rawheight = 200 - distanceCm;
    
    //Serial.print("rawheight (cm): ");
    //Serial.println(rawheight);

    if (rawheight >= 100)
    {
      delay(5000);
      // Clears the trigPin
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      // Sets the trigPin on HIGH state for 10 micro seconds
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);

      // Reads the echoPin, returns the sound wave travel time in microseconds
      duration = pulseIn(echoPin, HIGH);

      // Calculate the distance
      distanceCm = duration * SOUND_VELOCITY / 2;

      // Prints the distance on the Serial Monitor
      Serial.print("Distance (cm): ");
      Serial.println(distanceCm);

      heightraw = 200 - distanceCm;

      if (heightraw >= 100)
      {
           height = heightraw;
           String postStr = apiKey;
           postStr +="&field1=";
           postStr += String(height);
           postStr += "\r\n\r\n";

           client.print("POST /update HTTP/1.1\n");
           client.print("Host: api.thingspeak.com\n");
           client.print("Connection: close\n");
           client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
           client.print("Content-Type: application/x-www-form-urlencoded\n");
           client.print("Content-Length: ");
           client.print(postStr.length());
           client.print("\n\n");
           client.print(postStr);
           Serial.println("Data sent to Thingspeak IoT Analytics Cloud platform sucessfully.");

          // Prints the distance on the Serial Monitor
          Serial.print("Height (cm): ");
          Serial.println(height);
          Serial.println("Data Uploaded Sucessfully.");
          Serial.println("                                ");

        // set cursor to first column, first row
        lcd.setCursor(0, 0);
        // print message
        lcd.print("Height:");
        lcd.print(height);
        lcd.print(" cm");
        lcd.setCursor(0, 1);
        lcd.print(" Data Uploaded");
        digitalWrite(D3, HIGH);   // turn on buzzer
        digitalWrite(D8, HIGH);   // turn on Red LED
        delay(5000);
        // clears the display to print new message
        lcd.clear();
        digitalWrite(D3, LOW);   // turn off buzzer
        digitalWrite(D8, LOW);   // turn off Red LED

      for(i=6;i--;i<=0)
      {
      lcd.setCursor(0,0);
      lcd.print("Wait for");
      lcd.setCursor(0,1);
      lcd.print(i);
      lcd.print(" seconds");
      delay(1000);
      lcd.clear();
      }
      }
    }
 }
 client.stop();
}
