#include "systems/LoRaSystem.hpp"

using namespace RaveBuddy;

LoRaSystem::LoRaSystem()
{
    LogController::logMessage("System/Lora: Starting Recv/Send RTOS Tasks");
    xTaskCreate(this->initRecvTask, "System/Lora/Recv", 2048, this, 1, NULL);
    xTaskCreate(this->initSendTask, "System/Lora/Send", 2048, this, 2, NULL);
}

void LoRaSystem::initSendTask(void* t_this)
{
    LogController::logMessage("System/Lora/Task/Send: Task started");
    for (;;)
    {
        // Call tickSendTask at every interval defined by SYSTEM_LORA_SEND_DELAY
        ((LoRaSystem *)t_this)->tickSendTask();
        vTaskDelay(SYSTEM_LORA_SEND_DELAY / portTICK_PERIOD_MS);
    }
}

void LoRaSystem::initRecvTask(void* t_this)
{
    LogController::logMessage("System/Lora/Task/Recv: Task started");
    for (;;)
    {
        // Call tickRecvTask at every interval defined by SYSTEM_LORA_RECV_DELAY
        ((LoRaSystem *)t_this)->tickRecvTask();
        vTaskDelay(SYSTEM_LORA_RECV_DELAY / portTICK_PERIOD_MS);
    }
}

void LoRaSystem::tickSendTask()
{
     if(!xSemaphoreTake(m_loraMutex, 50 / portTICK_PERIOD_MS))
    {
        LogController::logMessage("System/Lora/Task/Send: Failed to aquired LoRa Mutex!");
        return;
    }
}

void LoRaSystem::tickRecvTask()
{
    if(!xSemaphoreTake(m_loraMutex, 50 / portTICK_PERIOD_MS))
    {
        LogController::logMessage("System/Lora/Task/Recv: Failed to aquired LoRa Mutex!");
        return;
    }

    if(LoRa.parsePacket() > 0)
    {
        LoRaPacket msg;
        LoRa.readBytes(( uint8_t *)&msg.id, sizeof(uint8_t));
        LoRa.readBytes(( uint8_t *)&msg.timestamp, sizeof(uint64_t));
        LoRa.readBytes(( uint8_t *)&msg.gps_lat, sizeof(uint64_t));
        LoRa.readBytes(( uint8_t *)&msg.gps_lon, sizeof(uint64_t));
        LoRa.readBytes(( uint8_t *)&msg.status_len, sizeof(uint8_t));

        while (LoRa.available())
        {
            msg.status += (char)LoRa.read();

        }
        xQueueSend(m_recvQueue, &msg, 10);
    }
    xSemaphoreGive(m_loraMutex);
}

LoRaSystem::~LoRaSystem()
{
    xQueueReset(m_sendQueue);
    xQueueReset(m_recvQueue);
    xSemaphoreGive(m_loraMutex);
}