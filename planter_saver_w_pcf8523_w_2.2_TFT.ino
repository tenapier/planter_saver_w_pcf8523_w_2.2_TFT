

/*
  Automatic watering system
  This program is coded for the Arduino Nano, with a RTC_PCF8523 and a 2.2 TFT
 
 created 30 Sep. 2017
 by Piotr Trembecki
 http://pino-tech.eu/
 modified 08 Mar. 2019
 by T.E.Napier
 This example code is in the public domain.
 
 */
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Fonts/FreeSerif24pt7b.h>  // Add a custom font
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST -1
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

 
#include <Wire.h> 
#include <RTClib.h>
RTC_PCF8523 rtc;

#define SOILWATCH_PIN             A0  //connect output of Soilwatch 10 to this analog pin (deafult A0)
#define PUMP_PIN                  2   //connect to pump mosfet
#define DOWN_PIN                  4   //Swap this two if buttons work in reverse
#define UP_PIN                    3   //Swap this two if buttons work in reverse
#define MIN_TIME_BETWEEN_WATERING 2   //minimum time between watering plant in seconds
#define MOISTURE_SET              0   //Set desired moisture level
#define WATER_FOR_SEC             5   //Set time for single watering in seconds


char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


void setup()
{
  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (! rtc.initialized()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  pinMode ( UP_PIN, INPUT );
  pinMode ( DOWN_PIN, INPUT );

  //Set analog reference to external 5.0V
  analogReference(EXTERNAL);
  
 

  tft.begin();
  tft.fillScreen(0X0000);
  tft.setRotation(1);
 //tft.fillScreen(ILI9341_BLACK);
  
  tft.setFont();
  //tft.setFont(&FreeSerif24pt7b); // Set a custom font
  //tft.setFont(&FreeMonoBoldOblique12pt7b);
  //tft.setFont(&FreeSerif9pt7b);
  tft.setTextSize(3);                // Set text size. We are using custom font so you should always set text size as 0
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(10, 100);
  tft.println("Plant Saver v.1.3");
  delay(2000);
  
  DateTime now = rtc.now();
   tft.fillScreen(0X000);
   tft.setCursor(30,70);
   tft.setTextColor(ILI9341_GREEN);
   tft.print("Moisture: ");
   tft.setCursor(30,110);
   tft.print ("MoistureSet: ");
   tft.setCursor (30,150);
   tft.print ("LastWatered:");
}

void loop(){

    DateTime now = rtc.now();
    

   
   static int dontwaterfor = MIN_TIME_BETWEEN_WATERING*10;
   static int moistureSet = MOISTURE_SET;
   static int watering = 0;
   static int lastmoisture = 0;
   static int lastmoistureSet = 0;
   
   int analogValue = analogRead(SOILWATCH_PIN);
   analogValue = analogValue + 50;
   
   int moisture = map(analogValue, 360, 670, 100, 0);
   if (analogValue > 670) {                                    //setting basement moisture reading at zero
    moisture = 0;
   }
   if (analogValue < 360) {                                    //setting ceiling moisture reading at 100%
    moisture = 100;
   }

   tft.setTextSize(3);                // Set text size. We are using custom font so you should always set text size as 0
   tft.setTextColor(ILI9341_YELLOW);

   if (moisture!= lastmoisture)
   {tft.setCursor(200, 70);
   tft.fillRect(200, 70, 90, 40, 0x0000);                    // Draw filled rectangle (x,y,width,height,color)
   tft.print(moisture),tft.print("%");                       // removes old moisture reading
    lastmoisture = moisture;                                 // prints new moisture
   }
    if (moisture == lastmoisture) {
     tft.setCursor(200, 70);
     tft.fillRect(200, 70, 90, 40, 0x0000);
     tft.print(moisture),tft.print("%");                     //if moisture hasn't changed, maintains that number
      } 
      
   //tft.print(moisture);
   //tft.print("%   ");
   delay(200);
   

   int downButton = digitalRead(DOWN_PIN);
   int upButton = digitalRead(UP_PIN);
   if(downButton == HIGH)
   {
    if(moistureSet != 0) moistureSet--;
    delay(50);
   
   }

   if(upButton)
   {
    if(moistureSet != 100) moistureSet++;
    delay(50);
   }
   
   tft.setCursor(250,110);
   tft.print(moistureSet);
   delay(50);

      if (moistureSet!= lastmoistureSet)
   {tft.setCursor(250, 110);
   tft.fillRect(250, 110, 90, 40, 0x0000);                    // Draw filled rectangle (x,y,width,height,color)
   tft.print(moistureSet);
    lastmoistureSet = moistureSet;
   }
    if (moistureSet == lastmoistureSet) {
     tft.setCursor(250, 110);
     tft.fillRect(250, 110, 90, 40, 0x0000);
     tft.print(moistureSet);
      } 
   

 if (daysOfTheWeek[now.dayOfTheWeek()] != daysOfTheWeek[now.dayOfTheWeek()]) {    //remove old day
   tft.setCursor(20,20 );
   tft.fillRect(20,20,100,30,0X000);
  }
  
  if (now.month() != now.month()){                                              //remove old month
    tft.setCursor(140,20);
    tft.fillRect(140,20,40,30,0X000);
  }

  if(now.day() != now.day()){                                                   //remove old day date
    tft.setCursor(170,20);
    tft.fillRect(170,20,50,30,0X000);
  }

  if(now.year() != now.year()){                                                //remove old year
   tft.setCursor(210,20);
   tft.fillRect(210,20,80,30,0X000);
  }
  
  
  tft.setTextSize(2);                   // Set text size. We are using custom font so you should always set text size as 0
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(20,20 );
  tft.fillRect(20,20,280,30,0X000);
  tft.setCursor (20,20);
  tft.print (daysOfTheWeek[now.dayOfTheWeek()]);          // print day of Week
  tft.print (" ");                                        // 1 empty space after week day, before date
  tft.setCursor(140,20);
  tft.print (now.month(), DEC);                           //  print month
  tft.print ("/");
  tft.setCursor(170,20);
  tft.print (now.day(), DEC);                             // print day date
  tft.print ("/");
  tft.setCursor(210,20);
  tft.print (now.year(), DEC);                            // print month date
  //tft.print("      "); 


   if(dontwaterfor >= 1)dontwaterfor--;
   
   if(((moisture < moistureSet) && (!dontwaterfor)) || (watering))
   {

      if(watering == 0)watering = WATER_FOR_SEC*10;
      if(watering > 1){

        digitalWrite(PUMP_PIN, HIGH);
        
        tft.setTextSize(2);
        tft.setTextColor(ILI9341_YELLOW);
        
      
        if (now.month() != now.month()){                                              //remove old month
        tft.setCursor(100,200);
        tft.fillRect(100,200,30,30,0X000);
          }

         if(now.day() != now.day()){                                                   //remove old day date
         tft.setCursor(170,200);
          tft.fillRect(170,120,40,30,0X000);
          }

         if(now.year() != now.year()){                                                //remove old year
         tft.setCursor(210,200);
          tft.fillRect(210,120,80,30,0X000);
          }

         tft.setCursor(100,200);
         tft.fillRect(100,200,100,30,0X000);
         tft.print(now.month(), DEC);
         tft.print("/");
         tft.print(now.day(), DEC);
         tft.print("/");
         tft.print(now.year(), DEC);


         tft.setCursor(100,180);
         tft.fillRect(100,180,80,20,0X000);
         tft.setCursor(100,180);
         tft.print(now.hour(), DEC);
         tft.print(":");
         tft.print(now.minute(), DEC);
         }
          
      
      
            else
      { 
        digitalWrite(PUMP_PIN, LOW);
        dontwaterfor = MIN_TIME_BETWEEN_WATERING*10;
      }

      watering--;
   
 
   delay (100); }}
