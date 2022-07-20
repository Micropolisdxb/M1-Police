#ifndef DriveMotor_h
#define DriveMotor_h


#define Rev_SW          10
#define Throttle_Pin    1
#define Speedometer_Pin 34

#define Enable_Power_SW 36 // controlled from the power board

#define Drive_MAX_Speed_PWM 200
#define Drive_Min_Speed_PWM 0

#define Drive_RC_Zero 1002
#define Drive_RC_Max 1722
#define Drive_RC_Min 282


void Drive_Init();

void Drive_Serial_Control();

void Drive_APP(int Throttle_Val, String Direction_Mode, int Emergency_Brake);
void Drive_RC(int Throttle_Val);



#endif