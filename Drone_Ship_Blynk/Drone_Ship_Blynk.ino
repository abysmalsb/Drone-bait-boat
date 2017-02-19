#include <ESP8266WiFi.h>
#include <Servo.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>

#define VREF 1.0
#define R1 9000
#define R2 1000
#define RESOLUTION 1024

#define SERVO_CARGO_PIN 4   //you have to use some other GPIO if you use Sparkfun Blynk Board
#define SERVO_RUDDER_PIN 5
#define MOTOR_PIN_1 12
#define MOTOR_PIN_2 13

#define ADC_PIN A0

//This equation is based on the voltage divider and adc value to voltage equations
//If I multiply the ADC's value with convertADCReadToVoltage, I get the battery's voltage
float convertADCReadToVoltage = (VREF * (R2 + R1)) / (R2 * RESOLUTION);

SimpleTimer timer;

Servo servoCargo;
Servo servoRudder;

char auth[] = "your auth";
char ssid[] = "your ssid";
char password[] = "your password";

//updating the battery status in the Blynk app
void batteryVoltage()
{
  float voltage = convertADCReadToVoltage * analogRead(ADC_PIN);
  Blynk.virtualWrite(V5, voltage);
}

void setup()
{
  //Serial.begin(9600);
  Blynk.begin(auth, ssid, password);
  
  servoCargo.attach(SERVO_CARGO_PIN);
  servoRudder.attach(SERVO_RUDDER_PIN);

  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);

  servoCargo.write(128);
  
  timer.setInterval(1000L, batteryVoltage);
}

//Setting up the angle of the rudder It receives the angle from the Blynk app's joystick (horizontal)
BLYNK_WRITE(V1)
{
  servoRudder.write(param.asInt());
  Serial.println("V1");
}

//Setting the speed of the motor based on the position of the Blynk app's joystick (vertical)
BLYNK_WRITE(V2)
{
  int motorSpeed = param.asInt();

  //by default the received motorSpeed is 0. If you move to joystick up or down, you can modify the speed of the boat
  if(motorSpeed > 0){
    analogWrite(MOTOR_PIN_1, motorSpeed);
    analogWrite(MOTOR_PIN_2, 0);
  }
  else{
    analogWrite(MOTOR_PIN_1, 0);
    analogWrite(MOTOR_PIN_2, abs(motorSpeed));
  }
}

//It opens the cargo door with the servo, then returns the servo arm to the original position
BLYNK_WRITE(V3)
{
  servoCargo.write(30);
  
  delay(1000);
  
  servoCargo.write(128);
}

void loop()
{
  Blynk.run();
  timer.run();
}

