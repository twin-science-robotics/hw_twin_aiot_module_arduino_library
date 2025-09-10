#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "Constants.h"
#include "BLEManager.h"
#include "DHT11.h" 
#include <ESP32Servo.h>
#include "singleDC.h"
#include <HardwareSerial.h>
extern HardwareSerial *doubledc ;

extern TwinDCMotor* dc_coding_at_pin_06_ptr ;
extern TwinDCMotor* dc_coding_at_pin_09_ptr ;
extern TwinDCMotor* dc_coding_at_pin_0A_ptr;

extern Servo	servo6;
extern Servo	servo9;
extern Servo	servo10;

extern QueueHandle_t patternQueue;
class CommandHandler {
public:
  CommandHandler();
  void handleCommand(Packet cmd);
  void applyCommands();
  void getResponse();
  void initTwin_F();
  uint8_t reverseBits(uint8_t b);
  void shiftOutAll(uint8_t colData, uint8_t rowData);
  
  // Double dc motor variables
  uint8_t motor_1_dir = 0;
  uint8_t motor_2_dir = 0;
  uint8_t motor_1_speed = 0;
  uint8_t motor_2_speed = 0;

private:
  uint8_t chooseSelect(uint8_t pin);
  uint8_t getActualPin(uint8_t logicalPin,  bool signal_type);
  uint8_t map_piano(uint8_t pin);

  
  void digitalWrite_F();
  void digitalRead_F();
  void analogWrite_F();
  void analogRead_F();
  void pianoButton_F();
  void servo_F();
  void hcsr04_F();
  void rgbLED_F();
  void getfirmwareVersion_F();
  void playNote_F();
  void ledMatrix_F();
  void singleDC_F();
  void doubleDC_F();
  void tempAndhum_F();
  void touchPad_F();
};

extern CommandHandler commandHandler;

#endif // COMMAND_HANDLER_H