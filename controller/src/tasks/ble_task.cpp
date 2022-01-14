#include "ble_task.h"
#include "serial_task.h"
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <string>
const char *ble_current_message;
unsigned long ble_timestamp;
#define SERVICE_UUID "f90cf582-2cbe-4591-ae8c-f4a72e93697f"
#define CHAR_TIMESTAMP_UUID "19cb9cd1-3ebd-4ea4-96e2-4b12ed721c7e"


BLECharacteristic* _ble_charaistic_timestamp = nullptr;
void task_ble_init(void *pvParameters)
{
    
    serial_print("Task/BLE: Init");
    BLEDevice::init("RaveBuddy #000");

    serial_print("Task/BLE: Creating BLE Server and Service");
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pService->

    serial_print("Task/BLE: Creaint timestamp charaistic");
    _ble_charaistic_timestamp = pService->createCharacteristic(
        CHAR_TIMESTAMP_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE);
    ble_timestamp = millis();
    serial_print("Task/BLE: Setting timestamp");
    _ble_charaistic_timestamp->setValue("000");
    pService->start();
    serial_print("Task/BLE: Advertising");
    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    serial_print("Task/BLE: Starting tick loop");
    for (;;)
    {
        task_ble_tick();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
void task_ble_tick()
{
    ble_timestamp = millis();
    _ble_charaistic_timestamp->setValue(String(ble_timestamp).c_str());
}