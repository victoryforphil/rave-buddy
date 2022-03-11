#pragma once

/*  RaveBuddy::BLESystem - System, Logger
    ---------------------------------
    Allows to view/edit state

*/

// --- SYSTREM INCLUDES --- //

#include <string>
#include <stdint.h>
#include <Arduino.h>
#include <heltec.h>
#include <unordered_map>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>


// --- LOCAL INCLUDES --- //

#include "systems/System.hpp"
#include "data/State.hpp"
#include "logging/LogController.hpp"
#include "data/uuids.h"

// --- PREPROC DEFINES --- //

#define SYSTEM_BLE_TASK_DELAY 250


namespace RaveBuddy
{
    struct BLEUnit{
        BLEService *service;
        std::unordered_map<std::string, BLECharacteristic*> characteristics;
    };

    class BLESystem  : public System
    {
    private:
        // --- PRIVATE VARIBLES --- //

        //Hashmap to store BLE units
        std::unordered_map<uint8_t, BLEUnit> m_units;
        
        // ID of unit 
        uint8_t m_id;

        State buffer;

        BLEAdvertising* m_advertising;
        BLEServer* m_server;

        // Mutex for BLE
        const SemaphoreHandle_t m_bleMutex = xSemaphoreCreateMutex();

        xQueueHandle m_stateQueue; 

        // --- PRIVATE FUNCTIONS --- //

        // Start the main RTOS task
        static void initTask(void* t_this);

        // Tick Task
        void tickTask();

        //Create new BLE Service
        void bleNew(State &t_state);

        void bleUpdate(State &t_state);

        // Init BLE
        void bleStart();

    public:
        BLESystem(uint8_t t_id);
        ~BLESystem() = default;

        // --- SYSTEM OVERRIDES --- ///

        bool requestUpdateAll(std::unordered_map<uint8_t, State> &t_state);

        // --- LOGGER OVERRIDES --- //

        // Called by LogController when a message is to be displayed (ref to the string in the vector)
        //void onMessage(std::shared_ptr<std::string> t_msg) = {};
    };
    
  
    
} // namespace RaveBuddy
