#include "ble_task.h"
#include "serial_task.h"
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <string>
#include "uuids.h"

const char *ble_current_message;
unsigned long ble_timestamp;




struct RaveDeviceBLE
{
    BLEService *service;

    BLECharacteristic *char_name;
    BLECharacteristic *char_message;
    BLECharacteristic *char_location;
    BLECharacteristic *char_timestamp;

} ble_device;

RaveDeviceBLE _ble_device_mine;
RaveDeviceBLE _ble_devices[8];
BLEAdvertising *_ble_advertising;
BLEServer *_ble_server;

xQueueHandle _ble_newDeviceQueue;
xQueueHandle _ble_messageQueue;

int _ble_device_indx = 0;

void task_ble_init(void *pvParameters)
{

    _ble_newDeviceQueue = xQueueCreate(8, sizeof(int));
    _ble_messageQueue = xQueueCreate(16, sizeof(char *));

    serial_print("Task/BLE: Init");
    BLEDevice::init((String("RaveBuddy #") + String(state_get_mine()->id)).c_str());
    

    serial_print("Task/BLE: Creating BLE Server");
    _ble_server = BLEDevice::createServer();
        serial_print("Task/BLE: Creating BLE Advertising");
    _ble_advertising = BLEDevice::getAdvertising();
    _ble_advertising->setScanResponse(true);
    _ble_advertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    _ble_advertising->setMinPreferred(0x12);
    serial_print("Task/BLE: Creating BLE Self Service and charisstic");

    RaveDeviceState *myState = state_get_mine();
    RaveDeviceBLE newDevice = {0};

    const char* myuuid = "dbd6cac0-e07a-4db8-8851-9c831374fc53";

    newDevice.service = _ble_server->createService(myuuid);

    newDevice.char_name = newDevice.service->createCharacteristic(uuid_get(), BLECharacteristic::PROPERTY_READ);
    String naname = "RaveBuddy #";
    naname += String(myState->id);
    newDevice.char_name->setValue(naname.c_str());

    // Create Message Char
    // serial_print("Task/BLE: Creaint message charaistic");
    newDevice.char_message = newDevice.service->createCharacteristic(uuid_get(), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    newDevice.char_message->setValue("");

    // Create Location Char
    // serial_print("Task/BLE: Creaint location charaistic");
    newDevice.char_location = newDevice.service->createCharacteristic(uuid_get(), BLECharacteristic::PROPERTY_READ);
    newDevice.char_location->setValue("");

    // Create Timestamp Char
    // serial_print("Task/BLE: Creaint timestamp charaistic");
    newDevice.char_timestamp = newDevice.service->createCharacteristic(uuid_get(), BLECharacteristic::PROPERTY_READ);
    newDevice.char_timestamp->setValue("");

    serial_print("Task/BLE: Advertising");
    newDevice.service->start();
    _ble_device_mine =  newDevice;
    _ble_advertising->addServiceUUID(myuuid);

    BLEDevice::startAdvertising();

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

void ble_sync_state(RaveDeviceState *state)
{
    // add state to queue
    xQueueSend(_ble_messageQueue, &state, 0);
}

void ble_read_mine(){
    RaveDeviceState *myState = state_get_mine();
    if(myState != nullptr){
        myState->state = String(_ble_device_mine.char_message->getValue().c_str());
    }else{
        serial_print("Task/BLE: Error getting state");
    }
    //
}

void task_ble_tick()
{

    // check for new devices
    ble_read_mine();
    int idToAdd;
    if (xQueueReceive(_ble_newDeviceQueue, &idToAdd, 100 / portTICK_PERIOD_MS) != pdTRUE)
    {
        // Serial.println("Task/BLE: No new device / failed queue");
    }
    else
    {
        const int id = idToAdd;
        const char *serviceID = uuid_get();
        Serial.printf("BLE: New device %d - %s\n", id, serviceID);

        RaveDeviceBLE newDevice = {0};
        newDevice.service = _ble_server->createService(serviceID);

        // serial_print("Task/BLE: Creaint name charaistic");

        newDevice.char_name = newDevice.service->createCharacteristic(uuid_get(), BLECharacteristic::PROPERTY_READ);
        String naname = "RaveBuddy #";
        naname += String(id);
        newDevice.char_name->setValue(naname.c_str());

        newDevice.char_message = newDevice.service->createCharacteristic(uuid_get(), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
        newDevice.char_message->setValue("");

        newDevice.char_location = newDevice.service->createCharacteristic(uuid_get(), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
        newDevice.char_location->setValue("");

        newDevice.char_timestamp = newDevice.service->createCharacteristic(uuid_get(), BLECharacteristic::PROPERTY_READ );
        newDevice.char_timestamp->setValue("");

        serial_print("Task/BLE: Advertising");
        newDevice.service->start();
        _ble_advertising->addServiceUUID(serviceID);

        //BLEDevice::startAdvertising();
        _ble_devices[id] = newDevice;
    }
    // Check for new state
    RaveDeviceState *state;

    if (xQueueReceive(_ble_messageQueue, &state, 100 / portTICK_PERIOD_MS) != pdTRUE)
    {
    }
    else
    {
        if (state->mine)
        {
            _ble_device_mine.char_message->setValue((String("MESSAGE:" + state->state)).c_str());
            _ble_device_mine.char_timestamp->setValue((String("TIMESTAMP:" + state->timestamp)).c_str());
        }
        else
        {
            _ble_devices[state->id].char_message->setValue((String("MESSAGE:" + state->state)).c_str());
            _ble_devices[state->id].char_timestamp->setValue((String("TIMESTAMP:" + state->timestamp)).c_str());
        }
        // update state
    }

}