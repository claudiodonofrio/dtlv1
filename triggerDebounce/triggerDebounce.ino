// upload to github now
// --------------------

#include "Wire.h"

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "I2Cdev.h"
#include "MPU6050.h"

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro(0x69);
int16_t ax, ay, az;
int16_t gx, gy, gz;


#define logTrigger 3         // this is the pin where the external push button is installed to start
volatile unsigned long triggerDebounce;      // store the log start time to debounce the manual trigger


#define LED_Activity 6      // use this led for activity, marked on board as L1
#define LED_Error 5         // use this led for errors, marked on board as L2

bool logging = false;


void setup() {
  Wire.begin();
  Serial.begin(57600);
  
  attachInterrupt(digitalPinToInterrupt(logTrigger), toggleLog, FALLING);
    triggerDebounce = millis(); // use this to debounce..

    //setup the logging trigger and push button
    pinMode(logTrigger, INPUT);
    digitalWrite(logTrigger, HIGH);

    pinMode(LED_Activity, OUTPUT);       //
  digitalWrite(LED_Activity, LOW);     //  set led's and and make sure
  pinMode(LED_Error, OUTPUT);          //  they are turned off
  digitalWrite(LED_Error, LOW);        //


Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  

}

void loop() {
  // put your main code here, to run repeatedly:
  // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);

    // display tab-separated accel/gyro x/y/z values
    Serial.print("a/g:\t");
    Serial.print(ax); Serial.print("\t");
    Serial.print(ay); Serial.print("\t");
    Serial.print(az); Serial.print("\t");
    Serial.print(gx); Serial.print("\t");
    Serial.print(gy); Serial.print("\t");
    Serial.println(gz);
    Serial.println("...............................");
    Serial.print("logging: ");
    Serial.println(logging);
    Serial.println("...............................");
  delay(500);
}

//------------------------------------------------------------------------------
// this is triggered either by interrupt 1 (external manual trigger on pin 3)
// or the serial command "l" -> start/stop logging
void toggleLog()
{
  if (millis() - triggerDebounce > 150)
  {
    logging = !logging;
    if (logging == true) {

      // turn on the led to show logging has started
      blink(3);
    }
    triggerDebounce = millis();
  }
}


//------------------------------------------------------------------------------
void blink(int count) {
  for (int i=0;i<count;i++){
    digitalWrite(LED_Activity, HIGH);
    delay(500);
    digitalWrite(LED_Activity, LOW);
    delay(500);    
  }
}
