#include "ble_task.h"
#include "serial_task.h"
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <string>

const char *ble_current_message;
unsigned long ble_timestamp;

#define CHAR_NAME_UUID "19cb9cd1-3ebd-4ea4-96e2-4b12ed721c7e"
#define CHAR_MESSAGE_UUID "3278a397-7a0c-4d6d-8795-081dc09a2f4d"
#define CHAR_LOCATION_UUID "bf4ec14e-9641-4dae-9b13-9285975c012f"
#define CHAR_TIMESTAMP_UUID "a85c631e-c128-4fd2-9215-306bbfd218c6"

const char *_ble_char_uuids[] = {
    "93923064-f91d-417f-be11-0dd6127faf94",
    "70213a12-8fb2-4778-b142-47195ba48470",
    "d985c603-c89f-449b-8853-5a122714a08a",
    "378fc3ae-a812-44af-ac4f-080a5cea4d5d",
    "cf17577d-79f4-4e0f-ad2e-4658e134508d",
};

struct RaveDeviceBLE
{
    BLEService *service;
    BLECharacteristic *char_name;
    BLECharacteristic *char_message;
    BLECharacteristic *char_location;
    BLECharacteristic *char_timestamp;
} ble_device;

RaveDeviceBLE _ble_devices[8];
BLEAdvertising *_ble_advertising;
BLEServer *_ble_server;

xQueueHandle _ble_newDeviceQueue;
xQueueHandle _ble_messageQueue;

int _ble_device_indx = 0;

void task_ble_init(void *pvParameters)
{
    
    _ble_newDeviceQueue = xQueueCreate(8, sizeof(int));
    _ble_messageQueue = xQueueCreate(16, sizeof(char*));

    serial_print("Task/BLE: Init");
    BLEDevice::init("RaveBuddy #001");

    serial_print("Task/BLE: Creating BLE Server");
    _ble_server = BLEDevice::createServer();

    serial_print("Task/BLE: Creating BLE Advertising");
    _ble_advertising = BLEDevice::getAdvertising();
    _ble_advertising->setScanResponse(true);
    _ble_advertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    _ble_advertising->setMinPreferred(0x12);
    serial_print("Task/BLE: Starting tick loop");

    for (;;)
    {
        task_ble_tick();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void ble_new_device(int id)
{
    xQueueSend(_ble_newDeviceQueue, &id, 0);
}

void ble_sync_state(RaveDeviceState* state){
    //add state to queue
    xQueueSend(_ble_messageQueue, &state, 0);
}

void task_ble_tick()
{

    //check for new devices

    int idToAdd;
    if (xQueueReceive(_ble_newDeviceQueue, &idToAdd, 100 / portTICK_PERIOD_MS) != pdTRUE)
    {
        //Serial.println("Task/BLE: No new device / failed queue");
        
    }
    else
    {
        const int id = idToAdd;
        const char *serviceID = _ble_char_uuids[id];
        Serial.printf("BLE: New device %d - %s\n", id, serviceID);
      
        RaveDeviceBLE newDevice = {0};
        newDevice.service = _ble_server->createService(serviceID);

       // serial_print("Task/BLE: Creaint name charaistic");

        newDevice.char_name = newDevice.service->createCharacteristic(CHAR_NAME_UUID, BLECharacteristic::PROPERTY_READ);
        String naname = "RaveBuddy #";
        naname += String(id);
        newDevice.char_name->setValue(naname.c_str());

        //Create Message Char
        //serial_print("Task/BLE: Creaint message charaistic");
        newDevice.char_message = newDevice.service->createCharacteristic(CHAR_MESSAGE_UUID, BLECharacteristic::PROPERTY_READ);
        newDevice.char_message->setValue("");

        //Create Location Char
        //serial_print("Task/BLE: Creaint location charaistic");
        newDevice.char_location = newDevice.service->createCharacteristic(CHAR_LOCATION_UUID, BLECharacteristic::PROPERTY_READ);
        newDevice.char_location->setValue("");

        //Create Timestamp Char
        //serial_print("Task/BLE: Creaint timestamp charaistic");
        newDevice.char_timestamp = newDevice.service->createCharacteristic(CHAR_TIMESTAMP_UUID, BLECharacteristic::PROPERTY_READ);
        newDevice.char_timestamp->setValue("");



        serial_print("Task/BLE: Advertising");
        newDevice.service->start();
        _ble_advertising->addServiceUUID(serviceID);

        BLEDevice::startAdvertising();
        _ble_devices[id] = newDevice;
    }
    //Check for new state
    RaveDeviceState* state;

    if(xQueueReceive(_ble_messageQueue, &state,100/portTICK_PERIOD_MS) != pdTRUE){
       
    }
    else{
        //update state
        Serial.printf("BLE: New state %d\n", state->id);
        _ble_devices[state->id].char_message->setValue(state->state.c_str());
        _ble_devices[state->id].char_timestamp->setValue(String(state->timestamp).c_str());
    }
}