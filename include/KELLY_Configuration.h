/********************************************************************************/
/*   Author  : Ahmed Farag  & Wajih                                             */
/*   Date    : 18/05/2022                                                       */
/*   Version : V02                                                              */
/********************************************************************************/

#ifndef KELLY_CONFIGURATION_H
#define KELLY_CONFIGURATION_H


#define RC_FW_UPPER_LIMIT 1722
#define RC_DEFAULT_LIMIT 1002
#define RC_RV_LOWER_LIMIT 283


// Front motor driver parameters

#define THROTTLE_FORWARD_PIN 2
#define FORWARD_DIRECTION_SWITCH 11
#define THROTTLE_MIN_PWM 220
#define THROTTLE_MAX_PWM 220


// Back motor driver parameters

#define THROTTLE_REVERSE_PIN 3
#define REVERSE_DIRECTION_SWITCH 12
#define THROTTLE_MIN_PWM_REVERSE 360
#define THROTTLE_MAX_PWM_REVERSE 360

// RC Limits
#define STEERING_RC_Zero 1002
#define STEERING_RC_Max 1722
#define STEERING_RC_Min 282

void Steering_Init();
void Kelly_Front_Steering(int RC_Readings,int Throttle_Pin_num, int Throttle_Min_Readings, int Throttle_Max_Readings, int Throttle_Min_PWM, int Throttle_Max_PWM, int Front_Switch_pin ,int Switch_State);
void Kelly_Back_Steering(int RC_Readings,int Throttle_Pin_num,  int Throttle_Min_Readings, int Throttle_Max_Readings, int Throttle_Min_PWM, int Throttle_Max_PWM, int Back_Switch_Pin , int Switch_State);
void Steering_RC(int RC_Readings, int Steering_SW);


#endif