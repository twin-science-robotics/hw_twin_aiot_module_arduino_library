/*
  Twin_CodeLab v2.4.3

  Bu yazılım Twin CodeLab Web (https://code.twinscience.com/) ya da mobil uygulamaları ile blok kodlama yapabilme imkanı sunan yazılımdır.
  
  Arduino IDE üzerinden Twin AIoT modülüne gömülen herhangi bir arduino kodundan sonra Twin Codelab ile blok kodlama yapmak isterseniz bu yazılımı yükleniz gereklidir.

  Temel olarak bu kod Twin Codelab uygulamalarını çalıştıran donanımlar (bilgisayar, tablet, telefon vb) ile Twin AIoT modülü arasında, Bluetooth bağlantısı üzerinden veri alışverişi yapar ve blok kodlarını çalıştırır.

  https://github.com/twin-science-robotics/hw_twin_aiot_module_arduino_library

  24 Eylül 2025
  Twin Science & Robotics
 
*/
#include <Arduino.h>
#include "Constants.h"
#include "BLEManager.h"
#include "CommandHandler.h"

// Modüle güç verildiğinde beliren Twin Science logo animasyonu
uint8_t twinlogo[][8]= {
  {0x38, 0x44, 0xBA, 0x82, 0xBA, 0x44, 0x38, 0x00}, // Orijinal
  {0x00, 0x38, 0x44, 0xBA, 0x82, 0xBA, 0x44, 0x38}, // Aşağı
  {0x00, 0x1C, 0x22, 0x5D, 0x41, 0x5D, 0x22, 0x1C}, // Sağa
  {0x1C, 0x22, 0x5D, 0x41, 0x5D, 0x22, 0x1C, 0x00}, // Yukarı
  {0x38, 0x44, 0xBA, 0x82, 0xBA, 0x44, 0x38, 0x00},  // Sola (başlangıç)
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};


// Modül ismi (örnek : Twin RGB)
String moduleName;

QueueHandle_t patternQueue;
portMUX_TYPE ledmatrix_mux = portMUX_INITIALIZER_UNLOCKED;
// Modül setup 
void setupTwin_F() {


  
  pinMode(D6_output_pin, OUTPUT);  // D6
  
  pinMode(D9_output_pin, OUTPUT);  // D6

  pinMode(D10_output_pin, OUTPUT);  // D6

  pinMode(S1pin, OUTPUT); 
  pinMode(S2pin, OUTPUT); 
  pinMode(S3pin, OUTPUT);

  pinMode(D_IN_4, INPUT);   
  

  pinMode(D_IN_8, INPUT);   
  

  pinMode(D_IN_12, INPUT);   
  

  pinMode(AN_IN_4, INPUT);   
  

  pinMode(AN_IN_8, INPUT);   
  

  pinMode(AN_IN_12, INPUT);   
  

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  FastLED.addLeds<WS2812B,RGB_PIN , RGB>(leds, 1);
  FastLED.setBrightness(64);
  
}

BLEManager bleManager;
CommandHandler commandHandler;

QueueHandle_t bleQueue;
QueueHandle_t responseQueue;

channel_st bleChannel;
CRGB leds[NUM_LEDS];

// Bluetooth ile alınan veriler işlenir
void processingTask(void *pvParameters) {
  Packet receivedCmd;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(10); 
  
  while(1) {
    if (xQueueReceive(bleQueue, &receivedCmd, pdMS_TO_TICKS(100)) == pdTRUE) {     
      commandHandler.handleCommand(receivedCmd);     
    }
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

// Veri işlendikten sonra cevap dönülür (sensör verileri gibi)
void responseTask(void *pvParameters) {
  Packet packet;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(10); 
  
  while(1) {
    vTaskDelayUntil(&xLastWakeTime, xFrequency);

    if (xQueueReceive(responseQueue, &packet, pdMS_TO_TICKS(10)) == pdTRUE) {
      bleManager.sendResponse(packet);
    }
  }
}

// Led matrixi yenileyen fonksiyon
void ledMatrixTask(void *pvParameters) {

  uint8_t buffer[8] = {0};

  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS(10); 
  
  xLastWakeTime = xTaskGetTickCount();

   //uint8_t brightness = 15; 
   //uint16_t onTime = (brightness + 1) * 16;  // 16-256 mikrosaniye
 
  while(1) { 
     
    if (xQueueReceive(patternQueue, &buffer,0) != pdTRUE) {
      //error
     
    }
    
    for (int row = 0; row < 8; row++) {
        uint8_t patternRow = buffer[row];
        uint8_t reversedPattern = commandHandler.reverseBits(patternRow);
        uint8_t rowData = (1 << row);
        uint8_t colData = ~reversedPattern;

        portENTER_CRITICAL(&ledmatrix_mux);
        commandHandler.shiftOutAll(colData, rowData);
        portEXIT_CRITICAL(&ledmatrix_mux);

        vTaskDelay(pdMS_TO_TICKS(1));
        //delayMicroseconds(190);

        portENTER_CRITICAL(&ledmatrix_mux);
        commandHandler.shiftOutAll(0x00, 0x00);
        portEXIT_CRITICAL(&ledmatrix_mux);
        //vTaskDelay(pdMS_TO_TICKS(1));
        //delayMicroseconds(10);
        
      }
        

    
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    //vTaskDelay(pdMS_TO_TICKS(5));
  
  }
}

// Bluetooth bağlantı durumunu periyodik olarak kontrol eden fonksiyon
void connTask(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(1000);

  bool prevConnectionState = false; 
  bool currentConnectionState = false;

  while(1) {

    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    currentConnectionState = bleManager.isConnected();
    //Serial.println(currentConnectionState);
    
    if(currentConnectionState != prevConnectionState) {
      if(currentConnectionState){
        //Serial.println("device connected");
        commandHandler.initTwin_F();
      }
      else {
        //Serial.println("disconnected");
        commandHandler.initTwin_F();
      }
    
      prevConnectionState = currentConnectionState;
    }
  }
}

// Bluetooth bağlantı durumunu RGB led'de belirten fonksiyon
void bleblinkTask(void *pvParameters) {

  while(1) {

    if(bleManager.isConnected() == false){
      //Serial.println("led is on");

      leds[0] = CRGB(0,0,255);
      FastLED.show();
      vTaskDelay(pdMS_TO_TICKS(500));
      //Serial.println("led is off");
      leds[0] = CRGB(0,0,0);
      FastLED.show();
      vTaskDelay(pdMS_TO_TICKS(500));
      
    }


  vTaskDelay(pdMS_TO_TICKS(80));
  } 

}

// Modül ismini led matrixde kaydıran fonksiyon
void scrollIDTask(void *pvParameters) {

  int offset = 0; 

  while(1) {

    if(bleManager.isConnected() == false){
      
      scrollID(offset);
      offset++;
      
    }


  vTaskDelay(pdMS_TO_TICKS(80)); // scroll speed
  } 

}


void scrollID(int offset){

  String scrollText = " " + moduleName; 
  uint8_t textLen = scrollText.length();
  const int charWidth = 8;
  const int gapWidth = 0;

  int compositeLength = textLen * (charWidth + gapWidth);

  uint8_t compositePattern[compositeLength] = {0};
  int pos = 0;
  
  for (size_t i = 0; i < textLen && pos < compositeLength; i++) {
    char c = scrollText[i];
    const uint8_t* letterPattern = getPatternForLetter(c);
    for (int col = 0; col < charWidth && pos < compositeLength; col++) {
      uint8_t colByte = 0;
      for (int row = 0; row < 8; row++) {
        if (letterPattern[row] & (1 << (7 - col))) {
          colByte |= (1 << row);
        }
      }
      compositePattern[pos++] = colByte;
    }

    for (int g = 0; g < gapWidth && pos < compositeLength; g++) {
      compositePattern[pos++] = 0x00;
    }
  }

  const int displayWidth = 8;

   uint8_t window[8];

    for (int i = 0; i < displayWidth; i++) {
      window[i] = compositePattern[(offset + i) % compositeLength];
      
    }
    uint8_t rowData[8] = {0};
    for (int r = 0; r < 8; r++) {
      uint8_t rowByte = 0;
      for (int c = 0; c < 8; c++) {
        uint8_t bit = (window[c] >> r) & 1;
        rowByte |= (bit << (7 - c));
      }
      rowData[r] = rowByte;
    }
    if (xQueueSendToBack(patternQueue, &rowData, pdMS_TO_TICKS(10)) != pdTRUE) {
          //Serial.println("id queue error");    
      }



}



void setup() {
  //Serial.begin(115200);
  
  
  // Kuyrukları oluştur
  bleQueue = xQueueCreate(QUEUE_LENGTH, sizeof(Packet));
  responseQueue = xQueueCreate(QUEUE_LENGTH, sizeof(Packet));
  patternQueue = xQueueCreate(QUEUE_LENGTH, sizeof(Packet));
  moduleName = calculateModuleName() ;

 //Setups
  bleManager.setup(moduleName);
  setupTwin_F();
  commandHandler.initTwin_F();
  
  // Görevleri oluştur
  xTaskCreate(processingTask, "ProcessingTask", 4096, NULL, 3, NULL);
  xTaskCreate(responseTask, "ResponseTask", 4096, NULL, 3, NULL);
  xTaskCreate(ledMatrixTask, "ledmatrixTask", 4096, NULL, 1, NULL);
  LogoAnimate();
  xTaskCreate(connTask, "ConnectionTask", 4096, NULL, 1, NULL);
  xTaskCreate(bleblinkTask, "bleblinkTask", 4096, NULL, 1, NULL);
  xTaskCreate(scrollIDTask, "scrollIDTask", 4096, NULL, 1, NULL);

  
  //Serial.println(moduleName);
}

void loop() {
  vTaskDelete(NULL);  
}

//Twin logo animasyonu
void LogoAnimate(){

  uint8_t numFrames = 6 ;

  for(uint8_t i = 0; i<numFrames ; i++){
    if (xQueueSendToBack(patternQueue, &twinlogo[i], pdMS_TO_TICKS(10)) != pdTRUE) {
          //Serial.println("logo queue error");    
  }

  vTaskDelay(pdMS_TO_TICKS(300)); // approximately 2 sec animation
  }

}


const uint8_t* getPatternForLetter(char c) {

  for (size_t i = 0; i < numLetters; i++) {
    if (ledMatrixLetters[i].letter == c) {
      return ledMatrixLetters[i].pattern;
    }
  }
  return ledMatrixLetters[numLetters - 1].pattern;
}


// Unique name calculation on mac address
String calculateModuleName() {
  uint64_t mac = ESP.getEfuseMac();

  //Serial.print("MAC Address: ");
  //Serial.println(mac);

  uint8_t macBytes[6];

  for (int i = 0; i < 6; i++) {
    macBytes[i] = (mac >> (8 * i)) & 0xFF;
  }

  uint16_t sum = 0;
  for (int i = 0; i < 6; i++) {
    sum += macBytes[i];
  }

  uint16_t modVal = sum % 216;  
  //Serial.print("Mod Value (sum % 216): ");
  //Serial.println(modVal);

  String colorNames[] = {"R", "G", "B", "W", "O", "P"};
  
  return "Twin " + colorNames[modVal / 36] + colorNames[(modVal % 36) / 6] + colorNames[modVal % 6];
}

