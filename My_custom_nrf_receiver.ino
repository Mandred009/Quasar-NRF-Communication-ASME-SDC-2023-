/*

Demonstrates simple RX and TX operation.
Any of the Basic_TX examples can be used as a transmitter.
Please read through 'NRFLite.h' for a description of all the methods available in the library.

Radio    Arduino
CE    -> 9
CSN   -> 10 (Hardware SPI SS)
MOSI  -> 11 (Hardware SPI MOSI) 
MISO  -> 12 (Hardware SPI MISO)
SCK   -> 13 (Hardware SPI SCK)
IRQ   -> No connection
VCC   -> No more than 3.6 volts
GND   -> GND

*/

#include "SPI.h"
#include "NRFLite.h"
#include <Servo.h>
#include <L298NX2.h>

Servo myservo;

const static uint8_t RADIO_ID = 97;       // Our radio's id.  The transmitter will send to this id.
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;

const unsigned int EN_A = 6;
const unsigned int IN1_A = 7;
const unsigned int IN2_A = 8;

const unsigned int IN1_B = 2;
const unsigned int IN2_B = 4;
const unsigned int EN_B = 5;

const int servo_pin = 3;

L298NX2 motors(EN_A, IN1_A, IN2_A, EN_B, IN1_B, IN2_B);

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
    uint32_t X;
    uint32_t Y;
    uint8_t Press;
};

NRFLite _radio;
RadioPacket _radioData;

int spee=0;
int speed_range[3]={100,175,220};
int turn_speed=125;

int left_angle=55;
int right_angle=115;
int center_angle=82;
int ang_flag=0;
void setup()
{
    TCCR2B = TCCR2B & B11111000 | B00000001; // for PWM frequency of 31372.55 Hz//PWM freq of 61.04Hz for pin 5 and 6
    Serial.begin(115200);

    motors.setSpeed(255);
    myservo.attach(servo_pin);
    myservo.write(center_angle);
    
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
    {
        Serial.println("Cannot communicate with radio");
        while (1); // Wait here forever.
    }
}

void loop()
{
//  Serial.println("Started Receiving");
    while (_radio.hasData())
    {
        _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name.

        if(ang_flag==0)
        {
          myservo.write(center_angle);
        }
        if(ang_flag<-1 || ang_flag>1)
        {
          ang_flag=0;
        }
        if(_radioData.Press==0)
        {
          spee+=1;
        }
        if(spee>2)
        {
          spee=0;
        }
        
        if(_radioData.Y<=490 &&(_radioData.X>500 && _radioData.X<=530))
        {
          speed_setter(speed_range[spee]);
          Motor1_forward();
          Motor2_forward();
        }
        if((_radioData.Y>490 && _radioData.Y<=520)&&(_radioData.X>500 && _radioData.X<=530))
        {
          Motor1_stop();
          Motor2_stop();
        }
        if(_radioData.Y>520 &&(_radioData.X>500 && _radioData.X<=530))
        {
          speed_setter(speed_range[spee]);
          Motor1_backward();
          Motor2_backward();
        }
        if(_radioData.X<=490 &&(_radioData.Y>490 && _radioData.Y<=520))
        {
          ang_flag-=1;
          speed_setter(turn_speed);
          Left();
        }
        if(_radioData.X>530 &&(_radioData.Y>490 && _radioData.Y<=520))
        {
          ang_flag+=1;
          speed_setter(turn_speed);
          Right();
        }
        Serial.println(_radioData.X);
    }
}

void Motor1_forward()
{
    motors.backwardA();
}
void Motor1_backward()
{
    motors.forwardA();
}
void Motor1_stop()
{
    motors.stopA();
}
void Motor2_forward()
{
    motors.backwardB();
}
void Motor2_backward()
{
    motors.forwardB();
}
void Motor2_stop()
{
    motors.stopB();
}
void Left()
{
  myservo.write(left_angle);
}
void Right()
{
  myservo.write(right_angle);
}
void speed_setter(int s)
{
  motors.setSpeed(s);
}
