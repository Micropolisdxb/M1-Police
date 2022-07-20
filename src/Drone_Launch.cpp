#include <Arduino.h>
#include <Drone_Launch.h>
#include <Light_Siren.h>
#include <ROS_Serial.h>

enum DronLaunchEnum
{
    Arm_Open,
    Arm_Stop,
    Arm_Close,

    Door1_Open,
    Door1_Stop,
    Door1_Close,

    Door2_Open,
    Door2_Stop,
    Door2_Close,

    Lifter_Open,
    Lifter_Stop,
    Lifter_Close,

    All_Doors_Open,
    All_Doors_Stop,
    All_Doors_Close,

    Wait_To_Stop
} DronLaunchState;

enum DoorsStatesEnum
{
    Doors_Opening,
    Doors_Opened,
    Doors_Closing,
    Doors_Closed

} DoorState;

unsigned long Flash_PrevMillis = 0;
unsigned long Doors_PrevMillis = 0;

void Drone_Launch_State();

void Drone_Launch_Init()
{
    pinMode(ARM_PWM, OUTPUT);
    pinMode(ARM_Dir, OUTPUT);

    pinMode(Door1_PWM, OUTPUT);
    pinMode(Door1_Dir, OUTPUT);

    pinMode(Door2_Lifter_PWM, OUTPUT);
    pinMode(Door2_Lifter_Dir, OUTPUT);

    pinMode(Door2_Enable_Pin, OUTPUT);
    pinMode(Lifter_Enable_Pin, OUTPUT);

    pinMode(ARM_Pot, INPUT);
    pinMode(Door1_Feedback, INPUT_PULLUP);
    pinMode(Door2_Feedback, INPUT_PULLUP);

    analogWrite(ARM_PWM, 0);
    analogWrite(Door1_PWM, 0);
    analogWrite(Door2_Lifter_PWM, 0);

    digitalWrite(Door2_Enable_Pin, LOW);
    digitalWrite(Lifter_Enable_Pin, LOW);

    // if (digitalRead(Door1_Feedback) && digitalRead(Door2_Feedback))
    // DoorState = Doors_Closed;
    DoorState = Doors_Opened;
    DronLaunchState = All_Doors_Stop;
}

int Read_ARM_Pot()
{
    short iteration = 50;
    float avg = 0;

    for (int i = 0; i < iteration; i++)
    {
        avg += analogRead(ARM_Pot);
    }
    avg /= iteration;

    return int(avg);
}

void Flashing()
{
    digitalWrite(Big_Lights, LOW);

    if (millis() - Flash_PrevMillis > 750)
    {
        digitalWrite(Med_Small_Lights, !digitalRead(Med_Small_Lights));
        Flash_PrevMillis = millis();
    }
}

int PrevDroneState_RC = 0;
int PrevDroneState_App = 0;
int DoorsState = 0;
/**
 * TODO Opening:
 * ! Turn ON Flashing
 * ! Arm open
 * ! Door1 Open
 * ! Door2 Open
 * ! Lifter Open
 * ! Turn OFF Flashing
 *
 * Door2 and Lifter opens and closes together
 * */

/**
 * TODO Closing:
 * ! Start Flashing
 * ! Close Lifter and door 2
 * ! close door1
 * ! close arm
 * ! Stop flashing
 * */

void Drone_Control_App(int Drone_Control_APP)
{
    
    if (DoorState == Doors_Closed)
    { // check if doors are closed and ready to open

        if (Drone_Control_APP != PrevDroneState_App && Drone_Control_APP == 1)
        { // check if previous state diff than the current also check if its value
            Serial.println("Doors Oppening");
            DoorState = Doors_Opening;
            Doors_PrevMillis = millis();
        }
    }
    else if (DoorState == Doors_Opening)
    {
        if (millis() - Doors_PrevMillis < 13000) // 13
        {

            DronLaunchState = All_Doors_Open;
            Flashing();
        }
        else
        {
            DronLaunchState = All_Doors_Stop;
            DoorState = Doors_Opened;
            digitalWrite(Med_Small_Lights, LOW);
            Serial.println("Doors Oppened");
        }
        Drone_Launch_State();
    }
    else if (DoorState == Doors_Opened)
    { // check if doors are closed and ready to Close

        if (Drone_Control_APP != PrevDroneState_App && Drone_Control_APP == 0)
        { // check if previous state diff than the current also check if its value
            Serial.println("Doors Closing");
            DoorState = Doors_Closing;
            Doors_PrevMillis = millis();
        }
    }

    else if (DoorState == Doors_Closing)
    {
        if (millis() - Doors_PrevMillis < 15000) // 15
        {

            DronLaunchState = All_Doors_Close;
            Flashing();
        }
        else
        {
            DronLaunchState = All_Doors_Stop;
            DoorState = Doors_Closed;
            digitalWrite(Med_Small_Lights, LOW);
            Serial.println("Doors Closed");
        }
        Drone_Launch_State();
    }
    PrevDroneState_App = Drone_Control_APP;
}

void Drone_Control_RC(int Drone_Control_RC)
{

    if (DoorState == Doors_Closed)
    { // check if doors are closed and ready to open

        if (Drone_Control_RC != PrevDroneState_RC && Drone_Control_RC == Drone_Launch_Open)
        { // check if previous state diff than the current also check if its value
            Serial.println("Doors Oppening");
            DoorState = Doors_Opening;
            Doors_PrevMillis = millis();
        }
    }
    else if (DoorState == Doors_Opening)
    {
        if (millis() - Doors_PrevMillis < 13000) // 13
        {

            DronLaunchState = All_Doors_Open;
            Flashing();
        }
        else
        {
            DronLaunchState = All_Doors_Stop;
            DoorState = Doors_Opened;
            digitalWrite(Med_Small_Lights, LOW);
            Serial.println("Doors Oppened");
        }
        Drone_Launch_State();
    }
    else if (DoorState == Doors_Opened)
    { // check if doors are closed and ready to Close

        if (Drone_Control_RC != PrevDroneState_RC && Drone_Control_RC == Drone_Launch_Close)
        { // check if previous state diff than the current also check if its value
            Serial.println("Doors Closing");
            DoorState = Doors_Closing;
            Doors_PrevMillis = millis();
        }
    }

    else if (DoorState == Doors_Closing)
    {
        if (millis() - Doors_PrevMillis < 15000) // 15
        {

            DronLaunchState = All_Doors_Close;
            Flashing();
        }
        else
        {
            DronLaunchState = All_Doors_Stop;
            DoorState = Doors_Closed;
            digitalWrite(Med_Small_Lights, LOW);
            Serial.println("Doors Closed");
        }
        Drone_Launch_State();
    }
    PrevDroneState_RC = Drone_Control_RC;
}

void Actuators_Control(short Dir_Pin, short Dir_Val, short PWM_Pin, int Speed)
{
    digitalWrite(Dir_Pin, Dir_Val);
    analogWrite(PWM_Pin, Speed);
}

void Drone_Launch_State()
{
    switch (DronLaunchState)
    {

    case Arm_Open:
        Actuators_Control(ARM_Dir, Arm_Open_State, ARM_PWM, Arm_Speed);
        break;
    case Arm_Close:
        Actuators_Control(ARM_Dir, Arm_Close_State, ARM_PWM, Arm_Speed);
        break;
    case Arm_Stop:
        Actuators_Control(ARM_Dir, Arm_Close_State, ARM_PWM, 0);
        break;

    case Door1_Open:
        Actuators_Control(Door1_Dir, Door1_Open_State, Door1_PWM, Door1_Speed);
        break;
    case Door1_Close:
        Actuators_Control(Door1_Dir, Door1_Close_State, Door1_PWM, Door1_Speed);
        break;
    case Door1_Stop:
        Actuators_Control(Door1_Dir, Door1_Close_State, Door1_PWM, 0);
        break;

    case Door2_Open:
        Actuators_Control(Door2_Lifter_Dir, Door2_Lifter_Open_State, Door2_Lifter_PWM, Door2_Lifter_Speed);
        break;
    case Door2_Close:
        Actuators_Control(Door2_Lifter_Dir, Door2_Lifter_Close_State, Door2_Lifter_PWM, Door2_Lifter_Speed);
        break;
    case Door2_Stop:
        Actuators_Control(Door2_Lifter_Dir, Door2_Lifter_Close_State, Door2_Lifter_PWM, 0);
        break;

    case Lifter_Open:
        Actuators_Control(Door2_Lifter_Dir, Door2_Lifter_Open_State, Door2_Lifter_PWM, Door2_Lifter_Speed);
        break;
    case Lifter_Close:
        Actuators_Control(Door2_Lifter_Dir, Door2_Lifter_Close_State, Door2_Lifter_PWM, Door2_Lifter_Speed);
        break;
    case Lifter_Stop:
        Actuators_Control(Door2_Lifter_Dir, Door2_Lifter_Close_State, Door2_Lifter_PWM, 0);
        break;

    case All_Doors_Open:
        digitalWrite(Lifter_Enable_Pin, Lifter_Enable);
        digitalWrite(Door2_Enable_Pin, Door2_Enable);

        Actuators_Control(Door1_Dir, Door1_Open_State, Door1_PWM, Door1_Speed);
        Actuators_Control(Door2_Lifter_Dir, Door2_Lifter_Open_State, Door2_Lifter_PWM, Door2_Lifter_Speed);
        break;
    case All_Doors_Close:
        digitalWrite(Lifter_Enable_Pin, Lifter_Enable);
        digitalWrite(Door2_Enable_Pin, Door2_Enable);

        Actuators_Control(Door1_Dir, Door1_Close_State, Door1_PWM, Door1_Speed);
        Actuators_Control(Door2_Lifter_Dir, Door2_Lifter_Close_State, Door2_Lifter_PWM, Door2_Lifter_Speed);
        break;
    case All_Doors_Stop:
        Actuators_Control(Door2_Lifter_Dir, Door2_Lifter_Close_State, Door2_Lifter_PWM, 0);
        Actuators_Control(Door1_Dir, Door1_Close_State, Door1_PWM, 0);
        Actuators_Control(ARM_Dir, Arm_Close_State, ARM_PWM, 0);
        break;

    default:
        Actuators_Control(Door2_Lifter_Dir, Door2_Lifter_Close_State, Door2_Lifter_PWM, 0);
        Actuators_Control(Door1_Dir, Door1_Close_State, Door1_PWM, 0);
        Actuators_Control(ARM_Dir, Arm_Close_State, ARM_PWM, 0);
        break;
    }
}

unsigned long Drone_Serial_PrevMillis = 0;
int monitor = false;

void Drone_Serial_Control()
{
    char SerialChar = ' ';

    if (Serial.available() > 0)
    {
        SerialChar = Serial.read();
        Serial.println("-------------");
        Serial.printf("Drone Serial Data: %c \n", SerialChar);
        Serial.println("1: Lifter Open \t 2: Lifter Close \n3: Door1  Open \t 4: Door1  Close \n5: Door2  Open \t 6: Door2  Close \n7: Doors  Open \t 8: Doors  Close \n0: STOP");
    }

    if (millis() - Drone_Serial_PrevMillis > 100 && monitor)
    {
        Serial.printf("Limit switch: %d  \n", int(digitalRead(Door1_Feedback)));
        Drone_Serial_PrevMillis = millis();
    }

    if (SerialChar == '1')
    {
        DronLaunchState = Lifter_Open;
        digitalWrite(Lifter_Enable_Pin, Lifter_Enable);
        digitalWrite(Door2_Enable_Pin, Door2_Disable);

        Drone_Launch_State();

        // digitalWrite(ARM_Dir, Arm_Open_State); // to be checked
        // analogWrite(ARM_PWM, Arm_Speed);
        Serial.println("Lifter OPEN");
    }

    else if (SerialChar == '2')
    {
        digitalWrite(Lifter_Enable_Pin, Lifter_Enable);
        digitalWrite(Door2_Enable_Pin, Door2_Disable);

        DronLaunchState = Lifter_Close;
        Drone_Launch_State();

        // digitalWrite(ARM_Dir, Arm_Close_State);
        // analogWrite(ARM_PWM, Arm_Speed);
        Serial.println("Lifter Close");
    }

    else if (SerialChar == '3')
    {
        DronLaunchState = Door1_Open;
        Drone_Launch_State();
        Serial.println("Door1 OPEN");
    }

    else if (SerialChar == '4')
    {
        DronLaunchState = Door1_Close;
        Drone_Launch_State();

        Serial.println("Door1 Close");
    }

    else if (SerialChar == '5')
    {
        digitalWrite(Door2_Enable_Pin, Door2_Enable);
        digitalWrite(Lifter_Enable_Pin, Lifter_Disable);

        DronLaunchState = Door2_Open;
        Drone_Launch_State();

        Serial.println("Door2 OPEN");
    }

    else if (SerialChar == '6')
    {
        digitalWrite(Door2_Enable_Pin, Door2_Enable);
        digitalWrite(Lifter_Enable_Pin, Lifter_Disable);

        DronLaunchState = Door2_Close;
        Drone_Launch_State();
        Serial.println("Door2 Close");
    }

    else if (SerialChar == '7')
    {
        DronLaunchState = All_Doors_Open;
        Drone_Launch_State();

        Serial.println("Doors OPEN");
    }

    else if (SerialChar == '8')
    {
        DronLaunchState = All_Doors_Close;
        Drone_Launch_State();

        Serial.println("Doors Close");
    }

    else if (SerialChar == '0')
    {
        DronLaunchState = All_Doors_Stop;
        Drone_Launch_State();

        Serial.println("!!!STOP!!!");
    }
    else if (SerialChar == 's')
    {
        monitor = !monitor;
        Serial.printf("Monitor: %d  \n", monitor);
    }
}
