#include "systems/BLESystem.hpp"

using namespace RaveBuddy;

BLESystem::BLESystem(){
    
    LogController::logMessage("BLE: Starting");
    
    xTaskCreatePinnedToCore(this->initTask, "System/Lora/Recv", 2048 * 6, this, 8, NULL, 0);
}

void BLESystem::initTask(void *t_this){
    
    LogController::logMessage("BLE: Task Started");
    BLESystem* self = (BLESystem*)t_this;
;
    self->bleStart();

    self->m_stateQueue = xQueueCreate(8, sizeof(State));
    if (!self->m_stateQueue)
    {
        LogController::logMessage("!!!BLE: Queue fail");
        return;
    }


    for (;;)
    {
        self->tickTask();
        vTaskDelay(SYSTEM_BLE_TASK_DELAY / portTICK_PERIOD_MS);
        // Call tickSendTask at every interval defined by SYSTEM_LORA_SEND_DELAY
      
    }
}

void BLESystem::tickTask(){
    
    while (xQueueReceive(m_stateQueue, &buffer, 50 / portTICK_PERIOD_MS) == pdTRUE)
    {
        //LogController::logMessage("BLE: Queue Found!");
       
        if (xSemaphoreTake(m_bleMutex, 500 / portTICK_PERIOD_MS) != pdTRUE)
        {
            LogController::logMessage("!!BLE: Mutex Fail");
            return;
        }

        if(m_units.find(buffer.getId()) == m_units.end()){
            //LogController::logMessage("BLE: Proccessing State");
            bleNew(buffer);
        }

        //LogController::logMessage("BLE: Updating: " + TO_STRING(buffer.getId()));
        bleUpdate(buffer);
        
        xSemaphoreGive(m_bleMutex);
    }

}

void BLESystem::bleStart()
{
     if (xSemaphoreTake(m_bleMutex, portMAX_DELAY) != pdTRUE)
    {
        LogController::logMessage("!!BLE: Mutex Fail");
        return;
    }
    LogController::logMessage("BLE: Creating BLE Server");
    BLEDevice::init("RaveBuddy");

    while(!BLEDevice::getInitialized()){
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    m_server = BLEDevice::createServer();
    
    LogController::logMessage("BLE: Starting Advertising");
    m_advertising = BLEDevice::getAdvertising();
    m_advertising->setScanResponse(true);
    m_advertising->setMinPreferred(0x06);
    m_advertising->setMinPreferred(0x12);

   
    BLEDevice::startAdvertising();
    xSemaphoreGive(m_bleMutex);
    LogController::logMessage("BLE: Done");
   

}

void BLESystem::bleNew(State &t_state)
{

    LogController::logMessage("BLE: DeviceCreate: " + TO_STRING(t_state.getId()));

    BLEUnit newUnit = {0};
    newUnit.service = m_server->createService(uuid_get());

    newUnit.characteristics["ID"] = newUnit.service->createCharacteristic(
        uuid_get(),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );

    newUnit.characteristics["NAME"] = newUnit.service->createCharacteristic(
        uuid_get(),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );

    newUnit.characteristics["GPS_LOC"] = newUnit.service->createCharacteristic(
        uuid_get(),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );

    newUnit.characteristics["GPS_SATS"] = newUnit.service->createCharacteristic(
        uuid_get(),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );

    newUnit.characteristics["STATUS"] = newUnit.service->createCharacteristic(
        uuid_get(),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );

    newUnit.characteristics["TIMESTAMP"] = newUnit.service->createCharacteristic(
        uuid_get(),
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    );
    newUnit.service->start();
    m_advertising->addServiceUUID(uuid_get());

    m_units[t_state.getId()] = newUnit;
    LogController::logMessage("BLE: Device Started: " + TO_STRING(t_state.getId()));
}

void BLESystem::bleUpdate(State &t_state){
    BLEUnit unit = m_units[t_state.getId()];

    if(unit.service == nullptr){
        return;
    }
     LogController::logMessage("BLE: Update: " + TO_STRING(t_state.getId()));

    unit.characteristics["ID"]->setValue("ID=" + TO_STRING( t_state.getId()));
    unit.characteristics["NAME"]->setValue("NAME=" +TO_STRING( t_state.getId()));
    unit.characteristics["GPS_LOC"]->setValue(
        "GPS_LOC=" + 
        TO_STRING(t_state.getLocation().getLat()) +
        ","+ 
        TO_STRING(t_state.getLocation().getLon())
    );
    unit.characteristics["GPS_SATS"]->setValue("GPS_SATS=" +TO_STRING( "1"));
    unit.characteristics["STATUS"]->setValue("STATUS=" + t_state.getStatus());
    unit.characteristics["TIMESTAMP"]->setValue("TIMESTAMP=" +TO_STRING( 000000));

}

bool BLESystem::requestUpdateAll(std::unordered_map<uint8_t, State> &t_state)
{
    //LogController::logMessage("BLE: REQUEST UPDATE");
    for(auto state : t_state){
        State devState = state.second;
        LogController::logMessage("BLE: Queueing: " + TO_STRING(state.first));
        xQueueSend(m_stateQueue, &devState, portMAX_DELAY);
    }
    return true;
    //
}
        // Init BLE
     