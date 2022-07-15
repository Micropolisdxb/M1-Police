#include <Arduino.h>
#include <Light_Siren.h>

void Front_Light_App(int Full_Light, int Head_Light);

void Light_Init()
{

    pinMode(Back_Light, OUTPUT);
    pinMode(Big_Lights, OUTPUT);
    pinMode(Siren_Lights, OUTPUT);
    pinMode(Med_Small_Lights, OUTPUT);

    digitalWrite(Back_Light, LOW);
    digitalWrite(Big_Lights, LOW);
    digitalWrite(Siren_Lights, LOW);
    digitalWrite(Med_Small_Lights, LOW);
}

void Light_App(int Full_Light, int Head_Light, int Siren_Light_Val, String Drive_Direction)
{
    // Front Light
    if (Full_Light && !Head_Light)
    {
        digitalWrite(Big_Lights, LOW);
        digitalWrite(Med_Small_Lights, HIGH);
    }

    else if (!Full_Light && Head_Light)
    {
        digitalWrite(Big_Lights, HIGH);
        digitalWrite(Med_Small_Lights, LOW);
    }
    else if (Full_Light && Head_Light)
    {
        digitalWrite(Big_Lights, HIGH);
        digitalWrite(Med_Small_Lights, HIGH);
    }
    else if ((!Full_Light && !Head_Light))
    {
        digitalWrite(Big_Lights, LOW);
        digitalWrite(Med_Small_Lights, LOW);
    }

    // Siren Light
    digitalWrite(Siren_Lights, Siren_Light_Val);

    // Back Light
    if (Drive_Direction == "r")
    {
        digitalWrite(Back_Light, HIGH);
    }
    else if (Drive_Direction == "d")
    {
        digitalWrite(Back_Light, LOW);
    }
}

void Light_RC(int Front_Light_Control, int Throttle_Val, int Flash_Control)
{
    if (Flash_Control == Drone_Flashing)
    {
        // Front Light
        if (Front_Light_Control == Full_Front_Light) // Full Light
        {
            // Serial.println("Full_Front_Light");
            digitalWrite(Big_Lights, HIGH);
            digitalWrite(Med_Small_Lights, HIGH);
            digitalWrite(Siren_Lights, HIGH);
        }

        else if (Front_Light_Control == Head_Front_Light) // Head Light
        {
            // Serial.println("Head_Front_Light");
            digitalWrite(Big_Lights, HIGH);
            digitalWrite(Med_Small_Lights, LOW);
            digitalWrite(Siren_Lights, HIGH);
        }

        else if (Front_Light_Control == OFF_Front_Light) // OFF Light
        {
            // Serial.println("OFF_Front_Light");
            digitalWrite(Big_Lights, LOW);
            digitalWrite(Med_Small_Lights, LOW);
            digitalWrite(Siren_Lights, LOW);
        }

        // Back Light
        Throttle_Val < 1002 ? digitalWrite(Back_Light, HIGH) : digitalWrite(Back_Light, LOW);
    }
}

void Light_Control_Serial()
{

    char Light_Serial_Data = ' ';

    if (Serial.available())
    {
        Light_Serial_Data = Serial.read();
        Serial.println("-----------");
        Serial.printf("Serial: %c \n", Light_Serial_Data);
    }

    if (Light_Serial_Data == '1')
    {
        digitalWrite(Big_Lights, HIGH);
        digitalWrite(Med_Small_Lights, HIGH);

        Serial.println("Big,Meduim,Small: on");
    }

    else if (Light_Serial_Data == '2')
    {

        digitalWrite(Big_Lights, LOW);
        digitalWrite(Med_Small_Lights, HIGH);

        Serial.println("Medium,Small: on");
    }

    else if (Light_Serial_Data == '3')
    {

        digitalWrite(Big_Lights, LOW);
        digitalWrite(Med_Small_Lights, LOW);

        Serial.println("Big,Medium,Small : off");
    }
}

// *****************************   Siren   ***************************************************

int Prev_Siren1_2_RC = 0;
int Prev_Siren3 = 0;
unsigned long Siren_Prev_Millis = 0;
int counter = 0;

int Prev_Siren1_App = 0;
int Prev_Siren2_App = 0;
int Prev_Siren3_App = 0;

enum Siren_State
{
    Pulse1,
    Pulse2,
    Pulse3,
    Pulse3_Long,
    Wait,
    DeActivate
} Siren_S;

void Siren_States_Fn();

void Siren_Delay(int Siren_Pin, int delay_Time)
{
    digitalWrite(Siren_Pin, LOW);
    delay(delay_Time);
    digitalWrite(Siren_Pin, HIGH);
}
void Siren_Init()
{
    pinMode(Siren1, OUTPUT);
    pinMode(Siren2, OUTPUT);
    pinMode(Siren3, OUTPUT);

    digitalWrite(Siren1, HIGH);
    digitalWrite(Siren2, HIGH);
    digitalWrite(Siren3, HIGH);
    // Siren_Delay(Siren3, 10);
}

void Siren_Millis(int Siren_Pin, unsigned int Active_Time, unsigned int Wait_Time = 0)
{
    if (Wait_Time == 0)
    {
        if (millis() - Siren_Prev_Millis < Active_Time)
            digitalWrite(Siren_Pin, LOW);

        else
        {
            if (Siren_S == Pulse3_Long)
                Siren_S = DeActivate;
            else if (Siren_S == Pulse3)
                Siren_S = Wait;

            digitalWrite(Siren_Pin, HIGH);
            Siren_Prev_Millis = millis();
        }
    }

    else
    {
        if (millis() - Siren_Prev_Millis < Wait_Time)
        {
            // wait
        }
        else
        {
            if (counter == 1)
            { // wait until the siren is repeated twice
                Siren_S = Pulse3_Long;
                counter = 0;
            }
            else
            {
                Siren_S = Pulse3;
                counter++;
            }
            Siren_Prev_Millis = millis();
        }
    }
}
// void Siren_Millis(int Siren_Pin, unsigned short delay_Time)
// {
//     if (millis() - Siren_Prev_Millis < delay_Time)
//         digitalWrite(Siren_Pin, LOW);

//     else
//     {
//         digitalWrite(Siren_Pin, HIGH);
//     }
// }
void Siren_RC(int Siren1_2_RC, int Siren3_RC)
{
    if (Siren1_2_RC == Siren_SW_Min && Prev_Siren1_2_RC == Siren_SW_Zero) // Activate Siren 1
    {
        Siren_Delay(Siren1, Siren_Pulse_Duration);
        Serial.println("Activate Siren 1");
    }
    else if (Siren1_2_RC == Siren_SW_Zero && Prev_Siren1_2_RC == Siren_SW_Min) // Deactivate Siren 1
    {
        Siren_Delay(Siren1, Siren_Pulse_Duration);
        Serial.println(" Deactivate Siren 1");
    }

    else if (Siren1_2_RC == Siren_SW_Max && Prev_Siren1_2_RC == Siren_SW_Zero) // Activate Siren 2
    {
        Siren_Delay(Siren2, Siren_Pulse_Duration);
        Serial.println(" Activate Siren 2");
    }

    else if (Siren1_2_RC == Siren_SW_Zero && Prev_Siren1_2_RC == Siren_SW_Max) // Deactivate Siren 2
    {
        Siren_Delay(Siren2, Siren_Pulse_Duration);
        Serial.println(" Deactivate Siren 2");
    }

    else if (Siren3_RC == Siren_SW_Max && Prev_Siren3 == Siren_SW_Min)
    {
        Serial.println("Siren3");

        Siren_S = Pulse3;
        Siren_Prev_Millis = millis();
    }
    // else if (Siren3_RC == Siren_SW_Min && Prev_Siren3 == Siren_SW_Max)
    // {
    //     Siren_S = Pulse3;
    //     Siren_Prev_Millis = millis();
    // }
    Siren_States_Fn();

    Prev_Siren1_2_RC = Siren1_2_RC;
    Prev_Siren3 = Siren3_RC;
}

void Siren_App(int Siren1_App, int Siren2_App, int Siren3_App)
{

    if (Siren1_App != Prev_Siren1_App || Siren2_App != Prev_Siren2_App || Siren3_App != Prev_Siren3_App)
    {
        if (Siren1_App == 1 && Siren2_App == 0 && Siren3_App == 0) // Activate Siren 1
        {
            Siren_S = Pulse1;
            Siren_Delay(Siren1, Siren_Pulse_Duration);
        }
        else if (Siren1_App == 0 && Siren2_App == 1 && Siren3_App == 0) // Activate Siren 1
        {
            Siren_S = Pulse2;
            Siren_Delay(Siren2, Siren_Pulse_Duration);
        }
        else if (Siren1_App == 0 && Siren2_App == 0 && Siren3_App == 1) // Activate Siren 1
        {
            Siren_S = Pulse3;
            Siren_Prev_Millis = millis();
        }
        else if (Siren1_App == 0 && Siren2_App == 0 && Siren3_App == 0) // DeActivate Sirens
        {
            switch (Siren_S)
            {
            case Pulse1:
                Siren_Delay(Siren1, Siren_Pulse_Duration);
                break;
            case Pulse2:
                Siren_Delay(Siren2, Siren_Pulse_Duration);
                break;

            default:
                break;
            }
            Siren_Prev_Millis = millis();
        }
    }
    Prev_Siren1_App = Siren1_App;
    Prev_Siren2_App = Siren2_App;
    Prev_Siren3_App = Siren3_App;
    Siren_States_Fn();
}

void Siren_States_Fn()
{

    if (Siren_S == Pulse3)
    {
        Siren_Millis(Siren3, Siren3_Pulse);
    }
    else if (Siren_S == Pulse3_Long)
    {
        Siren_Millis(Siren3, Siren3_Long_Sound);
    }
    else if (Siren_S == Wait)
    {
        Siren_Millis(0, 0, Siren3_Wait);
    }
}
void Siren_Serial_Control()
{
    char Siren_Serial_Data = ' ';

    if (Serial.available() > 0)
    {
        Siren_Serial_Data = Serial.read();
        Serial.println("-----------");
        Serial.printf(" Siren Serial: %c \n", Siren_Serial_Data);
    }

    if (Siren_Serial_Data == '1')
    {
        Siren_Delay(Siren1, 5);
        Serial.println("Siren1");
    }

    else if (Siren_Serial_Data == '2')
    {
        Siren_Delay(Siren2, 5);
        Serial.println("Siren2");
    }

    else if (Siren_Serial_Data == '3')
    {
        Serial.println("Siren3");
        Siren_Delay(Siren3, 200);
        delay(70);

        Siren_Delay(Siren3, 200);
        delay(70);

        Siren_Delay(Siren3, 500);
    }
}