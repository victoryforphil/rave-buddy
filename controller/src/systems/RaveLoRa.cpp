#include "RaveGPS.h"
#include "RaveSys.h"
RaveLoRa::RaveLoRa(const uint8_t t_id) : m_id(t_id)
{

    xTaskCreatePinnedToCore(this->task_init_recv, "System/Lora/Recv", 2048, this, 1, NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(this->task_init_send, "System/Lora/Send", 2048, this, 2, NULL, ARDUINO_RUNNING_CORE);
}

void RaveLoRa::task_init_recv(void *t_this)
{
    RaveSys::inst()->systems.serial->send("Systems/Lora/Recv: RTOS Task Started.");

    for (;;)
    {
        ((RaveLoRa *)t_this)->task_tick_recv();
        vTaskDelay(TASK_LORA_RECV_DELAY / portTICK_PERIOD_MS);
    }
}

void RaveLoRa::task_init_send(void *t_this)
{
    RaveSys::inst()->systems.serial->send("Systems/Lora/Send: RTOS Task Started.");

    for (;;)
    {
        ((RaveLoRa *)t_this)->task_tick_send();
        vTaskDelay(TASK_LORA_SEND_DELAY / portTICK_PERIOD_MS);
    }
}

void RaveLoRa::task_tick_recv()
{
    if (!xSemaphoreTake(m_loraMutex, 50 / portTICK_PERIOD_MS))
    {
        return;
    }

    
   

    if(LoRa.parsePacket() > 0){
        
        LoRaMessage msg;
        LoRa.readBytes(( uint8_t *)&msg.id, sizeof(uint8_t));
        LoRa.readBytes(( uint8_t *)&msg.timestamp, sizeof(uint64_t));
        LoRa.readBytes(( uint8_t *)&msg.gps_lat, sizeof(uint64_t));
        LoRa.readBytes(( uint8_t *)&msg.gps_lon, sizeof(uint64_t));
        LoRa.readBytes(( uint8_t *)&msg.msg_len, sizeof(uint8_t));

        while (LoRa.available())
        {
            msg.msg += String((char)LoRa.read());

        }
        if (!xSemaphoreTake(m_stateMutex, 100 / portTICK_PERIOD_MS))
        {
            return;
        }
        m_messages.push_back(msg);

        xSemaphoreGive(m_stateMutex);

        
    }
    xSemaphoreGive(m_loraMutex);
}

void RaveLoRa::task_tick_send()
{
    if (!xSemaphoreTake(m_loraMutex, 50 / portTICK_PERIOD_MS))
    {
        return;
    }

    LoRaMessage msg;
    if (xQueueReceive(m_MessageQueue, &msg, 10 / portTICK_PERIOD_MS) == pdTRUE)
    {
        RaveSys::inst()->systems.serial->send("Systems/Lora/Send: Sending Message");
        // Send the packet found in queue
        LoRa.beginPacket();
        LoRa.setTxPower(14, RF_PACONFIG_PASELECT_PABOOST);
        LoRa.write((const uint8_t *)&msg.id, sizeof(uint8_t));
        LoRa.write((const uint8_t *)&msg.timestamp, sizeof(uint64_t));
        LoRa.write((const uint8_t *)&msg.gps_lat, sizeof(uint64_t));
        LoRa.write((const uint8_t *)&msg.gps_lon, sizeof(uint64_t));
        LoRa.write((const uint8_t *)&msg.msg_len, sizeof(uint8_t));
        LoRa.print(msg.msg.c_str());
        LoRa.endPacket();
    }

    xSemaphoreGive(m_loraMutex);
}

void RaveLoRa::send(const RaveLoRa::LoRaMessage &t_msg)
{
    xQueueSend(m_MessageQueue, &t_msg, 0);
}

const std::vector<RaveLoRa::LoRaMessage> &RaveLoRa::getMessages()
{
    if (xSemaphoreTake(m_stateMutex, 1000 / portTICK_PERIOD_MS))
    {
        return m_messages;
    }
    else
    {
        return std::vector<RaveLoRa::LoRaMessage>();
    }
}
