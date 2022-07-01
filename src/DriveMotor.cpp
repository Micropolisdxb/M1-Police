#include <Arduino.h>
#include <DriveMotor.h>

float getSpeed(int SPDMTR_PIN);
unsigned long Drive_PrevMillis = 0;

void Drive_Init()
{
  pinMode(Rev_SW, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(Throttle_Pin, OUTPUT);
  pinMode(Enable_Power_SW, OUTPUT);

  pinMode(Speedometer_Pin, INPUT);

  digitalWrite(Enable_Power_SW, LOW); // Initially OFF, No enable for drive motor
}

void Drive_APP(int Throttle_Val, String Direction_Mode)
{
  int Throttle_PWM = map(Throttle_Val, 0, 100, Drive_Min_Speed_PWM, Drive_MAX_Speed_PWM);

  if (Direction_Mode == "r" && getSpeed(Speedometer_Pin) == 0)
  {
    digitalWrite(Rev_SW, HIGH);
  }
  else if (Direction_Mode == "d" && getSpeed(Speedometer_Pin) == 0)
  {
    digitalWrite(Rev_SW, LOW);
  }

  if (Throttle_PWM > 0)
  {
    analogWrite(Throttle_Pin, Throttle_PWM);

    digitalWrite(Enable_Power_SW, HIGH);
  }

  else if (Throttle_PWM == 0)
  {
    analogWrite(Throttle_Pin, 0);

    digitalWrite(Enable_Power_SW, LOW);
  }
}

void Drive_RC(int Throttle_Val)
{

  if (Throttle_Val > (Drive_RC_Zero + 2))
  {
    digitalWrite(Enable_Power_SW, HIGH);

    digitalWrite(Rev_SW, HIGH);
    int Throttle_PWM = map(Throttle_Val, Drive_RC_Zero, Drive_RC_Max, Drive_Min_Speed_PWM, Drive_MAX_Speed_PWM);

    analogWrite(Throttle_Pin, Throttle_PWM);
  }

  else if (Throttle_Val < (Drive_RC_Zero - 2))
  {
    digitalWrite(Enable_Power_SW, HIGH);
    digitalWrite(Rev_SW, LOW);
    int Throttle_PWM = map(Throttle_Val, Drive_RC_Min, Drive_RC_Zero, Drive_MAX_Speed_PWM, Drive_Min_Speed_PWM);

    analogWrite(Throttle_Pin, Throttle_PWM);
  }

  else if (Throttle_Val == Drive_RC_Zero)
  {
    digitalWrite(Rev_SW, digitalRead(Rev_SW));
    analogWrite(Throttle_Pin, 0);
    digitalWrite(Enable_Power_SW, LOW);
  }
}
void Drive_Serial_Control()
{
  char SerialChar = ' ';
  int max_speed = 150;
  short wait = 10;

  if (Serial.available() > 0)
  {
    SerialChar = Serial.read();
    Serial.println("-------------");
    Serial.printf("Serial Data: %c \n", SerialChar);
  }

  if (millis() - Drive_PrevMillis > 100)
  {
    float Speed = getSpeed(Speedometer_Pin);
    Serial.printf("Speed: %f  \n", Speed);
    Drive_PrevMillis = millis();
  }

  if (SerialChar == '1')
  {
    digitalWrite(Enable_Power_SW, HIGH);

    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(Rev_SW, LOW);

    for (int i = 40; i < max_speed; i++)
    {
      analogWrite(Throttle_Pin, i);
      Serial.printf("Throttle: %d \n", i);
      delay(wait);
    }

    Serial.println("Forward");
  }

  else if (SerialChar == '2')
  {
    digitalWrite(Enable_Power_SW, HIGH);

    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(Rev_SW, HIGH);
    for (int i = 40; i < max_speed; i++)
    {
      analogWrite(Throttle_Pin, i);
      Serial.printf("Throttle: %d \n", i);
      delay(wait);
    }

    Serial.println("Backward");
  }

  else if (SerialChar == '3')
  {
    digitalWrite(Enable_Power_SW, LOW);
    digitalWrite(Rev_SW, digitalRead(Rev_SW));
    for (int i = max_speed; i >= 0; i--)
    {
      analogWrite(Throttle_Pin, i);
      Serial.printf("Brake: %d \n", i);
      delay(1);
    }
    Serial.println("Brake");
  }
}

float getSpeed(int SPDMTR_PIN)
{
  // timeout after 10 ms
  unsigned long pulseDuration = pulseIn(SPDMTR_PIN, HIGH, 10000); // return duration of HIGH pulses from speedMeter wire (from hall sensors)

  // exclude 0 or 1 readings (noise)
  Serial.print("Duration: ");
  Serial.println(pulseDuration);
  if (pulseDuration > 1)
  {
    float rps, rpm;
    rps = 125500.0 / (float)pulseDuration; // 125276
    // Serial.print("rps: ");
    // Serial.println(rps);
    rpm = rps * 60;
    // ang_vel = 2.0 * PI / 60.0 * rpm;
    // lin_vel = ang_vel * WHEEL_RADIUS;
    // Serial.print("rpm: ");
    // Serial.printf("%d, %f\n", (int)rpm, rpm);
    // Serial.print("omega: ");
    // Serial.println(ang_vel);
    // Serial.print("speed: ");
    // Serial.print(lin_vel / 1000);
    // Serial.println("Km/h");
    // int rpm_int = rpm;
    return (float)rpm;
  }
  return 0; // no value returned
}