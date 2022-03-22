#pragma once
/*
    RaveBuddy::LEDSystem , System
    -------------------------------
    Subystem for LED control
*/

// --- SYSTEM INCLUDEs --- //

#include <Arduino.h>
#include <FastLED.h>
#include <heltec.h>
#include <vector>
// --- LOCAL INCLUDES --- //
#include "systems/System.hpp"
#include "logging/LogController.hpp"
#include "data/State.hpp"


// --- PREPROC DEFINES --- //
#define SYSTEM_LED_DELAY 50
#define SYSTEM_LED_PIN 12
#define SYSTEM_LED_LEN 24

namespace RaveBuddy
{
    class LEDSystem : public System
    {
        private:
            // Store queue of latest LED state to to process
            xQueueHandle m_queue;

            //Vector of LEDs
            //std::vector<CRGB> m_leds;
            CRGB m_leds[SYSTEM_LED_LEN];

            //Current LED strip length
            uint8_t m_num;

            //Current animation tick
            uint16_t m_tick;

             // --- PRIVATE FUNCTIONS --- //
        
            //Start the Send RTOS task
            static void initTask(void* t_this);
            // Send RTOS Task Tick
            void tickTask();

        public:
            // --- OPERATORS --- //
            LEDSystem(uint8_t t_num);
            ~LEDSystem();

            // --- SYSTEM OVERRIDES --- //
            
            // Get latest states
            bool responseUpdate(State &t_state);

        
    };
}