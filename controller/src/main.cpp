/*
    Main
    ----------
    Base class and entry point to the system. 
*/

// --- SYSTEM INCLUDES --- //
#include <Arduino.h>
#include <heltec.h>


// --- Local Includes --- //
#include "systems/SerialSystem.hpp"
#include "logging/LogController.hpp"

// --- PREPROC DEFS --- //
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#define BAND 433E6

void setup()
{
    Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
    
    // * TEST CODE * //
    RaveBuddy::LogController::logMessage("Main: Starting...");

    RaveBuddy::SerialSystem serial(9600);
    for (;;)
    {
    }

}

void loop(){

}