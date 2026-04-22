#include <IRremote.h>

//#include <Wire.h>

#define LED_LEFT 12
#define LED_RIGHT 13

#define PS_LEFT A1
#define PS_RIGHT A0
#define PS_MAX 1017
#define PS_MIN 350
#define PS_SENS 10

#define PIN_TRIG 8
#define PIN_ECHO 9
#define DIST_MIN 10
//левый мотор
#define ENAL 2
#define IN3 4
#define IN4 3
//правый мотор
#define ENAR 7
#define IN1 6
#define IN2 5

#define SPEED 255

#define IR 11
#define IR_PWR 0xFF00BF00

IRrecv irrecv(IR); // указываем вывод, к которому подключен приемник
bool isRunning = true;

void setup()
{
 
  //диоды
  pinMode(LED_LEFT, OUTPUT);
  pinMode(LED_RIGHT, OUTPUT);
  //консоль вывода
  Serial.begin(9600);
  //дальномер
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  //левый мотор
  pinMode(ENAL, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  //правый мотор
  pinMode(ENAR, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  
  irrecv.enableIRIn();
}

double getDistance()
{
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  return pulseIn(PIN_ECHO, HIGH)/58.2;;
}

void stop()
{
  digitalWrite(LED_LEFT, LOW);
  digitalWrite(LED_RIGHT, LOW);
  
  analogWrite(ENAL, 0);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENAR, 0);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void moveLeft()
{
  digitalWrite(LED_LEFT, HIGH);
  digitalWrite(LED_RIGHT, LOW);
  
  analogWrite(ENAL, 0);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENAR, SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void moveRight()
{
  digitalWrite(LED_LEFT, LOW);
  digitalWrite(LED_RIGHT, HIGH);
  
  analogWrite(ENAL, SPEED);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENAR, 0);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void moveForward()
{
  digitalWrite(LED_LEFT, HIGH);
  digitalWrite(LED_RIGHT, HIGH);
  
  analogWrite(ENAL, SPEED);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENAR, SPEED);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void loop()
{ 
  if (irrecv.decode()) 
  { // если данные пришли
    //Serial.println(irrecv.decodedIRData.decodedRawData, HEX); // печатаем данные
 	if (irrecv.decodedIRData.decodedRawData == IR_PWR) 
    {
    	isRunning = !isRunning;	
      	Serial.print("power ");
      	Serial.println(isRunning);
    }  
    irrecv.resume(); // принимаем следующую команду
  }

  
  if (!isRunning)
  {
    stop();
    return;
  }
  
  double distance = getDistance();
  Serial.print("distance: ");
  Serial.println(distance);
  
  if (distance < DIST_MIN)
  {
    stop();
    delay(100);
    return;
  }
 
  int left = analogRead(PS_LEFT);
  int right = analogRead(PS_RIGHT);
  Serial.print("left: ");
  Serial.println(left);
  Serial.print("right: ");
  Serial.println(right);

  if (left < PS_MIN)
  	left = PS_MIN;  
  if (left > PS_MAX)
    left = PS_MAX;
  
  if (right < PS_MIN)
  	right = PS_MIN;
  if (right > PS_MAX)
    right = PS_MAX;
  
  if ((right - left) >= PS_SENS) 
    moveLeft();
  else if ((left - right) >= PS_SENS) 
    moveRight();
  else if (right < PS_MAX)
    moveForward();
  else 
    stop();
  
  delay(100);
}