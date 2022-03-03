#pragma once

/*

    RaveBuddy::LoRaSystem
    ---------------------------
    Subsystem that reiceves and broadcasts the system states 
    over LoRa.

    Implements two RTOS tasks to send and recieve packets.
*/

// --- SYSTEM INCLUDES --- //

#include <Arduino.h>
#include <stdint.h>
#include <heltec.h>
#include <string>
#include <unordered_map>

// --- LOCAL INCLUDES --- //

#include "systems/System.hpp"
#include "data/State.hpp"
#include "logging/LogController.hpp"

// --- PREPROC DEFINES --- //
#define SYSTEM_LORA_RECV_QUEUE_SIZE 4
#define SYSTEM_LORA_SEND_QUEUE_SIZE 4
#define SYSTEM_LORA_STACK_SIZE 4096
#define SYSTEM_LORA_RECV_DELAY 100
#define SYSTEM_LORA_SEND_DELAY 500
#define SYSTEM_LORA_MAGIC 0x77

namespace RaveBuddy
{   
    /*
            RaveBuddy::LoRaPacket
            ----------------------------------
            Struct to store raw LoRa Data that will later
            be used to update state. This will represent
            the exact byte structure sent over the LoRa network
    */

    struct LoRaPacket{
        uint8_t id;
        uint64_t timestamp;
        int64_t gps_lat; // Shifted by E7
        int64_t gps_lon; // Shifted by E7
        char status[32];
    };
    
    class LoRaSystem : public System
    {
    private:

        

        // --- PRIVATE DATA --- //

        // RTOS queue to store packets to send on next tick
        xQueueHandle m_sendQueue;
        // RTOS queue to store recieved packets that should be proccessed on state request
        xQueueHandle m_recvQueue;

        // Current system ID 
        uint8_t m_sysId;

        // Buffer object when sending packets
        LoRaPacket m_sendBuffer;

        // Buffer object when reading packets;
        LoRaPacket m_recvBuffer;
        
        //Mutex to control access to LoRa
        const SemaphoreHandle_t m_loraMutex = xSemaphoreCreateMutex();

        // --- PRIVATE FUNCTIONS --- //
        
        //Start the Send RTOS task
        static void initSendTask(void* t_this);
        //Start the Recv RTOS task
        static void initRecvTask(void* t_this);
        // Send RTOS Task Tick
        void tickSendTask();
        // Recv RTOS Task tick
        void tickRecvTask();


    public:
        


        // --- OPERATORS --- //

        LoRaSystem();
        ~LoRaSystem();

        // --- SYSTEM OVERRIDES --- //

        // Empty single request
        bool requestUpdate(State &t_state){return true;};
        // Request state updates. Will parse all recieved packets and update the state
        bool requestUpdateAll(std::unordered_map<uint8_t, State> &t_state);
        // Send current state out across the LoRa Network
        bool responseUpdate(State &t_state);

        
    };
    

    
} // namespace RaveBuddy
