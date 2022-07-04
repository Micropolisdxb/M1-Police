/********************************************************************************/
/*   Author  : Ahmed Farag  & Wajih                                             */
/*   Date    : 18/05/2022                                                       */
/*   Version : V02                                                              */
/********************************************************************************/
#include <Arduino.h>
#include <KELLY_Configuration.h>
#include "SBUS.h"

// Steering Intitialization function
void Steering_Init()
{
  // Set the PWM pin for the throttle Front Steering to output
  pinMode(THROTTLE_FORWARD_PIN, OUTPUT);
  // Set the PWM pin for the throttle Back Steering to output
  pinMode(THROTTLE_REVERSE_PIN, OUTPUT);
  // Set the Front switch pin  to output
  pinMode(FORWARD_DIRECTION_SWITCH, OUTPUT);
  // Set the back switch pin  to output
  pinMode(REVERSE_DIRECTION_SWITCH, OUTPUT);
  // initialize Reverse pin to low
  digitalWrite(FORWARD_DIRECTION_SWITCH, LOW);
  // initialize Reverse pin to low
  digitalWrite(REVERSE_DIRECTION_SWITCH, LOW);
}

// Stering function without feedback
void Steering_RC(int RC_Readings, int Steering_SW)
{
// Serial.println(Steering_SW);
  if ((RC_Readings >= (STEERING_RC_Zero + 2)) && (RC_Readings <= (STEERING_RC_Max)))
  {
    if (Steering_SW == 282)
    {
      Kelly_Front_Steering(RC_Readings, THROTTLE_FORWARD_PIN, STEERING_RC_Zero, STEERING_RC_Max, THROTTLE_MIN_PWM, THROTTLE_MAX_PWM, FORWARD_DIRECTION_SWITCH, HIGH);
    }
    else if (Steering_SW == 1722)
    {
      Kelly_Back_Steering(RC_Readings, THROTTLE_REVERSE_PIN, STEERING_RC_Zero, STEERING_RC_Max, THROTTLE_MIN_PWM_REVERSE, THROTTLE_MAX_PWM_REVERSE, REVERSE_DIRECTION_SWITCH, LOW);
    }
    else 
    {
      Kelly_Front_Steering(RC_Readings, THROTTLE_FORWARD_PIN, STEERING_RC_Zero, STEERING_RC_Max, THROTTLE_MIN_PWM, THROTTLE_MAX_PWM, FORWARD_DIRECTION_SWITCH, HIGH);
      Kelly_Back_Steering(RC_Readings, THROTTLE_REVERSE_PIN, STEERING_RC_Zero, STEERING_RC_Max, THROTTLE_MIN_PWM_REVERSE, THROTTLE_MAX_PWM_REVERSE, REVERSE_DIRECTION_SWITCH, LOW);
    }
  }

  else if ((RC_Readings > (STEERING_RC_Zero - 2)) && (RC_Readings < (STEERING_RC_Zero + 2)))
  {
    Serial.println("Zero");
    analogWrite(THROTTLE_FORWARD_PIN, 1);
    analogWrite(THROTTLE_REVERSE_PIN, 1);

    digitalWrite(FORWARD_DIRECTION_SWITCH, LOW);
    digitalWrite(REVERSE_DIRECTION_SWITCH, LOW);
  }

  else if ((RC_Readings >= STEERING_RC_Min) && (RC_Readings <= (STEERING_RC_Zero - 2)))
  {
    Serial.println("Left: ");
    if (Steering_SW == 282)
    {
      Kelly_Front_Steering(RC_Readings, THROTTLE_FORWARD_PIN, STEERING_RC_Zero, STEERING_RC_Max, THROTTLE_MIN_PWM, THROTTLE_MAX_PWM, FORWARD_DIRECTION_SWITCH, LOW);
    }
    else if (Steering_SW == 1722)
    {
      Kelly_Back_Steering(RC_Readings, THROTTLE_REVERSE_PIN, STEERING_RC_Zero, STEERING_RC_Max, THROTTLE_MIN_PWM_REVERSE, THROTTLE_MAX_PWM_REVERSE, REVERSE_DIRECTION_SWITCH, HIGH);
    }
    else 
    {

      Kelly_Front_Steering(RC_Readings, THROTTLE_FORWARD_PIN, STEERING_RC_Zero, STEERING_RC_Max, THROTTLE_MIN_PWM, THROTTLE_MAX_PWM, FORWARD_DIRECTION_SWITCH, LOW);
      Kelly_Back_Steering(RC_Readings, THROTTLE_REVERSE_PIN, STEERING_RC_Zero, STEERING_RC_Max, THROTTLE_MIN_PWM_REVERSE, THROTTLE_MAX_PWM_REVERSE, REVERSE_DIRECTION_SWITCH, HIGH);
    }
  }
}

// Function to control the front steering motor
void Kelly_Front_Steering(int RC_Readings, int Throttle_Pin_num, int Throttle_Min_Readings, int Throttle_Max_Readings, int Throttle_Min_PWM, int Throttle_Max_PWM, int Front_Switch_pin, int Switch_State)
{
  int Throttle_PWM_Value = map(RC_Readings, Throttle_Min_Readings, Throttle_Max_Readings, Throttle_Min_PWM, Throttle_Max_PWM);
  digitalWrite(Front_Switch_pin, Switch_State);
  analogWrite(2, Throttle_PWM_Value);
}

// Function to control the rear steering motor
void Kelly_Back_Steering(int RC_Readings, int Throttle_Pin_num, int Throttle_Min_Readings, int Throttle_Max_Readings, int Throttle_Min_PWM, int Throttle_Max_PWM, int Back_Switch_Pin, int Switch_State)
{
  int Throttle_PWM_Value_Rev = map(RC_Readings, Throttle_Min_Readings, Throttle_Max_Readings, Throttle_Min_PWM, Throttle_Max_PWM);
  digitalWrite(Back_Switch_Pin, Switch_State);
  analogWrite(3, Throttle_PWM_Value_Rev / 3);
}
