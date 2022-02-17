#include "systems/LoRaSystem.hpp"

using namespace RaveBuddy;

LoRaSystem::LoRaSystem()
{
    LogController::logMessage("System/Lora: Starting Recv/Send RTOS Tasks");
    xTaskCreatePinnedToCore(this->initRecvTask, "System/Lora/Recv", 2048, this, 2, NULL, 1);
    xTaskCreatePinnedToCore(this->initSendTask, "System/Lora/Send", 2048, this, 7, NULL, 1);
}

void LoRaSystem::initSendTask(void *t_this)
{
    LoRaSystem *self = (LoRaSystem *)t_this;

    self->m_sendQueue = xQueueCreate(SYSTEM_LORA_SEND_QUEUE_SIZE, sizeof(LoRaPacket));
    if (!self->m_sendQueue)
    {
        LogController::logMessage("!!! System/Lora/Task/Send: Failed to create send queue. Finishing task");
        return;
    }

    LogController::logMessage("System/Lora/Task/Send: Task started");
    for (;;)
    {
        vTaskDelay(SYSTEM_LORA_SEND_DELAY / portTICK_PERIOD_MS);
        // Call tickSendTask at every interval defined by SYSTEM_LORA_SEND_DELAY
        ((LoRaSystem *)t_this)->tickSendTask();
    }
}

void LoRaSystem::initRecvTask(void *t_this)
{

    LoRaSystem *self = (LoRaSystem *)t_this;

    self->m_recvQueue = xQueueCreate(SYSTEM_LORA_RECV_QUEUE_SIZE, sizeof(LoRaPacket));
    if (!self->m_recvQueue)
    {
        LogController::logMessage("!!! System/Lora/Task/Recv: Failed to create recv queue. Finishing task");
        return;
    }

    LogController::logMessage("System/Lora/Task/Recv: Task started");
    for (;;)
    {
        vTaskDelay(SYSTEM_LORA_RECV_DELAY / portTICK_PERIOD_MS);
        // Call tickRecvTask at every interval defined by SYSTEM_LORA_RECV_DELAY
        ((LoRaSystem *)t_this)->tickRecvTask();
    }
}

void LoRaSystem::tickSendTask()
{

    // TODO: Look into making this a while loop so it can send all the queued packets at once?
     LoRaPacket buffer;
    if (xQueueReceive(m_sendQueue, &buffer, portMAX_DELAY) == pdTRUE)
    {

        if (xSemaphoreTake(m_loraMutex, portMAX_DELAY) != pdTRUE)
        {
            LogController::logMessage("System/Lora/Task/Send: Failed to aquired LoRa Mutex!");
            return;
        }

       
       
        LoRa.beginPacket();
        LoRa.setTxPower(14, RF_PACONFIG_PASELECT_PABOOST);
        LoRa.write((const uint8_t *)&buffer.id, sizeof(uint8_t));
        LoRa.write((const uint8_t *)&buffer.timestamp, sizeof(uint64_t));
        LoRa.write((const uint8_t *)&buffer.gps_lat, sizeof(uint64_t));
        LoRa.write((const uint8_t *)&buffer.gps_lon, sizeof(uint64_t));
        LoRa.write((const uint8_t *)&buffer.status_len, sizeof(uint8_t));
        auto statusPtr = buffer.status;
        LoRa.print(buffer.status->c_str());

        
        LoRa.endPacket();
        LogController::logMessage("Systems/LoRa/Task/Send: Sent Message");
        xSemaphoreGive(m_loraMutex);
    }

    
}

void LoRaSystem::tickRecvTask()
{
    if (xSemaphoreTake(m_loraMutex, 250 / portTICK_PERIOD_MS) != pdTRUE)
    {
        LogController::logMessage("System/Lora/Task/Recv: Failed to aquired LoRa Mutex!");
        return;
    }

    if (LoRa.parsePacket() > 0)
    {
        LogController::logMessage("Systen/Lora/Task/Recv: Got message. Parsing.");
        
    

        LoRa.readBytes((uint8_t *)&m_sendBuffer.id, sizeof(uint8_t));
        LoRa.readBytes((uint8_t *)&m_sendBuffer.timestamp, sizeof(uint64_t));
        LoRa.readBytes((uint8_t *)&m_sendBuffer.gps_lat, sizeof(uint64_t));
        LoRa.readBytes((uint8_t *)&m_sendBuffer.gps_lon, sizeof(uint64_t));
        LoRa.readBytes((uint8_t *)&m_sendBuffer.status_len, sizeof(uint8_t));
        std::string statusStr;
        while (LoRa.available())
        {
           statusStr +=  (char)LoRa.read();
        }
        m_sendBuffer.status = std::make_shared<std::string>(statusStr);
        xQueueSend(m_recvQueue, &m_sendBuffer, portMAX_DELAY);
    }
    xSemaphoreGive(m_loraMutex);
}

bool LoRaSystem::requestUpdateAll(std::unordered_map<uint8_t, State> &t_state)
{
    LogController::logMessage("System/LoRa/Request: Parsing packets...");
    // Empty the queue and parse all packet
    LoRaPacket buffer;

    while (xQueueReceive(m_recvQueue, &buffer, 100/ portTICK_PERIOD_MS) == pdTRUE)
    {
        auto statusPtr = buffer.status;
        
        t_state[buffer.id].setStatus(*statusPtr.get());
            
    }

    return true;
}

bool LoRaSystem::responseUpdate(State &t_state)
{
    LogController::logMessage("System/LoRa/Response: Submiting new state.");
    LoRaPacket newPacket;
    newPacket.id = t_state.getId();
    newPacket.status =  std::make_shared<std::string>(t_state.getStatus());
    newPacket.status_len = t_state.getStatus().length();
    newPacket.gps_lat = t_state.getLocation().getLatInt();
    newPacket.gps_lon = t_state.getLocation().getLonInt();
    xQueueSend(m_sendQueue, &newPacket, portMAX_DELAY);
    return true;
    // return xQueueSend(m_sendQueue, &newPacket, 10) == pdTRUE;
}

LoRaSystem::~LoRaSystem()
{
    xQueueReset(m_sendQueue);
    xQueueReset(m_recvQueue);
    xSemaphoreGive(m_loraMutex);
}