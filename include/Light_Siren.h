#ifndef Light_Siren_h
#define Light_Siren_h
#include <Arduino.h>


#define Back_Light          7
#define Big_Lights          6
#define Siren_Lights        13
#define Med_Small_Lights    5

#define Full_Front_Light   1722
#define Head_Front_Light   282
#define OFF_Front_Light    1002

#define Siren_Light_ON  1722
#define Siren_Light_OFF 282

#define Siren1 8
#define Siren2 9
#define Siren3 16

#define Siren_SW_Min    283
#define Siren_SW_Zero   1002
#define Siren_SW_Max    1722

#define Siren_Pulse_Duration 5
#define Siren3_Pulse 200
#define Siren3_Wait  70
#define Siren3_Long_Sound 500

void Light_Init();

void Light_App(int Full_Light, int Head_Light, int Siren_Light_Val, String Drive_Direction);
void Light_RC(int Front_Light_Control, int Throttle_Val);


void Siren_Init();

void Siren_App(int Siren1_App, int Siren2_App, int Siren3_App);
void Siren_RC(int Siren1_2_RC, int Siren3_RC);


#endif