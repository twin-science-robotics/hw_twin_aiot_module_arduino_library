#include "CommandHandler.h"

Servo	servo6;
Servo	servo9;
Servo	servo10;

TwinDCMotor* dc_coding_at_pin_06_ptr = NULL;
TwinDCMotor* dc_coding_at_pin_09_ptr = NULL;
TwinDCMotor* dc_coding_at_pin_0A_ptr = NULL;

HardwareSerial *doubledc = NULL;

CommandHandler::CommandHandler() {
}

void CommandHandler::handleCommand(Packet cmd) {

  if (cmd.data[0] == SYNC_BYTE1 && cmd.data[1] == SYNC_BYTE2 && cmd.data[2] == SYNC_BYTE3) {
    bleChannel.received.message_id = cmd.data[3];
    bleChannel.received.message_length = cmd.data[4];

    for (int i = 0; i < bleChannel.received.message_length; i++) {
      bleChannel.received.message_data[i] = cmd.data[header_length + i];
    }

    //Serial.print("receiving: ");
    //for (int i = 0; i < bleChannel.received.message_length + header_length; i++) {
     // Serial.printf("%02X ", cmd.data[i]);
    //}
   // Serial.println();

    applyCommands();
    getResponse();
  } else {
    //Serial.println("Invalid sync bytes");
  }
}

void CommandHandler::applyCommands() {
  switch (bleChannel.received.message_id) {

    case DIGITAL_WRITE:
        digitalWrite_F();
        break;

      case DIGITAL_READ:
        digitalRead_F();
        break;

      case ANALOG_WRITE:
        analogWrite_F();
        break;

      case ANALOG_READ:
        analogRead_F(); 
        break;

      case PIANO_BUTTON:
        pianoButton_F(); 
        break;

      case SERVO:
        servo_F(); 
        break;
      
      case HCSR04:
        hcsr04_F(); 
        break;

      case RGB_LED:
        rgbLED_F(); 
        break;    

      case INIT_TWIN:
        initTwin_F();        
        break;

      case GETFIRMWAREVERSION:
        getfirmwareVersion_F(); 
        break;

      case PLAYNOTE:
        playNote_F();
        break;

      case SINGLE_DC:
        singleDC_F();
        break;
      
      case TEMPRATUREHUMIDITY:
        tempAndhum_F();
        break;

      case DOUBLE_DC:
        doubleDC_F();
        break;

      case TOUCH_PAD:
        touchPad_F();
        break;

      case LED_MATRIX:
        ledMatrix_F();
        break;
    }
}


void CommandHandler::getResponse() {
  Packet packet = {0};
  packet.data[0] = SYNC_BYTE1;
  packet.data[1] = SYNC_BYTE2;
  packet.data[2] = SYNC_BYTE3;
  packet.data[3] = bleChannel.sent.message_id;
  packet.data[4] = bleChannel.sent.message_length;
  
  for(uint8_t i = 0; i < bleChannel.sent.message_length; i++) {
    packet.data[header_length+i] = bleChannel.sent.message_data[i];
  }

  uint8_t length = header_length + bleChannel.sent.message_length;

  //Serial.print("sending: ");
  //for (int i = 0; i < length; i++) {
  //  Serial.printf("%02X ", packet.data[i]);
  //}
  //Serial.println();

  if (xQueueSendToBack(responseQueue, &packet, pdMS_TO_TICKS(10)) != pdTRUE) {
    //Serial.println("response queue error");
  }
}

void CommandHandler::initTwin_F() {

     // Servo deattach
  if (servo6.attached())  servo6.detach();
  if (servo9.attached())  servo9.detach();
  if (servo10.attached()) servo10.detach();

  if (dc_coding_at_pin_06_ptr)
  {
      delete dc_coding_at_pin_06_ptr;
      dc_coding_at_pin_06_ptr = NULL;
  }

  if (dc_coding_at_pin_09_ptr)
  {
      delete dc_coding_at_pin_09_ptr;
      dc_coding_at_pin_09_ptr = NULL;
  }

  if (dc_coding_at_pin_0A_ptr)
  {
      delete dc_coding_at_pin_0A_ptr;
      dc_coding_at_pin_0A_ptr = NULL;
  }

  
    ////////Stop and deinit double dc motors//////////
  uint8_t stopCommand[] = {0xAA, 0x44, 0x1C, 0x00, 0x00, 0x00, 0x00};
  for(int i = 0; i < 3; i++) {
    if(doubledc != NULL) {
      doubledc->end();  
      delete doubledc;
      doubledc = NULL;
    }

    doubledc = new HardwareSerial(1);
  
    doubledc->begin(19200, SERIAL_8N1, -1,outputpins[i]);

    doubledc->write(stopCommand, sizeof(stopCommand));
    doubledc->flush();
    pinMode(outputpins[i], OUTPUT);
    digitalWrite(outputpins[i], LOW);
    //delay(10);
  }
    
  if (doubledc) {
    doubledc->end();
    delete doubledc;
    doubledc = NULL;
  }
  motor_1_speed=0;
  motor_2_speed=0;

     
  digitalWrite(D_IN_4, LOW);  
  digitalWrite(D_IN_8, LOW);
  digitalWrite(D_IN_12, LOW);

  digitalWrite(AN_IN_4, LOW);
  digitalWrite(AN_IN_8, LOW);
  digitalWrite(AN_IN_12, LOW);

  digitalWrite(TRIG_PIN, LOW); 
  digitalWrite(ECHO_PIN, LOW);
  
  digitalWrite(latchPin, LOW); 
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, LOW);

  digitalWrite(S1pin, LOW); 
  digitalWrite(S2pin, LOW);
  digitalWrite(S3pin, LOW); 
  
  leds[0] = CRGB(0, 0, 0);
  FastLED.show();
  //Serial.println("hello");
  uint8_t pattern[8]= {0};
  if (xQueueSendToBack(patternQueue, &pattern, 0) != pdTRUE) {
          //Serial.println("pattern queue error");
       }

  uint8_t response_data[1] = {1};
  bleChannel.sent.message_length = 1;
  memcpy(bleChannel.sent.message_data, response_data, bleChannel.sent.message_length);
  bleChannel.sent.message_id = INIT_TWIN;
}

  
void CommandHandler::digitalWrite_F() {
  uint8_t pin = getActualPin(bleChannel.received.message_data[0],DIGITAL);         
  pinMode(pin, OUTPUT);
  uint8_t value = bleChannel.received.message_data[1];
  digitalWrite(pin, value);

  bleChannel.sent.message_length = 1;
  uint8_t response_data[1] = {1};
  memcpy(bleChannel.sent.message_data, response_data, bleChannel.sent.message_length);
  bleChannel.sent.message_id = DIGITAL_WRITE;
}


void CommandHandler::digitalRead_F() {

  digitalWrite(chooseSelect(bleChannel.received.message_data[0]), HIGH); 
  uint8_t pin = getActualPin(bleChannel.received.message_data[0],DIGITAL);         
  pinMode(pin, INPUT); 

  uint8_t response_data[2] = {bleChannel.received.message_data[0], digitalRead(pin)};

  bleChannel.sent.message_length = 2;
  memcpy(bleChannel.sent.message_data, response_data, bleChannel.sent.message_length);
  bleChannel.sent.message_id = DIGITAL_READ;
}


void CommandHandler::analogWrite_F(){
  uint8_t pin = getActualPin(bleChannel.received.message_data[0], DIGITAL);
  pinMode(pin, OUTPUT);
  uint16_t value = bleChannel.received.message_data[1];
  //Serial.println(value);
  
  value = map(value,0,255,0,4095) + 1 ; 
  if(value > 4095) value = 4095;

  ledcAttach(pin, freq, resolution);
  ledcWrite(pin, value);
  
  bleChannel.sent.message_length = 1;
  uint8_t response_data[1] = {1};  
  memcpy(bleChannel.sent.message_data, response_data, bleChannel.sent.message_length);
  bleChannel.sent.message_id = ANALOG_WRITE;
}


void CommandHandler::analogRead_F(){
  digitalWrite(chooseSelect(bleChannel.received.message_data[0]), LOW);
  analogReadResolution(10); //set the resolution to 10 bits (0-1023)
  uint8_t pin = getActualPin(bleChannel.received.message_data[0],ANALOG);         
  pinMode(pin, INPUT);
  uint16_t analogValue = analogRead(pin);       
  uint8_t response_data[3] = {bleChannel.received.message_data[0],(uint8_t)(analogValue>> 0),(uint8_t)(analogValue>> 8)};
  bleChannel.sent.message_length = 3;
  memcpy(bleChannel.sent.message_data, response_data, bleChannel.sent.message_length);                 
  bleChannel.sent.message_id = ANALOG_READ; 

}

void CommandHandler::rgbLED_F(){
  
  leds[0] = CRGB(bleChannel.received.message_data[1],
  bleChannel.received.message_data[0],
  bleChannel.received.message_data[2]);
  FastLED.show();


  uint8_t response_data[1] = {1};
  bleChannel.sent.message_length = 1;
  memcpy(bleChannel.sent.message_data, response_data, bleChannel.sent.message_length);                 
  bleChannel.sent.message_id = RGB_LED; 
}

void CommandHandler::playNote_F(){
  uint8_t pin = getActualPin(bleChannel.received.message_data[0], DIGITAL);;
  pinMode(pin, OUTPUT);

  uint16_t note = bleChannel.received.message_data[1] + (bleChannel.received.message_data[2] << 8);
  uint16_t duration = bleChannel.received.message_data[3] + (bleChannel.received.message_data[4] << 8);

  ledcAttach(pin,2700,8);
  ledcWriteTone(pin, note); 
  delay(duration);
  ledcWriteTone(pin, 0);

  bleChannel.sent.message_length = 1;
  uint8_t response_data[1] = {1};  
  memcpy(bleChannel.sent.message_data, response_data, bleChannel.sent.message_length);
  bleChannel.sent.message_id = PLAYNOTE;
}
void CommandHandler::pianoButton_F(){
  uint32_t touch = 0;
  uint8_t response = 0;
  uint8_t pin = bleChannel.received.message_data[0];

  if(pin == 8)
    touch = touchRead(1);
  else if(pin == 9)
    touch = touchRead(2);
  else if(pin == 10)
    touch = touchRead(3);
  else if (pin == 12)
    touch = touchRead(4);

  //Serial.println(touch);

  if (touch> TOUCH_THRESHOLD_2) 
    response = 1;
  else response = 0;

  uint8_t response_data[2] = {pin,response};
  bleChannel.sent.message_length = 2;
  memcpy(bleChannel.sent.message_data, response_data, bleChannel.sent.message_length);                 
  bleChannel.sent.message_id = PIANO_BUTTON;


}

void CommandHandler::touchPad_F(){

  uint8_t response = 0;
  uint32_t touch = touchRead(TOUCH_PIN);
  //Serial.println(touch);

  if (touch> TOUCHPAD_THRESHOLD) 
    response = 1;
  else response = 0;


  uint8_t response_data[1] = {response};
  bleChannel.sent.message_length = 1;
  memcpy(bleChannel.sent.message_data, response_data, bleChannel.sent.message_length);                 
  bleChannel.sent.message_id = TOUCH_PAD;
}
void CommandHandler::hcsr04_F(){

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);

  uint16_t distance = duration * 0.034 / 2;

  //Serial.print(" distance: ");
  //Serial.println(distance);

  if (distance > 400) distance = 400;

  uint8_t response_data[2] = {(uint8_t)(distance >> 0), (uint8_t)(distance >> 8)};
  bleChannel.sent.message_length = 2;
  memcpy(bleChannel.sent.message_data, response_data, bleChannel.sent.message_length);                 
  bleChannel.sent.message_id = HCSR04; 

}
void CommandHandler::servo_F(){
  uint8_t pin = bleChannel.received.message_data[0];
  int degree = bleChannel.received.message_data[1] + (bleChannel.received.message_data[2] << 8) ;
 
  if(pin == 6) {
    if (!servo6.attached()) 
    servo6.attach(getActualPin(pin,DIGITAL));
    servo6.writeMicroseconds(degree);
  }
  else if(pin == 9) {
    if (!servo9.attached()) 
    servo9.attach(getActualPin(pin,DIGITAL));
    servo9.writeMicroseconds(degree);
  }
  else if(pin == 10) {
    if (!servo10.attached()) 
    servo10.attach(getActualPin(pin,DIGITAL));
    servo10.writeMicroseconds(degree);
  }
  

  uint8_t response_data[1] = {1};
  bleChannel.sent.message_length = 1;
  memcpy(bleChannel.sent.message_data, response_data, bleChannel.sent.message_length);                 
  bleChannel.sent.message_id = SERVO; 
}

void CommandHandler::tempAndhum_F(){
  digitalWrite(chooseSelect(bleChannel.received.message_data[0]), HIGH);
  uint8_t pin = getActualPin(bleChannel.received.message_data[0],DIGITAL);
  DHT11 dht11(pin);   
  int temperature = 0;
  int humidity = 0;

    // Attempt to read the temperature and humidity values from the DHT11 sensor.
    int result = dht11.readTemperatureHumidity(temperature, humidity);
     //if (result == 0) {
        //Serial.print("Temperature: ");
        //Serial.print(temperature);
        //Serial.print(" °C\tHumidity: ");
        //Serial.print(humidity);
        //Serial.println(" %");
    //} else {
        // Print error message based on the error code.
        //Serial.println(DHT11::getErrorString(result));
    //}
  

  uint8_t response_data[3] = {bleChannel.received.message_data[0],(uint8_t)temperature,(uint8_t)humidity};
  bleChannel.sent.message_length = 3;
  memcpy(bleChannel.sent.message_data, response_data, bleChannel.sent.message_length);                 
  bleChannel.sent.message_id = TEMPRATUREHUMIDITY; 

}

void CommandHandler::singleDC_F(){
  

  uint8_t motor_dir = (bleChannel.received.message_data[2] >> 7) & 1;
  uint8_t motor_speed = (bleChannel.received.message_data[2] & 0x7f);
  Serial.println(motor_dir);
        if (bleChannel.received.message_data[1] == 0x06)
        {
            if (!dc_coding_at_pin_06_ptr)
            {
                delete dc_coding_at_pin_06_ptr;
                dc_coding_at_pin_06_ptr = new TwinDCMotor(1, D6_motor_pin);
            }
            //Serial.println(motor_dir);
            //Serial.println(motor_speed);

            
            dc_coding_at_pin_06_ptr->setDirAndSpeedCoding(0, motor_dir, motor_speed);
         
        }
        else if (bleChannel.received.message_data[1] == 0x09)
        {
            if (!dc_coding_at_pin_09_ptr)
            {
                delete dc_coding_at_pin_09_ptr;
                dc_coding_at_pin_09_ptr = new TwinDCMotor(1, D9_motor_pin);
            }
              
              dc_coding_at_pin_09_ptr->setDirAndSpeedCoding(0, motor_dir, motor_speed);
              
        }
        else if (bleChannel.received.message_data[1] == 0x0A)
        {
            if (!dc_coding_at_pin_0A_ptr)
            {
                delete dc_coding_at_pin_0A_ptr;
                dc_coding_at_pin_0A_ptr = new TwinDCMotor(1, D10_motor_pin);
            }
            //Serial.println(motor_dir);
            //Serial.println(motor_speed);
              
              dc_coding_at_pin_0A_ptr->setDirAndSpeedCoding(0, motor_dir, motor_speed);
                 
        }
    
  
 
  
  bleChannel.sent.message_length = 1;
  uint8_t response_data[1] = {1};  
  memcpy(bleChannel.sent.message_data, response_data, bleChannel.sent.message_length);
  bleChannel.sent.message_id = SINGLE_DC;
  
}
void  CommandHandler::doubleDC_F(){


  if(doubledc != NULL) delete doubledc;

  doubledc = new HardwareSerial(1);

  doubledc->begin(19200, SERIAL_8N1, -1,getActualPin(bleChannel.received.message_data[0],DIGITAL));


  if (bleChannel.received.message_data[1] == 0x01){
    motor_1_dir = (bleChannel.received.message_data[2] >> 7) & 1 ;
    motor_1_speed = bleChannel.received.message_data[2] & 0x7f ;
  }
  else if(bleChannel.received.message_data[1] == 0x02){  
    motor_2_dir = (bleChannel.received.message_data[2] >> 7) & 1 ;
    motor_2_speed = bleChannel.received.message_data[2] & 0x7f ;
  }
  else if(bleChannel.received.message_data[1] == 0x03){
    motor_1_dir = (bleChannel.received.message_data[2] >> 7) & 1 ;
    motor_1_speed = bleChannel.received.message_data[2] & 0x7f ;
    motor_2_dir = motor_1_dir ;
    motor_2_speed = motor_1_speed ;
  }

  if(doubledc != NULL) 
    {
      doubledc->write(0xAA); // 3 senkron byte ı degisebilir, random
      doubledc->write(0x44);
      doubledc->write(0x1C);
      doubledc->write(motor_1_dir);
      doubledc->write(motor_1_speed);
      doubledc->write(motor_2_dir);
      doubledc->write(motor_2_speed);   
    }
  
  doubledc->flush();
  pinMode(getActualPin(bleChannel.received.message_data[0],DIGITAL), OUTPUT);
  digitalWrite(getActualPin(bleChannel.received.message_data[0],DIGITAL), LOW);

  uint8_t response_data[1] = {1};
  bleChannel.sent.message_length = 1;   
  memcpy(bleChannel.sent.message_data, response_data, bleChannel.sent.message_length);             
  bleChannel.sent.message_id = DOUBLE_DC;

}
void CommandHandler::getfirmwareVersion_F() {
  uint8_t response_data[2] = {FIRMWARE_VERSION_MINOR, FIRMWARE_VERSION_MAJOR};
  bleChannel.sent.message_length = 2;
  memcpy(bleChannel.sent.message_data, response_data, bleChannel.sent.message_length);
  bleChannel.sent.message_id = GETFIRMWAREVERSION;
}

void CommandHandler::ledMatrix_F(){
  uint8_t pattern[8]= {0};
  //static uint8_t lastpattern[8] = {0};
  memcpy(pattern, bleChannel.received.message_data, sizeof(pattern));

  //if(memcmp(pattern,lastpattern,sizeof(pattern))!=0)
      if (xQueueSendToBack(patternQueue, &pattern, pdMS_TO_TICKS(10)) != pdTRUE) {
          //Serial.println("pattern queue error");  
    
  }
  
  //memcpy(lastpattern, pattern, sizeof(lastpattern));
  uint8_t response_data[1] = {1};
  bleChannel.sent.message_length = 1;
  memcpy(bleChannel.sent.message_data, response_data, bleChannel.sent.message_length);                 
  bleChannel.sent.message_id = LED_MATRIX; 
}

uint8_t CommandHandler::chooseSelect(uint8_t pin) {
  
  switch (pin){
    case 4 : return S1pin;
    case 6 : return S1pin;
    case 8 :return S2pin;
    case 11 :return S3pin;
    case  12: return S3pin;
  }
}

uint8_t CommandHandler::getActualPin(uint8_t logicalPin,  bool signal_type) {
    switch (logicalPin) {
      
        case 6: 
          if(signal_type == DIGITAL) return D6_motor_pin;
          else if(signal_type == ANALOG) return AN_IN_4;
        
        case 9: return D9_motor_pin;
        case 10: return D10_motor_pin;

        //input pins
        case 4: return D_IN_4;
          
        case 8: 
        
          if(signal_type == DIGITAL) return D_IN_8;
          else if(signal_type == ANALOG) return AN_IN_8;

        case 11: return AN_IN_12;
        case 12: return D_IN_12;
        
        // Unmapped pins remain unchanged
        default: return logicalPin; 
    }
}

uint8_t CommandHandler::reverseBits(uint8_t b) {
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return b;
}
void CommandHandler::shiftOutAll(uint8_t colData, uint8_t rowData) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, colData);
  shiftOut(dataPin, clockPin, MSBFIRST, rowData);
  digitalWrite(latchPin, HIGH);
}
