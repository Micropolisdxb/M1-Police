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
        digitalWrite(Door2_Enable_Pin, HIGH);
        digitalWrite(Lifter_Enable_Pin, HIGH);
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
        digitalWrite(Door2_Enable_Pin, HIGH);
        digitalWrite(Lifter_Enable_Pin, HIGH);
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

        digitalWrite(Door2_Enable_Pin, LOW);
        digitalWrite(Lifter_Enable_Pin, LOW);
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
int state = 0;
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

        Serial.printf("Limit switch: %d  \n", int(digitalRead(Door1_Feedback)));
        Drone_PrevMillis = millis();
    }

    if (SerialChar == '1')
    {
        digitalWrite(ARM_Dir, Arm_Open_State); // to be checked
        analogWrite(ARM_PWM, Arm_Speed);
        Serial.println("Arm OPEN");
    }

    else if (SerialChar == '2')
    {
        digitalWrite(ARM_Dir, Arm_Close_State);
        analogWrite(ARM_PWM, Arm_Speed);
        Serial.println("Arm Close");
    }

    else if (SerialChar == '3')
    {
        digitalWrite(Door1_Dir, Door1_Open_State);
        analogWrite(Door1_PWM, Door1_Speed);
        Serial.println("Door1 OPEN");
    }

    else if (SerialChar == '4')
    {
        digitalWrite(Door1_Dir, Door1_Close_State);
        analogWrite(Door1_PWM, Door1_Speed);
        Serial.println("Door1 Close");
    }
    else if (SerialChar == '5')
    {
        // digitalWrite(Door2_Enable_Pin, HIGH);

        digitalWrite(Door2_Lifter_Dir, Door2_Lifter_Open_State);
        analogWrite(Door2_Lifter_PWM, Door2_Lifter_Speed);
        Serial.println("Door2 Lifter OPEN");
    }

    else if (SerialChar == '6')
    {
        // digitalWrite(Door2_Enable_Pin, HIGH);

        digitalWrite(Door2_Lifter_Dir, Door2_Lifter_Close_State);
        analogWrite(Door2_Lifter_PWM, Door2_Lifter_Speed);
        Serial.println("Door2 Lifter Close");
    }
    else if (SerialChar == '7')
    {
        digitalWrite(Door2_Enable_Pin, !digitalRead(Door2_Enable_Pin));
        Serial.println("Door2 enable: " + String(digitalRead(Door2_Enable_Pin)));
    }

    else if (SerialChar == '8')
    {
        digitalWrite(Lifter_Enable_Pin, !digitalRead(Lifter_Enable_Pin));
        Serial.println("Lifter enable: " + String(digitalRead(Lifter_Enable_Pin)));
    }
    // else if (SerialChar == '8')
    // {
    //     state = 1;
    //     Drone_PrevMillis = millis();
    // }

    else if (SerialChar == '9')
    {

        state = 2;
        Drone_PrevMillis = millis();

    }

    else if (SerialChar == '0')
    {
        digitalWrite(Lifter_Enable_Pin, Lifter_Disable);
        digitalWrite(Door2_Enable_Pin, Door2_Disable);
        analogWrite(ARM_PWM, 0);
        analogWrite(Door1_PWM, 0);
        analogWrite(Door2_Lifter_PWM, 0);
        Serial.println("!!!STOP!!!");
    }
    if (state == 1)
    {
        if (millis() - Drone_PrevMillis < 2000)
        {
            Serial.println("Doors Open:");
            Serial.println("- Door1 Open:");
            digitalWrite(Door1_Dir, Door1_Open_State);
            analogWrite(Door1_PWM, Door1_Speed);
        }
        else if (millis() - Drone_PrevMillis < 12000)
        {
            Serial.println("- Door2 Open:");
            digitalWrite(Lifter_Enable_Pin, Lifter_Enable);
            digitalWrite(Door2_Enable_Pin, Door2_Enable);
            digitalWrite(Door2_Lifter_Dir, Door2_Lifter_Open_State);
            analogWrite(Door2_Lifter_PWM, Door2_Lifter_Speed);
        }
        else if (millis() - Drone_PrevMillis > 12000)
        {
            Serial.println("- !!!STOP!!!");
            digitalWrite(Lifter_Enable_Pin, Lifter_Disable);
            digitalWrite(Door2_Enable_Pin, Door2_Disable);
            analogWrite(Door1_PWM, 0);
            analogWrite(Door2_Lifter_PWM, 0);
            state = 0;
        }
    }
    else if (state == 2)
    {
            Serial.println("Doors Close:");
        if (millis() - Drone_PrevMillis < 2000)
        {
            Serial.println("- Door2 Close:");
            digitalWrite(Lifter_Enable_Pin, Lifter_Enable);
            digitalWrite(Door2_Enable_Pin, Door2_Enable);

            digitalWrite(Door2_Lifter_Dir, Door2_Lifter_Close_State);
            analogWrite(Door2_Lifter_PWM, Door2_Lifter_Speed);
        }
        else if (millis() - Drone_PrevMillis < 12000)
        {
            Serial.println("- Door1 Close:");
            digitalWrite(Door1_Dir, Door1_Close_State);
            analogWrite(Door1_PWM, Door1_Speed);
        }
        else if (millis() - Drone_PrevMillis > 12000)
        {
            Serial.println("- !!!STOP!!!");
            digitalWrite(Lifter_Enable_Pin, Lifter_Disable);
            digitalWrite(Door2_Enable_Pin, Door2_Disable);
            analogWrite(Door1_PWM, 0);
            analogWrite(Door2_Lifter_PWM, 0);
            state = 0;
        }
    }
}
