#include <Arduino.h>
#include <Drone_Launch.h>
#include <Light_Siren.h>
#include <ROS_Serial.h>

enum DronLaunchStates
{
    Arm_Open,
    Arm_Stop,
    Arm_Close,

    Door1_Open,
    Door1_Close,

    Door2_Lifter_Open,
    Door2_Lifter_Close,

    Door1_Door2_Lifter_Stop,
    Wait_To_Stop
} DLS;

unsigned long Flash_PrevMillis = 0;
unsigned long Doors_Open_PrevMillis = 0;
unsigned long Doors_Close_PrevMillis = 0;

void Drone_Launch_State_Fn();

void Drone_Launch_Init()
{
    pinMode(ARM_PWM, OUTPUT);
    pinMode(ARM_Dir, OUTPUT);

    pinMode(Door1_PWM, OUTPUT);
    pinMode(Door1_Dir, OUTPUT);

    pinMode(Door2_Lifter_PWM, OUTPUT);
    pinMode(Door2_Lifter_Dir, OUTPUT);

    pinMode(Door2_Enable, OUTPUT);
    pinMode(Lifter_Enable, OUTPUT);

    pinMode(ARM_Pot, INPUT);

    analogWrite(ARM_PWM, 0);
    analogWrite(Door1_PWM, 0);

    digitalWrite(Door2_Enable, LOW);
    digitalWrite(Lifter_Enable, LOW);
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
 * Opening:
 *
 * Turn ON Flashing
 * Arm open
 * Door1 Open
 * Door2 Open
 * Lifter Open
 * Turn OFF Flashing
 *
 * Door2 and Lifter opens and closes together
 * */

/**
 * Closing:
 *
 * Start Flashing
 * Close Lifter and door 2
 * close door1
 * close arm
 * Stop flashing
 * */

void Drone_Control_App(int Drone_Control)
{
    if (Drone_Control != PrevDroneState_App && Drone_Control == 0)
        Doors_Close_PrevMillis = millis();

    if (Drone_Control == 1 && Read_ARM_Pot() < (ARM_Pot_Max * 0.1)) // Open Doors
    {
        DoorsState = 1;
    }
    else if (Drone_Control == 0 && Read_ARM_Pot() > (ARM_Pot_Max * 0.9))
    {
        DoorsState = 2;
    }
    else
    {
        // nh.loginfo("Doors Wait ...");
    }

    if (DoorsState == 1)
    {
        // nh.loginfo("Doors Openning ...");
        Flashing();

        if (Read_ARM_Pot() < ARM_Pot_Max)
            DLS = Arm_Open;

        else
            DLS = Arm_Stop;

        if (Read_ARM_Pot() == ARM_Pot_Door1)
            DLS = Door1_Open;

        else if (Read_ARM_Pot() == ARM_Pot_Door2)
        {
            DLS = Door2_Lifter_Open;
        }
    }

    else if (DoorsState == 2)
    {
        // nh.loginfo("Doors Closing ...");

        Flashing();

        if (millis() - Doors_Close_PrevMillis < 5000)
            DLS = Door2_Lifter_Close;
        else if (millis() - Doors_Close_PrevMillis < 8000)
            DLS = Door1_Close;
        else if (millis() - Doors_Close_PrevMillis < 12000)
            DLS = Arm_Close;
    }
    PrevDroneState_App = Drone_Control;
    Drone_Launch_State_Fn();
}

void Drone_Control_RC(int Drone_Control_RC)
{
    if (Drone_Control_RC != PrevDroneState_RC && Drone_Control_RC == Drone_Launch_Open)
        Doors_Close_PrevMillis = millis();
    /**
     * Turn ON Flashing
     * Arm open
     * Door1 Open
     * Door2 Open
     * Lifter Open
     * Turn OFF Flashing
     *
     * Door2 and Lifter opens and closes together
     * */

    if (Drone_Control_RC == Drone_Launch_Open && Read_ARM_Pot() < (ARM_Pot_Max * 0.1)) // Open Doors
    {
        DoorsState = 1;
    }
    else if (Drone_Control_RC == Drone_Launch_Close && Read_ARM_Pot() > (ARM_Pot_Max * 0.9))
    {
        DoorsState = 2;
    }
    else
    {
        nh.loginfo("Doors Wait ...");
    }

    if (DoorsState == 1)
    {
        nh.loginfo("Doors Openning ...");
        Flashing();

        if (Read_ARM_Pot() < ARM_Pot_Max)
            DLS = Arm_Open;

        else
            DLS = Arm_Stop;

        if (Read_ARM_Pot() == ARM_Pot_Door1)
            DLS = Door1_Open;

        else if (Read_ARM_Pot() == ARM_Pot_Door2)
        {
            DLS = Door2_Lifter_Open;
        }
    }

    else if (DoorsState == 2)
    {
        nh.loginfo("Doors Closing ...");

        Flashing();

        if (millis() - Doors_Close_PrevMillis < 5000)
            DLS = Door2_Lifter_Close;
        else if (millis() - Doors_Close_PrevMillis < 8000)
            DLS = Door1_Close;
        else if (millis() - Doors_Close_PrevMillis < 12000)
            DLS = Arm_Close;
    }

    PrevDroneState_RC = Drone_Control_RC;
    Drone_Launch_State_Fn();
}

void Drone_Launch_State_Fn()
{
    if (DLS == Arm_Open)
    {
        digitalWrite(ARM_Dir, HIGH);
        analogWrite(ARM_PWM, 255);
    }

    else if (DLS == Door1_Open)
    {
        digitalWrite(Door1_Dir, HIGH);
        analogWrite(Door1_PWM, 255);
    }

    else if (DLS == Door2_Lifter_Open)
    {
        digitalWrite(Door2_Enable, HIGH);
        digitalWrite(Lifter_Enable, HIGH);
        digitalWrite(Door2_Lifter_Dir, HIGH);
        analogWrite(Door2_Lifter_PWM, 255);
        Doors_Open_PrevMillis = millis();
        DLS = Wait_To_Stop;
    }

    else if (DLS == Arm_Close)
    {
        if (Read_ARM_Pot() > ARM_Pot_Min)
            DLS = Arm_Close;

        else
        {
            digitalWrite(Med_Small_Lights, LOW);
            DLS = Arm_Stop;
        }

        digitalWrite(ARM_Dir, LOW);
        analogWrite(ARM_PWM, 125);
    }

    else if (DLS == Door1_Close)
    {
        digitalWrite(Door1_Dir, LOW);
        analogWrite(Door1_PWM, 255);
    }

    else if (DLS == Door2_Lifter_Close)
    {
        digitalWrite(Door2_Enable, HIGH);
        digitalWrite(Lifter_Enable, HIGH);
        digitalWrite(Door2_Lifter_Dir, LOW);
        analogWrite(Door2_Lifter_PWM, 255);
    }

    else if (DLS == Arm_Stop)
    {
        analogWrite(ARM_PWM, 0);
        DLS = Wait_To_Stop;
        Doors_Close_PrevMillis = millis();
    }

    else if (DLS == Door1_Door2_Lifter_Stop)
    {
        digitalWrite(Med_Small_Lights, LOW);

        analogWrite(Door1_PWM, 0);

        digitalWrite(Door2_Enable, LOW);
        digitalWrite(Lifter_Enable, LOW);
        analogWrite(Door2_Lifter_PWM, 0);
    }

    else if (DLS == Wait_To_Stop)
    {
        analogWrite(Door2_Lifter_PWM, 255);

        if (DoorsState == 1 && (millis() - Doors_Open_PrevMillis) > 15000)
        {
            DLS = Door1_Door2_Lifter_Stop;
            DoorsState = 0;
        }
        if (DoorsState == 2 && (millis() - Doors_Close_PrevMillis) > 15000)
        {
            DLS = Door1_Door2_Lifter_Stop;
            DoorsState = 0;
        }
    }
}
unsigned long Drone_PrevMillis = 0;

void Drone_Serial_Control()
{
    char SerialChar = ' ';

    if (Serial.available() > 0)
    {
        SerialChar = Serial.read();
        Serial.println("-------------");
        Serial.printf("Drone Serial Data: %c \n", SerialChar);
    }

    if (millis() - Drone_PrevMillis > 100)
    {
        Serial.printf("Speed: %d  \n", int(Read_ARM_Pot()));
        Drone_PrevMillis = millis();
    }

    if (SerialChar == '1')
    {
        digitalWrite(ARM_Dir, HIGH);
        analogWrite(ARM_PWM, 125);
        Serial.println("Arm HIGH");
    }

    else if (SerialChar == '2')
    {
        digitalWrite(ARM_Dir, LOW);
        analogWrite(ARM_PWM, 125);
        Serial.println("Arm LOW");
    }

    else if (SerialChar == '3')
    {
        digitalWrite(Door1_Dir, HIGH);
        analogWrite(Door1_PWM, 125);
        Serial.println("Door1 HIGH");
    }

    else if (SerialChar == '4')
    {
        digitalWrite(Door1_Dir, LOW);
        analogWrite(Door1_PWM, 125);
        Serial.println("Door1 LOW");
    }
    else if (SerialChar == '5')
    {
        digitalWrite(Door2_Lifter_Dir, HIGH);
        analogWrite(Door2_Lifter_PWM, 125);
        Serial.println("Door2 Lifter HIGH");
    }

    else if (SerialChar == '6')
    {
        digitalWrite(Door2_Lifter_Dir, LOW);
        analogWrite(Door2_Lifter_PWM, 125);
        Serial.println("Door2 Lifter LOW");
    }
    else if (SerialChar == '7')
    {
        digitalWrite(Door2_Enable, !digitalRead(Door2_Enable));
        Serial.println("Door2 enable");
    }

    else if (SerialChar == '8')
    {
        digitalWrite(Lifter_Enable, !digitalRead(Lifter_Enable));
        Serial.println("Lifter enable");
    }
    else if (SerialChar == '0')
    {
        digitalWrite(Lifter_Enable, LOW);
        digitalWrite(Door2_Enable, LOW);
        analogWrite(ARM_PWM, 0);
        analogWrite(Door1_PWM, 0);
        analogWrite(Door2_Lifter_PWM, 0);
        Serial.println("Lifter enable");
    }
}
