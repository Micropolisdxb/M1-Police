#ifndef Drone_Launch_h
#define Drone_Launch_h

#define ARM_Dir 27  //Cytron 1 Dir 2
#define ARM_PWM 24  //Cytron 1 PWM 2
#define ARM_Pot 22
// #define ARM_Pot 38

#define Door1_Dir       32
#define Door1_PWM       29
#define Door1_Feedback  37

#define Door2_Lifter_Dir 33
#define Door2_Lifter_PWM 28
#define Door2_Feedback   41

// #define Door2_Enable 58
#define Door2_Enable_Pin 38
#define Lifter_Enable_Pin 35

// --------------------- Config--------------------
#define ARM_Pot_Max 800
#define ARM_Pot_Min 300
#define ARM_Pot_Door1 450
#define ARM_Pot_Door2 600

#define Arm_Open_State      HIGH
#define Arm_Close_State     LOW
#define Arm_Speed           125

#define Door1_Open_State    HIGH
#define Door1_Close_State   LOW
#define Door1_Speed         210

#define Door2_Lifter_Open_State     HIGH
#define Door2_Lifter_Close_State    LOW
#define Door2_Lifter_Speed          250

#define Door2_Enable LOW
#define Door2_Disable HIGH

#define Lifter_Enable LOW
#define Lifter_Disable HIGH

#define Drone_Launch_Open 282
#define Drone_Launch_Close 1722

void Drone_Launch_Init();
void Drone_Control_App(int Drone_Control);
void Drone_Serial_Control();




#endif