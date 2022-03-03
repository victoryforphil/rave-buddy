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
#include "Unit.hpp"

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
    //LoRa.setSyncWord(0x15);
     Serial2.begin(9600, SERIAL_8N1, 23, 22);
    LoRa.setSpreadingFactor(10); //6-12
    LoRa.setCodingRate4(7); //5-8
    LoRa.setSignalBandwidth(250E3);
    randomSeed(analogRead(0));
    // * TEST CODE * //
   // RaveBuddy::LogController::logMessage("Main: Starting...");
    //RaveBuddy::SerialSystem serial(9600);
    RaveBuddy::Unit unit(random(200));
    for(;;){

    }

}

void loop(){

}