//LIBRARIES 

#include <Wire.h> //I2C communication enable karta hai LCD se baat karne ke liye
#include <LiquidCrystal_I2C.h> //LCD control library
#include <CheapStepper.h> //Stepper motor control
#include <Servo.h> //Servo motor control

// OBJECT CREATIONS 

LiquidCrystal_I2C lcd(0x27, 16, 2); //LCD object bana Address = 0x27 Size = 16×2


Servo servo1; //Servo motor name = servo1

//PIN DEFINITIONS 

#define ir 5 //IR sensor → pin 5
#define proxi 6 //Metal proximity sensor → pin 6
#define buzzer 12 //Buzzer → pin 12

//MOISTURE SENSOR VARIABLES


int potPin = A0; //Moisture sensor analog input pin
int soil=0; // Raw moisture value store
int fsoil; //Final moisture percentage
int servoAng=95; //Servo resting angle

//STEPPER OBJECT

CheapStepper stepper (11,10,9,8); //Stepper motor pins connected

//SETUP FUNCTION
//Runs only once when power ON

void setup()
{Serial.begin(9600); //Communication with laptop

  lcd.init(); //LCD ON
  lcd.backlight(); //Light ON
  lcd.print("SYSTEM READY"); //initial message 
  
  //PIN MODES

  pinMode(proxi, INPUT_PULLUP); //Metal sensor input with pullup //“INPUT_PULLUP enables internal pull-up resistor, ensuring stable readings and avoiding floating input when sensor output is not active.”
  pinMode(ir, INPUT); //IR sensor input
  pinMode(buzzer, OUTPUT); //Buzzer output
  servo1.attach(7); //Servo connected to pin 7
  stepper.setRpm(17); //Stepper speed set
 
  //SERVO INITIAL POSITION
 
  servo1.write(180);
delay(2000);
servo1.write(servoAng);
delay(1000);
  //Gate open → close System ready state

}

//LOOP FUNCTION START

void loop() 
{
  fsoil=0;//Reset moisture value
  int L =digitalRead(proxi); //Read metal sensor
Serial.print(L); //Print reading on serial monitor
if(L==0) //IF METAL DETECTED
{
  lcd.clear();
  lcd.print("METAL DETECTED"); //LCD message
  tone(buzzer, 1000, 1000); //Buzzer
 stepper.moveDegreesCCW (240); //Rotate to metal bin
delay(1000); 
servo1.write(180); //Open gate
delay(2000);
servo1.write(servoAng); //Close gate
delay(1000);
stepper.moveDegreesCW (240); //Return to original position
lcd.clear();
lcd.print("SYSTEM READY"); //Show ready message again
delay(1000); 
}

//IR SENSOR CHECK

if(digitalRead(ir)==0) //Object detected
{
  lcd.clear();
  lcd.print("SCANNING....."); //Show scanning
   tone(buzzer, 1000, 500); //Buzzer
   delay(1000);
  int soil=0;

  //MOISTURE MEASUREMENT LOOP

  for(int i=0;i<3;i++) //Read moisture 3 times
    {
    soil = analogRead(potPin) ; //Read sensor
      soil = constrain(soil, 485, 1023); //Limit range
        fsoil = (map(soil, 485, 1023, 100, 0))+fsoil; //Convert to %
          delay(75);
      }
    fsoil=fsoil/3; //Average
    Serial.print(fsoil);
    Serial.print("%");Serial.print("\n");

    if(fsoil>20) //IF WET
           {
            lcd.clear();
            lcd.print("WET WASTE"); //Show wet message
            lcd.setCursor(0,1);
            lcd.print("Moisture: "); lcd.print(fsoil); lcd.print("%");
            stepper.moveDegreesCW (240); //Rotate to wet bin
            delay(1000); 
              servo1.write(180); //Open gate 
              delay(2000);
              servo1.write(servoAng); // Close gate 
                delay(1000);
              stepper.moveDegreesCCW (240); //Return to Original Position 
                delay(1000); 
              } 

      else {
        lcd.clear();
        lcd.print("DRY WASTE"); //ELSE → DRY Show dry message
        lcd.setCursor(0,1);
        lcd.print("Moisture: ");  lcd.print(fsoil); lcd.print("%"); 
         tone(buzzer, 1000, 500);
           delay(1000);
            servo1.write(180); //Open gate 
              delay(2000);
              servo1.write(servoAng); // Close gate 
                delay(1000); 
                lcd.clear();
                lcd.print("SYSTEM READY");
                }
}

}