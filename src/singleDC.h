#ifndef SINGLE_DC_H
#define SINGLE_DC_H


#include "Adafruit_NeoPixel.h"


class TwinDCMotor : public Adafruit_NeoPixel  
{
public:

    uint8_t num_serial_motors;

    /* Constructer for coding mode
    / red color output of WS2811 is connected to STM8s ADC1 channel 4. 
    / We are using Adafruit_NeoPixel to communicate with WS2811.
    / n: number of serial motors
    / p: dc motor digital pin
    */
    TwinDCMotor(uint8_t n, uint16_t p) : Adafruit_NeoPixel(n, p, NEO_RGB + NEO_KHZ800) 
    {
		this->num_serial_motors = n;
        begin();
        setBrightness(255);
        show();
    }

	~TwinDCMotor()
    {
        // begin();
        // setBrightness(255);
        // show();

		for (uint8_t mtr_id = 0; mtr_id < this->num_serial_motors; mtr_id++)
		{
			setDirAndSpeedUsingNeopixelLibrary(mtr_id, 0);
		}
    }

    void setDirAndSpeedCoding(uint8_t motorId, uint8_t dir, uint8_t speed)
    {
        if(speed > 100) {
            speed = 100;
        }
		
        
        // left-most bit indicates direction, the other seven speed
        uint8_t m_dir_speed = ((speed)&0x7f) | ((dir & 1) << 7);
        setDirAndSpeedUsingNeopixelLibrary(motorId, m_dir_speed);
    }

private:
    void setDirAndSpeedUsingNeopixelLibrary(uint8_t motorId, uint8_t redColor)
    {
        // try two times
        setPixelColor(motorId, 252 - redColor, 0, 0); 
        show();
        setPixelColor(motorId, 252 - redColor, 0, 0); 
        show();
    }
};



#endif //SINGLE_DC_H