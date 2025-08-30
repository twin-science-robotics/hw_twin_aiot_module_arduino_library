#include "BLEManager.h"

//constructor  : initialization list
BLEManager::BLEManager() : deviceConnected(false), pServer(nullptr), pCharacteristic(nullptr) {
}

void BLEManager::setup(const String& moduleName) {
  // // Initialize BLE
  BLEDevice::init(moduleName);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks(this));
  
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE_NR |
    BLECharacteristic::PROPERTY_NOTIFY
  );
    
  //BLEDevice::setMTU(23);

  pCharacteristic->setCallbacks(new MyCharacteristicCallbacks(bleQueue));
  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setValue("0"); 
  pService->start();
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x06);
 
  BLEDevice::startAdvertising();
  
  //Serial.println("adverstising");
}


bool BLEManager::isConnected() {
  return deviceConnected;
}

void BLEManager::sendResponse(Packet packet) {

  uint8_t packet_length = packet.data[4] + header_length;

  if (deviceConnected) {
    pCharacteristic->setValue(packet.data, packet_length);
    pCharacteristic->notify();
  }
}

// ServerCallbacks class implementation
BLEManager::MyServerCallbacks::MyServerCallbacks(BLEManager* mgr) : manager(mgr) {
}

void BLEManager::MyServerCallbacks::onConnect(BLEServer* pServer) {
  manager->deviceConnected = true;
  //Serial.println("device connected");
}

void BLEManager::MyServerCallbacks::onDisconnect(BLEServer* pServer) {
  manager->deviceConnected = false;
  manager->pCharacteristic->setValue("0");
  pServer->startAdvertising();
  //Serial.println("disconnected, starting advertising");
}


// MyCharacteristicCallbacks class implementation
BLEManager::MyCharacteristicCallbacks::MyCharacteristicCallbacks(QueueHandle_t queue) : bleQueue(queue) {
}

void BLEManager::MyCharacteristicCallbacks::onWrite(BLECharacteristic *pChar) {
  //TickType_t xLastWakeTime = xTaskGetTickCount();
  //const TickType_t xFrequency = pdMS_TO_TICKS(1); 
  
  Packet cmd;
  String value = pChar->getValue();

  if(value.length() > 0) {
    size_t copyLength = min(value.length(), sizeof(cmd.data));
    memcpy(cmd.data, value.c_str(), copyLength);
    if (xQueueSendToBack(bleQueue, &cmd, 0) != pdTRUE) {
      //Serial.println("blequeue error");
    }
    //vTaskDelayUntil(&xLastWakeTime, xFrequency);
    vTaskDelay(pdMS_TO_TICKS(2));
  }  
}








