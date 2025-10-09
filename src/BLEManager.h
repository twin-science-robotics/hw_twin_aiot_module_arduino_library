#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "Constants.h"

class BLEManager {
public:
  BLEManager();
  void setup(const String& moduleName);
  bool isConnected();
  void sendResponse(Packet packet);
  BLEServer* pServer;
  BLECharacteristic* pCharacteristic;
  
private:
  
  bool deviceConnected;
  
  class MyServerCallbacks : public BLEServerCallbacks {
  public:
    MyServerCallbacks(BLEManager* manager);
    void onConnect(BLEServer* pServer) override;
    void onDisconnect(BLEServer* pServer) override;
  private:
    BLEManager* manager;
  };
  
  class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
  public:
    MyCharacteristicCallbacks(QueueHandle_t queue);
    void onWrite(BLECharacteristic* pChar) override;
  private:
    QueueHandle_t bleQueue;
  };
};

extern BLEManager bleManager;
extern channel_st channel;
extern QueueHandle_t commandQueue;
extern QueueHandle_t responseQueue;
 

#endif // BLE_MANAGER_H