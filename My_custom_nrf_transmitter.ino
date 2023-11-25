/*

Demonstrates simple RX and TX operation.
Any of the Basic_RX examples can be used as a receiver.
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
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define VRX_PIN  A0 // Arduino pin connected to VRX pin
#define VRY_PIN  A1 // Arduino pin connected to VRY pin
#define P_PIN 2 // Pin connected to detect button press

#define OLED_RESET -1

int xValue = 0; // To store value of the X axis
int yValue = 0; // To store value of the Y axis
int pValue = 0; // To store value of joystick button press

const static uint8_t RADIO_ID = 1;             // Our radio's id.
const static uint8_t DESTINATION_RADIO_ID = 97; // Id of the radio we will transmit to.
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
    uint32_t X;
    uint32_t Y;
    uint8_t Press;
};

NRFLite _radio;
RadioPacket _radioData;

Adafruit_SSD1306 display(OLED_RESET);

void setup()
{
    Serial.begin(115200);

    pinMode(P_PIN,INPUT_PULLUP);
    Disp_init();
    
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
    {
        display.setTextSize(2);             
        display.setTextColor(SSD1306_WHITE);       
        display.setCursor(0,10);             
        display.println("Failed Setup");
        display.display();
        Serial.println("Cannot communicate with radio");
        while (1); // Wait here forever.
    }
    display.clearDisplay();
    display.display();
    
}

void loop()
{
    xValue = analogRead(VRX_PIN);
    yValue = analogRead(VRY_PIN);
    pValue = digitalRead(P_PIN);
    
//    Serial.print("X: ");
//    Serial.print(xValue);
//    Serial.print(" y: ");
//    Serial.print(yValue);
//    Serial.print(" Press: ");
//    Serial.print(pValue);
    
    _radioData.X=xValue;
    _radioData.Y=yValue;
    _radioData.Press=pValue;
    
    if (_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData))) // Note how '&' must be placed in front of the variable name.
    {
        Serial.println("...Success");
        Dat(xValue,yValue,pValue);
    }
    else
    {
        Serial.println("...Failed");
        Dat(xValue,yValue,pValue);
    }

    delay(1);
}

void Disp_init()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);  // here the 0x3c is the I2C address, check your i2c address if u have multiple devices.
//  Serial.println(display.begin(SSD1306_SWITCHCAPVCC, 0x3C));
  display.display();
  display.clearDisplay();
  display.setTextSize(2);             
  display.setTextColor(SSD1306_WHITE);       
  display.setCursor(20,10);             
  display.println("MANDRED");
  display.display();
  delay(1000);
  display.clearDisplay();
  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  display.clearDisplay();
  display.setTextSize(1.2);             
  display.setTextColor(SSD1306_WHITE);       
  display.setCursor(12,10);             
  display.println("KOSMIC");
  display.setCursor(12,20);             
  display.println("CONTROLLER");
  display.fillTriangle(
      105  , 10,
      95, 20,
      115, 20, SSD1306_INVERSE);
  display.fillTriangle(
      95, 10,
      115, 10,
      105  , 20,SSD1306_INVERSE);
  display.drawCircle(105, 15, 10, SSD1306_WHITE);
  display.display();
  delay(3000);
  display.clearDisplay();
  display.display();

}

void Dat(uint32_t x,uint32_t y,uint8_t p)
{
  display.setTextSize(1);             
  display.setTextColor(SSD1306_WHITE);       
  display.setCursor(5,2);             
  display.print("X-Val : ");
  display.println(x);
  display.setCursor(5,12);
  display.print("Y-Val : ");
  display.println(y);
  display.setCursor(5,20);
  display.print("P-Val : ");
  display.println(p);
  display.display();
  delay(250);
  display.setTextColor(SSD1306_BLACK);       
  display.setCursor(5,2);             
  display.print("        ");
  display.println(x);
  display.setCursor(5,12);
  display.print("        ");
  display.println(y);
  display.setCursor(5,20);
  display.print("        ");
  display.println(p);
//  display.fillRect(50, 2, 40, 50, SSD1306_BLACK);
  display.display();
  delay(100);
}
