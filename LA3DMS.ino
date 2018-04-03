//For Servo and controls
#include <Servo.h>
#include <LiquidCrystal.h>
//For Lidar
#include <SoftwareSerial.h>
#include "TFMini.h"

//Servo controls
Servo servoX;
Servo servoY;

//Lidar Controls
SoftwareSerial mySerial(10, 11);      // Uno RX (TFMINI TX), Uno TX (TFMINI RX)
TFMini tfmini;

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);



// Minimum and maximum servo angle in degrees
// Modify to avoid hitting limits imposed by pan/tilt bracket geometry
int minPosX = 0;
int maxPosX = 180;
int minPosY = 0;
int maxPosY = 90;

//int buttonPin = A0;
//int buttonValue = 0;
//int buttonThreshold = 50;
int lastPosX = 0;
int lastPosY = 0;
int loopCount = 0;
int radius = 0;
int lastRadius = 0;
//boolean selectButtonPressed = false;
boolean scanning = true;
boolean scanDirection = false;
int scanIncrement = 1;
int posX = (maxPosX + minPosX) / 2;
int posY = minPosY;
float pi = 3.14159265;
float deg2rad = pi / 180.0;




void setup() {
  // put your setup code here, to run once:
  
  // Step 1: Initialize hardware serial port (serial debug port)
  Serial.begin(115200);
  // wait for serial port to connect. Needed for native USB port only
  while (!Serial);
     
  Serial.println ("Initializing...");

  // Step 2: Initialize the data rate for the SoftwareSerial port
  mySerial.begin(TFMINI_BAUDRATE);

  // Step 3: Initialize the TF Mini sensor
  tfmini.begin(&mySerial);    

  //Initialize LCD
  lcd.begin(16, 2);
  
  
  //Setup Servo Pins
  servoX.attach(2);
  servoY.attach(3);
  servoX.write(posX);
  servoY.write(posY);
  
  //Serial Monitor setup
  //Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:

//  buttonValue = analogRead(buttonPin);

  if (scanning) {
/*    if (abs(buttonValue - 741) < buttonThreshold) {
      if (!selectButtonPressed) {
        // switch to manual scan mode
        selectButtonPressed = true;
        scanning = false;
        updateModeDisplay();
      }
    } else {
      selectButtonPressed = false;
    } */
    if (scanDirection) {
      posX += scanIncrement;
    } else {
      posX -= scanIncrement;
    }
    if (posX > maxPosX || posX < minPosX) {
      // hit limit X limit, reverse auto scan direction
      scanDirection = !scanDirection;
      posY += scanIncrement;
      if (posY > maxPosY) {
        // completed auto scan, return to manual scan mode
        scanning = false;
        updateModeDisplay();
      }
    }
  } else {

    Serial.print("Not scanning check scanninf bool");
    lcd.print("End Of Scan ");
    
/*    if (abs(buttonValue - 741) < buttonThreshold) {
      if (!selectButtonPressed) {
        // switch to auto scan mode
        selectButtonPressed = true;
        scanning = true;
        posX = minPosX;
        posY = minPosY;
        scanDirection = true;
        updateModeDisplay();
      }
    } else if (abs(buttonValue - 505) < buttonThreshold) {
      // manual scan left
      posX += 1;
    } else if (abs(buttonValue - 329) < buttonThreshold) {
      // manual scan down
      posY -= 1;
    } else if (abs(buttonValue - 145) < buttonThreshold) {
      // manual scan up
      posY += 1;
    } else if (abs(buttonValue - 0) < buttonThreshold) {
      // manual scan right
      posX -= 1;
    } else {
      selectButtonPressed = false;
    } */
  }

  posX = min(max(posX, minPosX), maxPosX);
  posY = min(max(posY, minPosY), maxPosY);
  bool moved = moveServos();
  displayPosition();

  loopCount += 1;
  if (loopCount % 100 == 0) {
    // recalibrate scanner every 100 scans
    radius = tfmini.getDistance();
  } 
  else {
    radius = tfmini.getDistance();
  } 
  if (abs(radius - lastRadius) > 2)
  {
    lastRadius = radius;
    lcd.setCursor(8, 0);
    lcd.print("D:" + String(radius / 100.0, 2) + "  ");
  }
  if (scanning || moved) {
    float azimuth = posX * deg2rad;
    float elevation = (180 - maxPosY + posY) * deg2rad;
    double x = radius * sin(elevation) * cos(azimuth);
    double y = radius * sin(elevation) * sin(azimuth);
    double z = radius * cos(elevation);
    Serial.println(String(x, 5) + " " + String(y, 5) + " " + String(-z, 5));
  }

  
}

//Functions------------
bool moveServos()
{
  bool moved = false;
  static int lastPosX;
  static int lastPosY;
  int delta = 0;  
  if (posX != lastPosX) {
    delta += abs(posX - lastPosX);
    servoX.write(posX);
    lastPosX = posX;
    moved = true;
  }
  if (posY != lastPosY) {
    delta += abs(posY - lastPosY);
    servoY.write(posY);
    lastPosY = posY;
    moved = true;
  }
  delay(30);
  return moved;
}

void displayPosition()
{
  static int lastPosX;
  static int lastPosY;
  if (posX != lastPosX) {
    lcd.setCursor(0, 0);
    lcd.print("X:" + String(posX) + "  ");
    lastPosX = posX;
  }
  if (posY != lastPosY) {
    lcd.setCursor(0, 1);
    lcd.print("Y:" + String(posY) + "  ");
    lastPosY = posY;
  }  
}

void updateModeDisplay()
{
  lcd.setCursor(8, 1);
  if (scanning) {
    lcd.print("Scanning");
  } else {
    lcd.print("Not Scanning");
  }
}

//------------------------------------ROUGH WORK----------------------------------------


//____________________________________________________________________


