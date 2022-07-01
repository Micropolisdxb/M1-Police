#ifndef Drone_Launch_h
#define Drone_Launch_h

#define ARM_Dir 27  //Cytron 1 Dir 2
#define ARM_PWM 24  //Cytron 1 PWM 2
#define ARM_Pot 22
// #define ARM_Pot 38

#define Door1_Dir 32
#define Door1_PWM 28

#define Door2_Lifter_Dir 33
#define Door2_Lifter_PWM 29

// #define Door2_Enable 58
#define Door2_Enable 38
#define Lifter_Enable 35


#define ARM_Pot_Max 800
#define ARM_Pot_Min 300
#define ARM_Pot_Door1 450
#define ARM_Pot_Door2 600

#define Drone_Launch_Open 282
#define Drone_Launch_Close 1722

void Drone_Launch_Init();
void Drone_Control_App(int Drone_Control);



#endif