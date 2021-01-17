

#include <Wire.h>              // I2C communication used for RealTimeClock
#include "RTClib.h"            // Realtime Clock
#include <SD.h>                // ReadWrite to SD Card

// setup RealTimeClock
RTC_DS1307 RTC;


DateTime m_t_stamp; 
String yyyy,mm,dd, date, filename;
void setup()
{
  Serial.begin(9600);
Wire.begin();   
// start the lib for RTC so we can get to the timestamp
//RTC.begin();

}
void loop()
{
  delay(1000);
  m_t_stamp = RTC.now();

 yyyy = String(m_t_stamp.year());
 mm = String(m_t_stamp.month());
 if (mm.length() < 2){mm = '0' + mm;}
 dd = String(m_t_stamp.day());
 if (dd.length() < 2){dd = '0' + dd;}
 date = yyyy + '/' + mm + '/'+ dd;
  filename = yyyy+mm+dd + ".csv";
  Serial.println(date);
  Serial.println(filename);
  delay(3000);
}

