#include "systems/LoRaSystem.hpp"

using namespace RaveBuddy;
LoRaSystem *LoRaSystem::m_inst = nullptr;
void loraRecieve(int t_size)
{

    LogController::logMessage("LORA: Got message. Parsing Byes: " + TO_STRING(t_size));
   
    
    LoRa.flush();
    // LoRa.receive();
   
    // LoRaPacket buffer;
    // LoRa.readBytes((uint8_t *)&buffer,t_size);
    //  xSemaphoreGive(self->m_loraMutex);
    // LogController::logMessage("LORA: from: " + TO_STRING(buffer.id));
    // xQueueSend(LoRaSystem::getInstance()->m_recvQueue, &buffer, portMAX_DELAY);
}
LoRaSystem::LoRaSystem()
{
    LoRaSystem::m_inst = this;
    
   // LoRa.onReceive(loraRecieve);
   // LoRa.receive();

    LogController::logMessage("LORA: Starting Recv/Send RTOS Tasks");
    xTaskCreatePinnedToCore(this->initRecvTask, "LORA/Recv", 2048 * 4, this, 10, NULL, 1);
    xTaskCreatePinnedToCore(this->initSendTask, "LORA/Send", 2048 * 4, this, 9, NULL, 1);
}

LoRaSystem *LoRaSystem::getInstance()
{
    if (!m_inst)
    {
        LogController::logMessage("!!!LORA: Singleton Created Class");
        m_inst = new LoRaSystem();
    }
    return m_inst;
};



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

        // Call tickSendTask at every interval defined by SYSTEM_LORA_SEND_DELAY
        vTaskDelay((SYSTEM_LORA_SEND_DELAY + random(2000)) / portTICK_PERIOD_MS);
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


    for (;;)
    {
        ((LoRaSystem *)t_this)->tickRecvTask();

        vTaskDelay(SYSTEM_LORA_RECV_DELAY / portTICK_PERIOD_MS);
        // Call tickRecvTask at every interval defined by SYSTEM_LORA_RECV_DELAY
    }
}

void LoRaSystem::tickSendTask()
{

    // TODO: Look into making this a while loop so it can send all the queued packets at once?

    if (xQueueReceive(m_sendQueue, &m_sendBuffer, 10 / portTICK_PERIOD_MS) == pdTRUE)
    {
        LogController::logMessage("LORA: New Packet!");
        xQueueReset(m_sendQueue);
    }

    if (!m_sendPending)
    {
        return;
    }
    LogController::logMessage("LORA: Attempting to send");
    // LogController::logMessage("LORA: Startng to send...");
    if (xSemaphoreTake(m_loraMutex, portMAX_DELAY / portTICK_PERIOD_MS) != pdTRUE)
    {
        // LogController::logMessage("LORA: SEND Failed to aquired LoRa Mutex!");
        return;
    }

    LoRa.beginPacket();

   
    // uint8_t magicNum = SYSTEM_LORA_MAGIC;
    // LoRa.write((const uint8_t *)&magicNum, sizeof(uint8_t));
    // LoRa.setTxPower(16, RF_PACONFIG_PASELECT_PABOOST);
    LoRa.write((const uint8_t *)&m_sendBuffer, sizeof(LoRaPacket));
    LoRa.endPacket();
    xSemaphoreGive(m_loraMutex);
    LogController::logMessage("LORA: Sent State");
}

void LoRaSystem::tickRecvTask()
{

    if (xSemaphoreTake(m_loraMutex, 10 / portTICK_PERIOD_MS) != pdTRUE)
    {
        // LogController::logMessage("LORA: RECV Failed to aquired LoRa Mutex!");
        return;
    }

    if (LoRa.parsePacket() > 0)
    {
        LogController::logMessage("LORA: Got message. Parsing.");

        LoRaPacket buffer;
        LoRa.readBytes((uint8_t *)&buffer, sizeof(LoRaPacket));
        xSemaphoreGive(m_loraMutex);
        LogController::logMessage("LORA: from: " + TO_STRING(buffer.id));
        xQueueSend(m_recvQueue, &buffer, portMAX_DELAY);
    }
    else
    {
        xSemaphoreGive(m_loraMutex);
    }
}

bool LoRaSystem::requestUpdateAll(std::unordered_map<uint8_t, State> &t_state)
{
    // LogController::logMessage("LORA/Request: Parsing packets...");
    //  Empty the queue and parse all packet
    LoRaPacket buffer;

    while (xQueueReceive(m_recvQueue, &buffer, 250 / portTICK_PERIOD_MS) == pdTRUE)
    {
        // auto statusPtr = buffer.status;
        t_state[buffer.id].setUintId(buffer.id);
        t_state[buffer.id].setLocation(buffer.gps_lat, buffer.gps_lon);
        t_state[buffer.id].setTimestamp(buffer.timestamp);
        t_state[buffer.id].resetStrikes();
        // t_state[buffer.id].setLocation(34.0904538621197, -117.29315916640135);
        // t_state[buffer.id].setStatus(*statusPtr.get());
    }

    return true;
}

bool LoRaSystem::responseUpdate(State &t_state)
{
    // LogController::logMessage("LORA/Response: Submiting new state.");
    LoRaPacket newPacket;
    newPacket.id = t_state.getId();
    newPacket.gps_lat = t_state.getLocation().getLat();
    newPacket.gps_lon = t_state.getLocation().getLon();
    newPacket.timestamp = t_state.getTimestamp();

    strcpy(newPacket.status, t_state.getStatus().c_str());

    xQueueSendToFront(m_sendQueue, &newPacket, portMAX_DELAY);
    m_sendPending = true;
    return true;
    // return xQueueSend(m_sendQueue, &newPacket, 10) == pdTRUE;
}

LoRaSystem::~LoRaSystem()
{
    xQueueReset(m_sendQueue);
    xQueueReset(m_recvQueue);
    xSemaphoreGive(m_loraMutex);
}