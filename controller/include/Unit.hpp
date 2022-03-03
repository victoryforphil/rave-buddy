#pragma once

/*
    RaveBuddy::Unit
    ---------------------
    Main system class that manages all subsystems,
    tracks state, and other management tasks.

    Implements main tick loop that calls all subsystems
    response/respond sub tasks.

*/

// --- SYSTEM INCLUDES --- //

#include <Arduino.h>
#include <stdint.h> 
#include <string>
#include <unordered_map>
#include <heltec.h>
#include <vector>

// --- LOCAL INCLUDES --- //

#include "systems/DisplaySystem.hpp"
#include "systems/LoRaSystem.hpp"
#include "systems/SerialSystem.hpp"
#include "systems/BLESystem.hpp"
#include "data/State.hpp"
#include "logging/LogController.hpp"


// --- PREPROC DEFINES --- //
#define UNIT_TASK_DELAY 3000

namespace RaveBuddy
{
    class Unit
    {
    private:
        
        // --- PRIVATE DATA --- //

        // Vector to store all systems
        std::vector<std::shared_ptr<System>> m_systems;

        // Hash map to store all known unit states
        std::unordered_map<uint8_t, State> m_units;
        
        //ID of the unit
        uint8_t m_id;

        // --- PRIVATE FUNCTIONS --- //
        
        // Start the main RTOS Task
        static void initTask(void* t_this);
        
        // RTOS Tack tick
        void tickTask();

    public:

        // --- OPERATORS --- //

        // ID of the uint to create
        Unit(uint8_t t_id);
        ~Unit();
    };
    
    
} // namespace RaveBuddy
