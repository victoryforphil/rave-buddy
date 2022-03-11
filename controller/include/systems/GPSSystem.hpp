#pragma once

/* 
    RaveBuddy::GPSSystem , System
    --------------------------------
    Subsystem for managing GPS Sensor
*/


// --- SYSTEM INCLUDES --- //

#include <Arduino.h>
#include <stdint.h>
#include <heltec.h>
#include <string>
#include <unordered_map>
#include <TinyGPS++.h>

// --- LOCAL INCLUDES --- //

#include "systems/System.hpp"
#include "data/State.hpp"
#include "logging/LogController.hpp"

// --- PREPROC DEFINES --- //
#define SYSTEM_GPS_DELAY 500

namespace RaveBuddy
{   
    /*
            RaveBuddy::LoRaPacket
            ----------------------------------
            Struct to store raw LoRa Data that will later
            be used to update state. This will represent
            the exact byte structure sent over the LoRa network
    */

    struct GPSPacket{
        double lat;
        double lon;
        int sats;
        uint32_t timestamp;
    };
    
    class GPSSystem : public System
    {
    private:

        

        // --- PRIVATE DATA --- //
        // RTOS queue to store recieved packets that should be proccessed on state request
        xQueueHandle m_recvQueue;
        
        TinyGPSPlus m_gps;

        //Mutex to control access to LoRa
        const SemaphoreHandle_t m_gpsMutex = xSemaphoreCreateMutex();

        // --- PRIVATE FUNCTIONS --- //
        
        //Start the Send RTOS task
        static void initTask(void* t_this);
        // Send RTOS Task Tick
        void tickTask();


    public:
        


        // --- OPERATORS --- //

        GPSSystem();
        ~GPSSystem() = default;

        // --- SYSTEM OVERRIDES --- //

        // Empty single request
        bool requestUpdate(State &t_state);

        
    };
    

    
} // namespace RaveBuddy
