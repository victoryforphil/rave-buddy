#include "systems/LoRaSystem.hpp"

using namespace RaveBuddy;

LoRaSystem::LoRaSystem()
{
    
    LogController::logMessage("LORA: Starting Recv/Send RTOS Tasks");
    xTaskCreatePinnedToCore(this->initRecvTask, "LORA/Recv", 2048 * 4, this, 5, NULL, 0);
    xTaskCreatePinnedToCore(this->initSendTask, "LORA/Send", 2048 * 4, this, 4, NULL, 0);
}

void LoRaSystem::initSendTask(void *t_this)
{
    LoRaSystem *self = (LoRaSystem *)t_this;

    self->m_sendQueue = xQueueCreate(SYSTEM_LORA_SEND_QUEUE_SIZE, sizeof(LoRaPacket));
    if (!self->m_sendQueue)
    {
        LogController::logMessage("!!! LORA/Task/Send: Failed to create send queue. Finishing task");
        return;
    }

    LogController::logMessage("LORA/Task/Send: Task started");
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
        LogController::logMessage("!!! LORA: Failed to create recv queue. Finishing task");
        return;
    }

    LogController::logMessage("LORA: Rev Task started");
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
    if (xQueueReceive(m_sendQueue, &buffer, portMAX_DELAY / portTICK_PERIOD_MS) == pdTRUE)
    {
        //LogController::logMessage("LORA: Startng to send...");
        if (xSemaphoreTake(m_loraMutex, 500 / portTICK_PERIOD_MS) != pdTRUE)
        {
            LogController::logMessage("LORA: Failed to aquired LoRa Mutex!");
            return;
        }

       
       
        LoRa.beginPacket();
       // LoRa.setTxPower(14, RF_PACONFIG_PASELECT_PABOOST);

        //uint8_t magicNum = SYSTEM_LORA_MAGIC;
        //LoRa.write((const uint8_t *)&magicNum, sizeof(uint8_t));

        LoRa.write((const uint8_t *)&buffer, sizeof(3));
        LoRa.endPacket();
        LogController::logMessage("LORA: Sent State");
        xSemaphoreGive(m_loraMutex);
    }

    
}

void LoRaSystem::tickRecvTask()
{
    
    if (xSemaphoreTake(m_loraMutex, 250 / portTICK_PERIOD_MS) != pdTRUE)
    {
        LogController::logMessage("LORA: Failed to aquired LoRa Mutex!");
        return;
    }
    
    if (LoRa.parsePacket() > 0)
    {
        LogController::logMessage("LORA: Got message. Parsing.");
   
        LoRaPacket buffer;
        LoRa.readBytes((uint8_t *)&buffer, sizeof(LoRaPacket));

        LogController::logMessage("LORA: from: " + TO_STRING(buffer.id));
        xQueueSend(m_recvQueue, &buffer, portMAX_DELAY);
    }
    xSemaphoreGive(m_loraMutex);
}

bool LoRaSystem::requestUpdateAll(std::unordered_map<uint8_t, State> &t_state)
{
    //LogController::logMessage("LORA/Request: Parsing packets...");
    // Empty the queue and parse all packet
    LoRaPacket buffer;

    while (xQueueReceive(m_recvQueue, &buffer, 250/ portTICK_PERIOD_MS) == pdTRUE)
    {
       // auto statusPtr = buffer.status;
        t_state[buffer.id].setUintId(buffer.id);
        t_state[buffer.id].setLocation(34.0904538621197, -117.29315916640135);
       // t_state[buffer.id].setStatus(*statusPtr.get());

    }

    return true;
}

bool LoRaSystem::responseUpdate(State &t_state)
{
    //LogController::logMessage("LORA/Response: Submiting new state.");
    LoRaPacket newPacket;
    newPacket.id = t_state.getId();
    newPacket.gps_lat = t_state.getLocation().getLatInt();
    newPacket.gps_lon = t_state.getLocation().getLonInt();

    strcpy(newPacket.status, t_state.getStatus().c_str());

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