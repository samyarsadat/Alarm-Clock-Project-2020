/*
   Alarm Clock Project Code .
   Written By Samyarsadatakhavi .
   Last Updated : 3/22/2020 - 9:20 PM

   Written For Arduino Mega 2560 .
   Wiring Diagram Is Inside The "Circuit Diagrams" Folder .
*/

//Libraries :
#include <Nextion.h> //Nextion Library .
#include <DS3231.h> //DS3231 RTC Module Library .
#include <Wire.h> //Wire Library For RTC Module .
#include <DFPlayer_Mini_Mp3.h> //DF Player Mini Library .
#include "DHT.h" //DHT Library For DHT11 Temperature Sensor .
#include <Adafruit_NeoPixel.h> //Adafruit NeoPixel Library .

//Pin Defines :
#define Temp_Pin 3 //DS18B20 Temperature Sensor Pin Define .
#define Serial_TX 11 //Software Serial TX Pin Define .
#define Serial_RX 10 //Software Serial RX Pin Define .
#define Neo_Pixel_Pin 4 //Neo Pixel Pin Define .
#define LDR A0 //LDR Sensor Pin Define .

#define Dht_Type DHT11 //Setting The DHT Sensor Type ( Because The DHT Library Supports Diferent Sensor Type's )
#define Neo_Pixel_Num 24 //Setting The Number Of Neo Pixel LED's In Our Ring . ( In My Case There Are 24 LED's )
#define Ambient_Light_Min 480 //The Minimum Analog Value Of The LDR ( In Ambient Light ) .
#define Ambient_Light_Max 810 //The Maximum Analog Value Of The LDR ( In Full Darkness ) .

DHT sensor(Temp_Pin, Dht_Type); //DHT Sensor Setup .

Adafruit_NeoPixel ring = Adafruit_NeoPixel(Neo_Pixel_Num, Neo_Pixel_Pin, NEO_GRB + NEO_KHZ800); //Neo Pixel Setup .

SoftwareSerial serial(Serial_RX, Serial_TX); //Software Serial Setup .

DS3231  rtc(SDA, SCL); //DS3231 RTC Setup .

Nextion nextion(serial, 9600); //Nextion LCD Setup .

//Variables :
//Intager :
int Auto;
int stat;
int Screen_Auto;
int date_year = 2020;
int date_month = 01;
int date_day = 01;
int time_hour = 12;
int time_minute = 30;
int alarm_minute = 30;
int alarm_hour = 12;
int alarm_year = 2020;
int alarm_month = 01;
int alarm_day = 01;
int alarm;
int volume = 10;

//This Function Shows The Time , Date And The Temperature On The Main Page Of The Nextion Display .
void Main_Screen(void)
{
  Time t = rtc.getTime();
  int Hour = t.hour;
  int Min = t.min;

  nextion.setComponentText("t0", String(Hour));
  nextion.setComponentText("t2", String(Min));
  nextion.setComponentText("t7", rtc.getDateStr());
  nextion.setComponentValue("n1", int(sensor.readTemperature()));
}

void screen_auto(void)
{
  int dim = map(analogRead(A0), Ambient_Light_Min, Ambient_Light_Max, 100, 0);
  String cmd = "dim=" + String(dim);

  nextion.sendCommand(cmd.c_str());
}

void setup()
{
  pinMode(LDR, INPUT); //Setting The LDR Pin As An Input .

  Wire.begin(); //Starting The Wire Library .

  Serial.begin(9600); //Starting The Serial Port .
  Serial1.begin(9600); //Starting The Second Serial Port .
  serial.begin(9600); //Starting The Software Serial Port .

  sensor.begin(); //Initializing The Temperature Sensor .

  ring.begin(); //Initializing The Neo Pixel .
  ring.setBrightness(100); //Setting The Brightnes Of The Neo Pixel .
  ring.show(); //Turning All Of The Neo Pixel LED's Off .

  mp3_set_serial(Serial1); //Telling The DF Player To Use The Second Serial Port .
  mp3_set_volume(10); //Setting The Volume .

  rtc.begin(); //Initializing The RTC Module .

  nextion.init(); //Initializing The Nextion LCD .

  randomSeed(analogRead(A1));

  nextion.sendCommand("dim=100");

  //rtc.setTime(11, 47, 20);
  //rtc.setDate(21, 3, 2020);
}

void loop()
{
  String command = nextion.listen();

  if (command == "")
  {
    Main_Screen();

    Serial.println("Main Screen Running .");

    if (alarm == 1)
    {
      nextion.setComponentText("t5", "Alarm ON");

      Time t = rtc.getTime();
      int Hour = t.hour;
      int Min = t.min;

      String Date;

      if (alarm_month > 9)
      {
        Date = String(alarm_day) + "." + String(alarm_month) + "." + String(alarm_year);
      }

      if (alarm_month < 10)
      {
        Date = String(alarm_day) + "." + "0" + String(alarm_month) + "." + String(alarm_year);
      }

      if (Date == rtc.getDateStr() and Hour == alarm_hour and Min == alarm_minute)
      {
        for (int i = 0; i < Neo_Pixel_Num; i++)
        {
          ring.setPixelColor(i, 255, 0, 0);
          ring.show();
          delay(10);
        }

        mp3_set_volume(20);
        mp3_play(3);
        nextion.sendCommand("page 7");
        alarm = 0;

        while (1)
        {
          String command = nextion.listen();

          if (command == "65 7 4 1 ffff ffff ffff")
          {
            mp3_stop();
            mp3_set_volume(volume);

            for (int i = 24; i > -1; i--)
            {
              ring.setPixelColor(i, 0, 0, 0);
              ring.show();
              delay(50);
            }

            stat = 0;
            nextion.sendCommand("page 0");
            break;
          }
        }
      }
    }

    else
    {
      nextion.setComponentText("t5", "Alarm OFF");
    }

    if (Auto == 1)
    {
      Serial.println("Auto LED Running .");

      int mode = random(0, 6);

      if (mode == 0)
      {
        for (int i = 0; i < Neo_Pixel_Num; i++)
        {
          ring.setPixelColor(i, 255, 0, 0);
          ring.show();
          delay(10);
        }
      }

      if (mode == 1)
      {
        for (int i = 0; i < Neo_Pixel_Num; i++)
        {
          ring.setPixelColor(i, 0, 255, 0);
          ring.show();
          delay(10);
        }
      }

      if (mode == 2)
      {
        for (int i = 24; i > -1; i--)
        {
          ring.setPixelColor(i, 0, 0, 255);
          ring.show();
          delay(10);
        }
      }

      if (mode == 3)
      {
        for (int i = 24; i > -1; i--)
        {
          ring.setPixelColor(i, 255, 255, 0);
          ring.show();
          delay(10);
        }
      }

      if (mode == 4)
      {
        for (int i = 0; i < Neo_Pixel_Num; i++)
        {
          ring.setPixelColor(i, 255, 0, 255);
          ring.show();
          delay(10);
        }
      }

      if (mode == 5)
      {
        for (int i = 24; i > -1; i--)
        {
          ring.setPixelColor(i, 0, 204, 255);
          ring.show();
          delay(10);
        }
      }

      if (mode == 6)
      {
        for (int i = 0; i < Neo_Pixel_Num; i++)
        {
          ring.setPixelColor(i, 255, 0, 162);
          ring.show();
          delay(10);
        }
      }
    }

    if (Screen_Auto == 1)
    {
      screen_auto();
    }
  }

  else
  {
    Serial.println("Message Recived , In Else .");

    nextion.setComponentText("t0", "Settings");

    while (1)
    {

settings:

      Serial.println("In Settings While .");

      String command = nextion.listen();

      if (command == "65 1 3 1 ffff ffff ffff")
      {
        Serial.println("Set Time .");

        nextion.setComponentText("t1", String(time_minute));
        nextion.setComponentText("t0", String(time_hour));

        while (1)
        {
          Serial.println("Set Time While .");

          String command = nextion.listen();

          if (command == "65 3 8 1 ffff ffff ffff")
          {
            time_minute++;
            nextion.setComponentText("t1", String(time_minute));
          }

          if (command == "65 3 7 1 ffff ffff ffff")
          {
            time_minute--;
            nextion.setComponentText("t1", String(time_minute));
          }

          if (command == "65 3 5 1 ffff ffff ffff")
          {
            time_hour++;
            nextion.setComponentText("t0", String(time_hour));
          }

          if (command == "65 3 6 1 ffff ffff ffff")
          {
            time_hour--;
            nextion.setComponentText("t0", String(time_hour));
          }

          if (command == "65 3 a 1 ffff ffff ffff")
          {
            rtc.setTime(time_hour, time_minute, 30);

            time_hour = 12;
            time_minute = 30;
          }

          if (command == "65 3 1 1 ffff ffff ffff")
          {
            break;
          }
        }
      }

      if (command == "65 1 7 1 ffff ffff ffff")
      {
        Serial.println("Set Date .");

        nextion.setComponentText("t2", String(date_day));
        nextion.setComponentText("t3", String(date_year));
        nextion.setComponentText("t4", String(date_month));

        while (1)
        {
          Serial.println("Set Date While .");

          String command = nextion.listen();

          if (command == "65 4 8 1 ffff ffff ffff")
          {
            date_year++;
            nextion.setComponentText("t3", String(date_year));
          }

          if (command == "65 4 7 1 ffff ffff ffff")
          {
            date_year--;
            nextion.setComponentText("t3", String(date_year));
          }

          if (command == "65 4 a 1 ffff ffff ffff")
          {
            date_month++;
            nextion.setComponentText("t4", String(date_month));
          }

          if (command == "65 4 9 1 ffff ffff ffff")
          {
            date_month--;
            nextion.setComponentText("t4", String(date_month));
          }

          if (command == "65 4 c 1 ffff ffff ffff")
          {
            date_day++;
            nextion.setComponentText("t2", String(date_day));
          }

          if (command == "65 4 b 1 ffff ffff ffff")
          {
            date_day--;
            nextion.setComponentText("t2", String(date_day));
          }

          if (command == "65 4 e 1 ffff ffff ffff")
          {
            rtc.setDate(date_day, date_month, date_year);

            date_year = 2020;
            date_month = 01;
            date_day = 01;
          }

          if (command == "65 4 1 1 ffff ffff ffff")
          {
            break;
          }
        }
      }

      if (command == "65 1 4 1 ffff ffff ffff")
      {
        if (alarm == 0)
        {
          Serial.print("Set Alarm .");

          nextion.sendCommand("page 5");

          nextion.setComponentText("t1", String(alarm_hour));
          nextion.setComponentText("t2", String(alarm_minute));

          while (1)
          {
            Serial.print("Set Alarm While .");

            String command = nextion.listen();

            if (command == "65 5 a 1 ffff ffff ffff")
            {
              alarm_minute++;
              nextion.setComponentText("t2", String(alarm_minute));
            }

            if (command == "65 5 9 1 ffff ffff ffff")
            {
              alarm_minute--;
              nextion.setComponentText("t2", String(alarm_minute));
            }

            if (command == "65 5 7 1 ffff ffff ffff")
            {
              alarm_hour++;
              nextion.setComponentText("t1", String(alarm_hour));
            }

            if (command == "65 5 8 1 ffff ffff ffff")
            {
              alarm_hour--;
              nextion.setComponentText("t1", String(alarm_hour));
            }

            if (command == "65 5 1 1 ffff ffff ffff")
            {
              break;
            }

            if (command == "65 5 3 1 ffff ffff ffff")
            {
              Serial.print("Set Alarm Page 2 .");

              nextion.setComponentText("t3", String(alarm_year));
              nextion.setComponentText("t4", String(alarm_month));
              nextion.setComponentText("t2", String(alarm_day));

              while (1)
              {
                Serial.print("Set Alarm Page 2 While .");

                String command = nextion.listen();

                if (command == "65 6 8 1 ffff ffff ffff")
                {
                  alarm_year++;
                  nextion.setComponentText("t3", String(alarm_year));
                }

                if (command == "65 6 7 1 ffff ffff ffff")
                {
                  alarm_year--;
                  nextion.setComponentText("t3", String(alarm_year));
                }

                if (command == "65 6 a 1 ffff ffff ffff")
                {
                  alarm_month++;
                  nextion.setComponentText("t4", String(alarm_month));
                }

                if (command == "65 6 9 1 ffff ffff ffff")
                {
                  alarm_month--;
                  nextion.setComponentText("t4", String(alarm_month));
                }

                if (command == "65 6 c 1 ffff ffff ffff")
                {
                  alarm_day++;
                  nextion.setComponentText("t2", String(alarm_day));
                }

                if (command == "65 6 b 1 ffff ffff ffff")
                {
                  alarm_day--;
                  nextion.setComponentText("t2", String(alarm_day));
                }

                if (command == "65 6 e 1 ffff ffff ffff")
                {
                  alarm = 1;
                  nextion.sendCommand("page 1");
                  goto settings;
                }

                if (command == "65 6 1 1 ffff ffff ffff")
                {
                  break;
                }
              }
            }
          }
        }

        if (alarm == 1)
        {
          nextion.sendCommand("page 8");
          Serial.println("Deactivate Aalarm .");

          while (1)
          {
            Serial.println("Deactivate Alarm While .");
            String command = nextion.listen();

            if (command == "65 8 2 1 ffff ffff ffff")
            {
              alarm = 0;
              nextion.sendCommand("page 1");
              break;
            }

            if (command == "65 8 3 1 ffff ffff ffff")
            {
              break;
            }
          }
        }
      }

      if (command == "65 1 8 1 ffff ffff ffff")
      {
        Serial.println("MP3 Player .");

        while (1)
        {
          Serial.println("MP3 Player While .");

          String command = nextion.listen();

          if (command == "65 a 1 1 ffff ffff ffff")
          {
            mp3_next();
          }

          if (command == "65 a 2 1 ffff ffff ffff")
          {
            mp3_prev();
          }

          if (command == "65 a 7 1 ffff ffff ffff")
          {
            volume++;
            mp3_set_volume(volume);
          }

          if (command == "65 a 8 1 ffff ffff ffff")
          {
            volume--;
            mp3_set_volume(volume);
          }

          if (command == "65 a 3 1 ffff ffff ffff")
          {
            mp3_play();
          }

          if (command == "65 a 4 1 ffff ffff ffff")
          {
            mp3_pause();
          }

          if (command == "65 a 5 1 ffff ffff ffff")
          {
            break;
          }
        }
      }

      if (command == "65 1 6 1 ffff ffff ffff")
      {
        while (1)
        {
          String command = nextion.listen();

          if (command == "65 9 4 1 ffff ffff ffff")
          {
            nextion.sendCommand("dim=100");

            Screen_Auto = 0;
            nextion.setComponentText("t1", "Auto OFF");
          }

          if (command == "65 9 5 1 ffff ffff ffff")
          {
            nextion.sendCommand("dim=10");

            Screen_Auto = 0;
            nextion.setComponentText("t1", "Auto OFF");
          }

          if (command == "65 9 3 1 ffff ffff ffff")
          {
            Screen_Auto = 1;
            nextion.setComponentText("t1", "Auto ON");
          }

          if (command == "65 9 1 1 ffff ffff ffff")
          {
            break;
          }
        }
      }

      if (command == "65 1 2 1 ffff ffff ffff")
      {
        Serial.println("LED Settings .");

        while (1)
        {
          Serial.println("LED Settings While .");

          String command = nextion.listen();

          if (command == "65 2 7 1 ffff ffff ffff")
          {
            if (stat == 1)
            {
              for (int i = 24; i > -1; i--)
              {
                ring.setPixelColor(i, 0, 0, 0);
                ring.show();
                delay(50);
              }
              stat = 0;
              Auto = 0;
            }

            else
            {

            }
          }

          if (command == "65 2 2 1 ffff ffff ffff")
          {
            for (int i = 0; i < Neo_Pixel_Num; i++)
            {
              ring.setPixelColor(i, 255, 0, 0);
              ring.show();
              delay(50);
            }
            stat = 1;
            Auto = 0;
          }

          if (command == "65 2 3 1 ffff ffff ffff")
          {
            for (int i = 0; i < Neo_Pixel_Num; i++)
            {
              ring.setPixelColor(i, 0, 255, 0);
              ring.show();
              delay(50);
            }
            stat = 1;
            Auto = 0;
          }

          if (command == "65 2 4 1 ffff ffff ffff")
          {
            for (int i = 0; i < Neo_Pixel_Num; i++)
            {
              ring.setPixelColor(i, 0, 0, 255);
              ring.show();
              delay(50);
            }
            stat = 1;
            Auto = 0;
          }

          if (command == "65 2 5 1 ffff ffff ffff")
          {
            for (int i = 0; i < Neo_Pixel_Num; i++)
            {
              ring.setPixelColor(i, 255, 255, 255);
              ring.show();
              delay(50);
            }
            stat = 1;
            Auto = 0;
          }

          if (command == "65 2 6 1 ffff ffff ffff")
          {
            Auto = 1;
            stat = 1;
          }

          if (command == "65 2 1 1 ffff ffff ffff")
          {
            break;
          }
        }
      }

      if (command == "65 1 1 1 ffff ffff ffff")
      {
        break;
      }
    }
  }
  delay(400);
}
