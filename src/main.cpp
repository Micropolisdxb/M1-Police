#include <Arduino.h>
#include <ROS_Serial.h>
#include <DriveMotor.h>
#include <Brake.h>
#include <Light_Siren.h>
#include <Drone_Launch.h>
#include <KELLY_Configuration.h>
#include "SBUS.h"

#define Steering_RC_CH 0
#define Throttle_RC_CH 2
#define Drone_CH 4
#define Front_Light_CH 5
#define Siren12_CH 6
#define Steering_Control_CH 7
#define APP_Enable_CH 8
#define Emergency_Brake_CH 9
#define Siren3_CH 10
#define Brake_Release_CH 11
/**
 * Drone Launch
 *
 */
// serial port 1
SBUS x8r(Serial1);

// channel, fail safe, and lost frames data
uint16_t channels[16];
bool failSafe = HIGH;
bool lostFrame;

void setup()
{
  Serial.begin(115200);
  ROS_Init();
  Drive_Init();
  Brake_init();
  Steering_Init();

  Light_Init();
  Siren_Init();
  Drone_Launch_Init();

  x8r.begin();
}
#define APP_Enable 1722
#define APP_Disable 282

void loop()
{
  if (x8r.read(&channels[0], &failSafe, &lostFrame))
    ;

  int Throttle_RC = channels[Throttle_RC_CH];
  int Steering_Reading = channels[Steering_RC_CH];
  int Drone_RC = channels[Drone_CH];
  int Front_Light_RC = channels[Front_Light_CH];
  int Steering_Control = channels[Steering_Control_CH];
  int APP_Enable_RC = channels[APP_Enable_CH];
  int EM_Brake = channels[Emergency_Brake_CH];
  int Release_Brake = channels[Brake_Release_CH];
  int Siren_12_RC = channels[Siren12_CH];
  int Siren_3_RC = channels[Siren3_CH];
  // Serial.printf("Siren_12_RC: %d  \n", Siren_12_RC);
  // Serial.printf("Siren_3_RC: %d  \n", Siren_3_RC);

  // Serial.println(failSafe);
  if (!failSafe)
  {
    if (APP_Enable_RC == APP_Disable)
    {
      EMERGENCY_Brake(EM_Brake, Release_Brake, Throttle_RC);
      Drive_RC(Throttle_RC);
      Steering_RC(Steering_Reading, Steering_Control);
      Light_RC(Front_Light_RC, Throttle_RC, Drone_RC);
      // Drone_Control_RC(Drone_RC);

      Drone_Serial_Control();
      Siren_RC(Siren_12_RC, Siren_3_RC);
    }
    else if (APP_Enable_RC == APP_Enable)
    {
// 
    }
  }

  else
  {
    // Brake_Control_RC(Drive_RC_Zero);
    // Drive_RC(Drive_RC_Zero);
    // Light_RC(Front_Light_RC, Drive_RC_Zero, Drone_RC);
    // Steering_RC(Drive_RC_Zero, Steering_Control);
  }

      check_connection();

      Drive_APP(G_Throttle_Msg, G_Direction_Msg, G_Emergency_Brake_Msg);
      EMERGENCY_Brake_App(G_Emergency_Brake_Msg, G_Throttle_Msg);
      Steering_App(G_Steering);

      Light_App(G_Full_Light, G_Head_Light, G_Siren_Light_Msg, G_Direction_Msg);
      Siren_App(G_Siren1_Msg, G_Siren2_Msg, G_Siren3_Msg);
      Drone_Control_App(G_Drone_Launch_Msg);
      nh.spinOnce();
  //---------------- App ---------------------

  //  Brake_Control_Serial();

  delayMicroseconds(100);
}