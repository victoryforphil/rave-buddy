#pragma once
#include "systems/System.hpp"
#include "data/State.hpp"

#include <Arduino.h>
#include <stdint.h>
#define SYSTEM_LORA_RECV_QUEUE_SIZE 8
#define SYSTEM_LORA_SEND_QUEUE_SIZE 4
#define SYSTEM_LORA_STACK_SIZE 4096
#define SYSTEM_LORA_RECV_DELAY 25
#define SYSTEM_LORA_SEND_DELAY 100

namespace RaveBuddy
{
    class LoRaSystem
    {
    private:
        const xQueueHandle m_sendQueue = xQueueCreate(SYSTEM_LORA_SEND_QUEUE_SIZE, sizeof(State));
        const xQueueHandle m_recvQueue = xQueueCreate(SYSTEM_LORA_RECV_QUEUE_SIZE, sizeof(State));

        uint8_t m_sysId;
        
        const SemaphoreHandle_t m_loraMutex = xSemaphoreCreateMutex();

        static void initSendTask(void* t_this);
        static void initRecvTask(void* t_this);
        void tickSendTask();
        void tickRecvTask();


    public:
        LoRaSystem();

        bool requestUpdate(State &t_state);
        bool responseUpdate(State &t_state);

        ~LoRaSystem();
    };
    

    
} // namespace RaveBuddy
