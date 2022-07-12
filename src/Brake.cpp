#include <Brake.h>
#include <Arduino.h>
#include <ROS_Serial.h>
enum Brake_States
{
    Brake,
    Braked,
    Release,
    Released
} B_S;

int previous_throttle = 0;
unsigned long PrevMillis_Brake = 0;

void Brake_System();

// Throttle_Value = map(Throttle_Value, Throttle_Zero, Throttle_Max, 0, 100);
//  Serial.printf("Throttle_Value: %d " , Throttle_Value);
//  Serial.printf("  previous_throttle: %d \n " , previous_throttle);
int Read_Brake_Pot()
{
    short iteration = 50;
    float avg = 0;

    for (int i = 0; i < iteration; i++)
    {
        avg += analogRead(Brake_Pot);
    }
    avg /= iteration;
    // Serial.println(avg);
    return int(avg);
}

void Brake_init()
{
    pinMode(Brake_Dir, OUTPUT);
    pinMode(Brake_PWM, OUTPUT);

    pinMode(Brake_Pot, INPUT);

    B_S = Brake; // Go to Braking State
    Brake_System();

    Serial.println("Braking Init");
}

void Brake_Control_App(int Throttle_Value)
{
    if (Throttle_Value > App_Throttle_Zero && Throttle_Value <= App_Throttle_Max)
    {
        if ((Throttle_Value - previous_throttle >= 1) && B_S != Released)
        {
            B_S = Release;

            Serial.printf("Forward  Releasing %d  \n", Throttle_Value);
            // previous_throttle = Throttle_Value;
        }

        else if ((Throttle_Value - previous_throttle <= -1) && Throttle_Value < 30 && B_S != Braked)
        {
            B_S = Brake;

            Serial.printf("Forward Braking %d  \n", Throttle_Value);
        }
        previous_throttle = Throttle_Value;
    }

    else if (Throttle_Value == App_Throttle_Zero && B_S != Braked)
    {
        B_S = Brake;
    }

    Brake_System();
}

void EMERGENCY_Brake(int Emergency_Brake, int Release_Control, int Throttle)
{
    // Serial.printf("Emergency_Brake: %d  \n", Emergency_Brake);
    // Serial.printf("Release_Control: %d  \n", Release_Control);
    if ((Emergency_Brake > 1500) && B_S != Braked)
    {
        B_S = Brake;
    }
    else if (Release_Control > 1500 && B_S != Released)
    {
        B_S = Release;
    }
    else if (Emergency_Brake < 500 && Release_Control < 500)
    {
        // B_S = Release;
        Brake_Control_RC(Throttle);
    }

    Brake_System();
}
void Brake_Control_RC(int Throttle_Value)
{

    if (Throttle_Value > (RC_Throttle_Zero + 2))
    {
        int Throttle_PWM = map(Throttle_Value, RC_Throttle_Zero, RC_Throttle_Max, App_Throttle_Zero, App_Throttle_Max);

        if ((Throttle_PWM - previous_throttle > 1) && B_S != Released)
        {
            B_S = Release;

            Serial.printf("Forward  Releasing %d  \n", Throttle_PWM);
        }

        else if ((Throttle_PWM - previous_throttle < -1) && Throttle_PWM < 30 && B_S != Braked)
        {
            B_S = Brake;

            Serial.printf("Forward Braking %d  \n", Throttle_PWM);
        }
        previous_throttle = Throttle_PWM;
    }
    else if (Throttle_Value < (RC_Throttle_Zero - 2))
    {
        int Throttle_PWM = map(Throttle_Value, RC_Throttle_Min, RC_Throttle_Zero, App_Throttle_Max, App_Throttle_Zero);

        if ((Throttle_PWM - previous_throttle > 1) && B_S != Released)
        {
            B_S = Release;

            Serial.printf("Forward  Releasing %d  \n", Throttle_PWM);
        }

        else if ((Throttle_PWM - previous_throttle < -1) && Throttle_PWM < 30 && B_S != Braked)
        {
            B_S = Brake;

            Serial.printf("Forward Braking %d  \n", Throttle_PWM);
        }
        previous_throttle = Throttle_PWM;
    }

    else if (Throttle_Value == RC_Throttle_Zero && B_S != Braked)
    {
        Serial.println("Brake Stop");

        B_S = Brake;
    }

    Brake_System();
}

void Brake_System()
{
    if (B_S == Brake)
    {
        if (Read_Brake_Pot() < Pot_Brake_Limit)
        {
            digitalWrite(Brake_Dir, HIGH);
            analogWrite(Brake_PWM, Brake_Motor_Speed_PWM);
            // Serial.println("Brake");
        }
        else
            B_S = Braked;
    }

    else if (B_S == Braked)
    {
        analogWrite(Brake_PWM, 0);
        // Serial.println("Braked");
    }

    else if (B_S == Release)
    {
        if (Read_Brake_Pot() > Pot_Release_Limit)
        {
            digitalWrite(Brake_Dir, LOW);
            analogWrite(Brake_PWM, Brake_Motor_Speed_PWM);
            // Serial.println("release");
        }
        else
            B_S = Released;
    }

    else if (B_S == Released)
    {
        // Serial.println("Released");

        analogWrite(Brake_PWM, 0);
    }
}

void Brake_Control_Serial()
{
    char Serial_Data = ' ';

    if (Serial.available())
    {
        Serial_Data = Serial.read();
        Serial.println("-----------");
        Serial.printf("Serial: %c \n", Serial_Data);
    }
    if (millis() - PrevMillis_Brake > 200)
    {
        Serial.printf("Brake Pot Val: %d \n", Read_Brake_Pot());
        PrevMillis_Brake = millis();
    }

    if (Serial_Data == '1')
    {
        Serial.println("Direction HIGH Release");
        digitalWrite(Brake_Dir, HIGH);
        analogWrite(Brake_PWM, 255);
    }

    else if (Serial_Data == '2')
    {
        Serial.println("Direction LOW Brake");
        digitalWrite(Brake_Dir, LOW);
        analogWrite(Brake_PWM, 255);
    }
    else if (Serial_Data == '3')
    {
        Serial.println("Direction LOW");
        digitalWrite(Brake_Dir, LOW);
        analogWrite(Brake_PWM, 0);
    }
}