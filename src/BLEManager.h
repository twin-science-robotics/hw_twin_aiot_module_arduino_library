#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "Constants.h"

struct message_st {
	uint8_t	  message_length ;
	uint8_t	  message_id ;
	uint8_t	  message_data[data_length];
};

struct channel_st {
	message_st received;
	message_st sent;
};

struct Packet {
  uint8_t data[packet_size];
};


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
extern channel_st bleChannel;
extern QueueHandle_t bleQueue;
extern QueueHandle_t responseQueue;
 

#endif // BLE_MANAGER_H