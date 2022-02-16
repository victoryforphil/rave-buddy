#include "systems/LoRaSystem.hpp"

using namespace RaveBuddy;

LoRaSystem::LoRaSystem()
{
    LogController::logMessage("System/Lora: Starting Recv/Send RTOS Tasks");
    xTaskCreatePinnedToCore(this->initRecvTask, "System/Lora/Recv", 2048, this, 5, NULL, 1);
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
    vTaskDelay(500 / portTICK_PERIOD_MS);

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
    if (xQueueReceive(m_sendQueue, &m_sendBuffer, portMAX_DELAY) == pdTRUE)
    {

        if (xSemaphoreTake(m_loraMutex, portMAX_DELAY) != pdTRUE)
        {
            LogController::logMessage("System/Lora/Task/Send: Failed to aquired LoRa Mutex!");
            return;
        }

        LogController::logMessage("Systems/LoRa/Task/Send: Sending Message");

        LoRa.beginPacket();
        LoRa.setTxPower(14, RF_PACONFIG_PASELECT_PABOOST);
        LoRa.write((const uint8_t *)&m_sendBuffer.id, sizeof(uint8_t));
        LoRa.write((const uint8_t *)&m_sendBuffer.timestamp, sizeof(uint64_t));
        LoRa.write((const uint8_t *)&m_sendBuffer.gps_lat, sizeof(uint64_t));
        LoRa.write((const uint8_t *)&m_sendBuffer.gps_lon, sizeof(uint64_t));
        LoRa.write((const uint8_t *)&m_sendBuffer.status_len, sizeof(uint8_t));
        LoRa.print(m_sendBuffer.status.c_str());
        LoRa.endPacket();

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
        LoRaPacket msg;
        LoRa.readBytes((uint8_t *)&msg.id, sizeof(uint8_t));
        LoRa.readBytes((uint8_t *)&msg.timestamp, sizeof(uint64_t));
        LoRa.readBytes((uint8_t *)&msg.gps_lat, sizeof(uint64_t));
        LoRa.readBytes((uint8_t *)&msg.gps_lon, sizeof(uint64_t));
        LoRa.readBytes((uint8_t *)&msg.status_len, sizeof(uint8_t));

        while (LoRa.available())
        {
            msg.status += (char)LoRa.read();
        }
        xQueueSend(m_recvQueue, &msg, 10);
    }
    xSemaphoreGive(m_loraMutex);
}

bool LoRaSystem::requestUpdateAll(std::unordered_map<uint8_t, State> &t_state)
{
    LogController::logMessage("System/LoRa/Request: Parsing packets...");
    // Empty the queue and parse all packet
    LoRaPacket buffer;
    while (xQueueReceive(m_recvQueue, &buffer, 10) == pdTRUE)
    {

        t_state[buffer.id]
            .setStatus(buffer.status)
            ->setLocation(buffer.gps_lat, buffer.gps_lon)
            ->setTimestamp(buffer.timestamp);
    }

    return true;
}

bool LoRaSystem::responseUpdate(State &t_state)
{
    LogController::logMessage("System/LoRa/Response: Submiting new state.");
    LoRaPacket newPacket;
    // newPacket.id = t_state.getId();
    // newPacket.status = t_state.getStatus();
    // newPacket.status_len = t_state.getStatus().length();
    // newPacket.gps_lat = t_state.getLocation().getLatInt();
    // newPacket.gps_lon = t_state.getLocation().getLonInt();
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