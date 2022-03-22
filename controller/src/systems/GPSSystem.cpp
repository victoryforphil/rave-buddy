#include "systems/GPSSystem.hpp"

using namespace RaveBuddy;

GPSSystem::GPSSystem()
{
    LogController::logMessage("GPS: Starting Tasks");
    xTaskCreatePinnedToCore(this->initTask, "GPS", 2048, this, 4, NULL, 1);
}

void GPSSystem::initTask(void *t_this)
{
    LogController::logMessage("BLE: Task Started");
    GPSSystem *self = (GPSSystem *)t_this;

    self->m_recvQueue = xQueueCreate(32, sizeof(GPSPacket));
    if (!self->m_recvQueue)
    {
        LogController::logMessage("!!!GPS: Queue fail");
        return;
    }

    for (;;)
    {
        self->tickTask();
        vTaskDelay(SYSTEM_GPS_DELAY / portTICK_PERIOD_MS);
        // Call tickSendTask at every interval defined by SYSTEM_LORA_SEND_DELAY
    }
}

void GPSSystem::tickTask()
{
    if (xSemaphoreTake(m_gpsMutex, 100 / portTICK_PERIOD_MS) != pdTRUE)
    {
        LogController::logMessage("!!GPS: Mutex Fail");
        return;
    }

    while (Serial2.available() > 0)
    {

        if (m_gps.encode(Serial2.read()))
        {
            GPSPacket packet = {0};
           // LogController::logMessage("GPS: Sats" + TO_STRING(m_gps.satellites.value()));
            packet.sats = m_gps.satellites.value();
            if (m_gps.location.isValid())
            {
                //LogController::logMessage("GPS: Loc " + TO_STRING(m_gps.location.lat()));
                packet.lat = m_gps.location.lat();
                packet.lon = m_gps.location.lng();
               
            }

            if(m_gps.time.isValid()){
                
                packet.timestamp = m_gps.time.value();

            }
            xQueueReset(m_recvQueue);
            xQueueSendToFront(m_recvQueue, &packet, 100 / portTICK_PERIOD_MS);
        }
    }

    xSemaphoreGive(m_gpsMutex);
}

bool GPSSystem::requestUpdate(State &t_state)
{
    //LogController::logMessage("LORA/Request: Parsing packets...");
    // Empty the queue and parse all packet
    GPSPacket buffer = {0};

    if (xQueueReceive(m_recvQueue, &buffer, 200/ portTICK_PERIOD_MS) == pdTRUE)
    {
        LogController::logMessage("GPS: SAT " + TO_STRING(buffer.sats));
        LogController::logMessage("GPS: Loc " + TO_STRING(buffer.lat));
       t_state.setLocation(buffer.lat, buffer.lon);
        t_state.setSats(buffer.sats);
        t_state.setTimestamp(buffer.timestamp);
    }
    


    return true;
}