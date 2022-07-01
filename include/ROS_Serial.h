#ifndef ROS_Serial_h
#define ROS_Serial_h
#include <Arduino.h>
#include <ros.h>


extern ros::NodeHandle nh;
void ROS_Init();

extern String G_Mode_Msg ;
extern String G_Direction_Msg ;

extern int G_Emergency_Brake_Msg ; 
extern int G_Throttle_Msg ;
extern int G_Steering ;
extern int G_Full_Light ;
extern int G_Head_Light ;
extern int G_Siren1_Msg ;
extern int G_Siren2_Msg ;
extern int G_Siren3_Msg ;
extern int G_Siren_Light_Msg ;
extern int G_Disc_Brake_Msg ;
extern int G_Drone_Launch_Msg ;

#endif