#pragma once

/*
    RaveBuddy::DisplaySystem - System
    --------------------------
    Displays current state information to the OLED Screen.
    
    Implements a single RTOS task to update the current state.
*/

// --- SYSTEM INCLUDES --- //
#include <string>
#include <stdint.h>
#include <Arduino.h>
#include <heltec.h>

// --- LOCAL INCLUDES --- //

#include "systems/System.hpp"
#include "data/State.hpp"
#include "logging/LogController.hpp"

// --- PREPROC DEFINES --- //

#define SYSTEM_DISPLAY_TASK_DELAY 250

namespace RaveBuddy
{
    class DisplaySystem : public System, public Logger
    {
    private:
        // --- PRIVATE VARIBLES --- //

        // Mutex to control hardware access to the OLED display
        const SemaphoreHandle_t m_displayMutex = xSemaphoreCreateMutex();

        // Last log message to dispaly
        String m_logmsg = "Log Display....";

        // Our State
        State m_state;

        // All device states
        std::vector<State> m_allStates;

        // --- PRIVATE FUNCTIONS --- //
        
        //Start the main RTOS TASk
        static void initTask(void* t_this);

        //Tick for the task
        void tickTask();

        // Draw list of currently conneted devices
        void drawDeviceList(std::vector<State> t_states);

        // Draw Current Device ID
        void drawDeviceInfo(State t_state);

    public:
        // --- OPERATORS --- ///
        DisplaySystem(/* args */);
        ~DisplaySystem() = default;


        // --- SYSTEM OVERRIDES --- //

        // Request state updates. Allow us to display full system state
        bool requestUpdateAll(std::unordered_map<uint8_t, State> &t_state);

        // Updates to our state
        bool responseUpdate(State &t_state);


        // --- LOGGER OVERRIDES --- //

        // Called by LogController when a message is to be displayed (ref to the string in the vector)
        void onMessage(std::shared_ptr<std::string> t_msg);
    };
    
    
} // namespace RaveBuddy
