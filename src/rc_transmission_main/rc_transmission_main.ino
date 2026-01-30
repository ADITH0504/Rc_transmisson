#include <ps5.h>
#include <ps5Controller.h>
#include <ps5_int.h>


const int yPin   = 13; // JOYSTICK
const int xPin   = 35;

const int  wiper = 34; // pot and switch 
const int Switch = 32; 

const int ENA = 14; // Drive Motor A 
const int IN1 = 26;
const int IN2 = 27;

const int IN3 = 12;// Drive Motor B
const int IN4 = 4;
const int ENB = 25;

const int led = 2; // ESP LED

const int pwmFreq = 2000;
const int pwmResolution = 8; // BIT size 

const int center = 128;
const int deadZoneS = 8;

int currentPos = 0;

const int centerMin = 100;
const int centerMax = 200;

const int leftLim = 50;
const int rightLim = 220;

int value = 0;
int value2 = 0;

int lastButtonState = HIGH; // STATE VARIABLES
int ledState = LOW;

void setup() {
  
 Serial.begin(9600);
 
 pinMode(wiper, INPUT);
 pinMode(IN3, OUTPUT);
 pinMode(IN4, OUTPUT);
 pinMode(xPin, INPUT);


 pinMode( IN1, OUTPUT);
 pinMode( IN2, OUTPUT);
 pinMode( Switch, INPUT_PULLUP);
 pinMode(led, OUTPUT);
 
 ledcAttach(ENA, pwmFreq, pwmResolution);
 
 ledcAttach(ENB, pwmFreq, pwmResolution);
 
 ps5.begin("24:A6:FA:E0:E6:0A");

}


void loop() 

{
  if (!ps5.isConnected())
  {
    delay(200);
    stopMotor();
    stopDrive();
    return;
  }
  else {
    driveFunction();
    steerFunction();
  }
 
delay(10);

}


void steerRight()
{
  int now = steerReading();

     if ( now >= rightLim)
     {
        stopMotor();
        return;
     }else{ moveRight(150); }
     
}
void steerLeft()
{

  int now = steerReading();

     if ( now <= leftLim)
     {
      stopMotor();
      return;
     }else {  moveLeft(150); }
   
 
}

void steerStop()
{
  int now = steerReading();

   
  if ( now > centerMin && now < centerMax )
   {
    stopMotor();
     return;
   }
   else if ( now < 50) {
    
    moveRight(100);
    
   }else if ( now > 200)
   {
    moveLeft(100);
   }
  
      
}

void steerFunction()
{
  int xValue = ps5.LStickX();
  xValue = constrain(xValue, -128, 128);
  int target = map(xValue, -128 , 128, 0, 255);
   target = constrain(target, 0 , 255);
      Serial.print(" -|Target: ");
        Serial.println(target);
        
if ( target >= center + deadZoneS)
{
   steerRight();
 
}else if ( target <= center - deadZoneS)
{
  steerLeft();
}else if (abs(target - center) <= deadZoneS)
{
 steerStop();
}

}

int steerReading()
{
    int pot = analogRead(wiper); 
     pot =  constrain(pot, 1296, 2267);
      currentPos = map(pot, 1296 , 2267, 0 , 255);  
  int pos = constrain(currentPos, 0 , 255);
     Serial.print(" -|CurrentPos: ");
        Serial.println(pos);
        return pos;
}

void moveRight(int pwm)
{
  digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
     ledcWrite(ENB, pwm);
}
void moveLeft(int pwm)
{
   digitalWrite(IN3,HIGH);
    digitalWrite(IN4, LOW);
     ledcWrite(ENB, pwm);
}

 void stopMotor()
 {
      digitalWrite(IN3, LOW);
       digitalWrite(IN4, LOW);
        ledcWrite(ENB, 0);
 }
         
void forward(int drivePwm)
{
  digitalWrite(IN1, HIGH); // Forward
    digitalWrite(IN2, LOW);
     ledcWrite(ENA, drivePwm);
}

void backward(int drivePwm)
{
   digitalWrite(IN1, LOW); // Backward
    digitalWrite(IN2, HIGH);
    ledcWrite(ENA, drivePwm);
}

void stopDrive()
{
   digitalWrite(IN1, LOW); 
     digitalWrite(IN2, LOW);
       ledcWrite(ENA, 0);
}


void driveFunction()
{
          int moveB = ps5.R2Value();
          int moveF = ps5.L2Value();
          int deadZone = 8;
          int pwm = 0;

          
  if ( moveF >  deadZone) // FORWARD
  { 
     forward(moveF);
     if (  moveB > deadZone)
  {
    stopDrive();
  }
    
  }else if (moveB > deadZone) // BACKWARD
  {
     backward(moveB);
     if (  moveF > deadZone)
  {
    stopDrive();
  }
    
  }else if ( moveF <= deadZone || moveB <= deadZone )// ABS HELP TO GIVE THE OUTPUT VALUE AS POSITIVE IF THE VALUE IS NEGATIVE, THAT S IT,,
  {
     stopDrive();
  }  

}

void startStop()
{
   int currentButtonState = digitalRead(Switch);
 
   if ( currentButtonState == LOW && lastButtonState == HIGH )
   {
    delay(25); // TO avoid noise
    currentButtonState = digitalRead(Switch);
    
    if ( currentButtonState == LOW)
    {
      ledState = !ledState; 
      digitalWrite(led, ledState);
    }
   }
   lastButtonState = currentButtonState;

  
} 
