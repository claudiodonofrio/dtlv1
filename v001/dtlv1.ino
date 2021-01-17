/*
This sketch runs a Standalone Datalooger for two general purpose inputs
and two Thermo Couple inputs. Pleae be aware, you MUST use K-Type Thermocouples.
specification for input can be found at: http://www.maximintegrated.com/datasheet/index.mvp/id/7273
(max31855 Cold-Junction Compensated Thermocouple-to-Digital Converter)
The breakoutboard for max31855 and specs are found at http://www.adafruit.com/products/269
The logfiles are written to an SD Card and and a RealTimeClock is setup.
For schematic, libraries etc please visit http://www.ladyada.net/make/logshield/design.html


PinLayout:
A0
A1
A2 = GPIP    / General Inpu 0-5V Channel C
A3 = GPIP    / General Inpu 0-5V Channel D
A4 = I2C bus / RTC
A5 = I2C bus / RTC

DIO 0  = 
DIO 1  = 
DIO 2  = LED   / Green Indicator LED
DIO 3  = LED   / Red Indicator LED
DIO 4  = CLK    / Thermocouple Channel  \
DIO 5  = CS    / Thermocouple Channel   > A
DIO 6  = DO   / Thermocouple Channel  /
DIO 7  = CLK    / Thermocouple Channel  \
DIO 8  = CS    / Thermocouple Channel   > B
DIO 9  = DO   / Thermocouple Channel /
DIO 10 = CS    / sd card
DIO 11 = MOSI  / sd card
DIO 12 = MISO  / sd card
DIO 13 = SCK   / sd card
*/

#include <Wire.h>              // I2C communication used for RealTimeClock
#include "RTClib.h"            // Realtime Clock
#include <SD.h>                // ReadWrite to SD Card
#include "Adafruit_MAX31855.h" // readout data from MAX31855 

// setup RealTimeClock
RTC_DS1307 RTC;

// setup var's
DateTime m_t_stamp; 
String m_date;
String m_time;
String m_data;
const int m_led_r = 2;
const int m_led_g = 3;
const int m_sdcard_CS = 10;
const int m_ch_A_CLK = 4;
const int m_ch_A_CS = 5;
const int m_ch_A_DO = 6;
const int m_ch_B_CLK = 7;
const int m_ch_B_CS = 8;
const int m_ch_B_DO = 9;
const int m_ch_C = A2;
const int m_ch_D = A3;

double m_ch_A, m_ch_B;
int m_A2, m_A3;

int m_timer;           // delay of main loop to slow down the arduino // -> cfg file
File m_logfile;
char m_file_name[12];

Adafruit_MAX31855 ch_A(m_ch_A_CLK,m_ch_A_CS,m_ch_A_DO);
Adafruit_MAX31855 ch_B(m_ch_B_CLK,m_ch_B_CS,m_ch_B_DO);

void setup()
{
  pinMode(m_led_r, OUTPUT);
  digitalWrite(m_led_r, LOW);
  pinMode(m_led_g, OUTPUT);
  digitalWrite(m_led_g, LOW);
  pinMode(m_ch_A_CS, OUTPUT);
  pinMode(m_ch_B_CS, OUTPUT);
  pinMode(m_sdcard_CS, OUTPUT);
  
  /* 
  set default timer to one second, since the RTC provides seconds as smallest unit
  however the reading of the sensor can be done faster and hence this timer may be
  set lower to capture more readings per seconds. The logfile entries will have the 
  same time stamp
  */ 
  m_timer = 2000; 
  
  Serial.begin(9600);
  // start communication for RTC (using default values of Analog 4 and Analog 5
  Wire.begin();   
  // start the lib for RTC so we can get to the timestamp
  RTC.begin();   
  // check if the realtime clock is running
  if (! RTC.isrunning() ) 
  {
    blink(m_led_r, 2, 400);   
  }
  if (!SD.begin(m_sdcard_CS))
  {
    blink(m_led_r, 5, 400); 
    return;
  }
  blink(m_led_g, 2, 200);
  Serial.println("setup done");
}

void loop()
{
  getTimeStamp();   // set m_date, m_time, m_logfilename
  debug();
  // get temp reading for the thermocouples (channel A+B) 
  m_ch_A = ch_A.readCelsius();
  if (isnan(m_ch_A))
  {
    m_data = m_data + ',cha_NaN';
  }else{
    m_data = m_data + ',cha' ;
    //+ floatToString(ch_A.readCelsius(),2);
  }
   debug();
  m_ch_B = ch_B.readCelsius();
  if (isnan(m_ch_B))
  {
    m_data = m_data + ',chb_NaN';
  }else{
    m_data += ',chb';
   // + floatToString(ch_B.readCelsius(),2);
  }
  debug();
  // get the GPIO input for AnalogSignal 2&3
  m_data  = m_data + ',' + analogRead(m_ch_C);
  m_data  = m_data + ',' + analogRead(m_ch_D);
  

  
  m_logfile = SD.open(m_file_name, FILE_WRITE);
  if (m_logfile)
  {
    
    m_logfile.println(m_data);
    m_logfile.close();
    blink(m_led_g,1,500);
  } else {
    blink(m_led_r,2,300);
  }
  Serial.println("-----------------");
  delay(m_timer);  
}

void debug(){  Serial.println(m_data);}


void getTimeStamp()
{  
  m_t_stamp = RTC.now();
  // set the filename to write the data to (change the name for every day)
  String name = String(m_t_stamp.year()) + String(m_t_stamp.month()) + String(m_t_stamp.day()) + '.csv';
  name.toCharArray(m_file_name,13);
  
  m_date = String(m_t_stamp.year()) + '/';
  m_date = m_date + String(m_t_stamp.month()) + '/'; 
  m_date = m_date + String(m_t_stamp.day()); 
  
  // start of assembly of data line
  m_data = m_date;
  
  m_time = String(m_t_stamp.hour()) + ':';
  m_time = m_time + String(m_t_stamp.minute()) + ':';
  m_time = m_time + String(m_t_stamp.second());  
  m_data = m_data + "," + m_time;
}
void blink(int led, int freq, int velocity)
{
  for (int i=0; i<freq; i++)
  {
    digitalWrite(led, HIGH);
    delay(velocity);
    digitalWrite(led, LOW);
    delay(velocity);
  }
}

String floatToString(double number, uint8_t digits) 
{ 
  String resultString = "";
  // Handle negative numbers
  if (number < 0.0)
  {
     resultString += "-";
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  resultString += int_part;

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    resultString += "."; 

  // Extract digits from the remainder one at a time
  while (digits-- > 0)
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    resultString += toPrint;
    remainder -= toPrint; 
  } 
  return resultString;
}
