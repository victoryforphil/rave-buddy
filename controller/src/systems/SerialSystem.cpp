#include "systems/SerialSystem.hpp"

using namespace RaveBuddy;

SerialSystem::SerialSystem(uint32_t t_baud) : m_baud(t_baud)
{
    LogController::logMessage("SRL: Starting serial with buad");
    Serial.begin(m_baud);
    Serial.println("INIT-");
    LogController::registerLogger(this);
    //xTaskCreatePinnedToCore(this->initTask, "System/Serial/Task", 1024 * 4, this, 3, NULL, 1);
   
}

void SerialSystem::initTask(void *t_this)
{
    LogController::logMessage("SRL: Starting RTOS task...");
    
    for (;;)
    {
       //
        // Call tickTask at every interval defined by SERIAL_DELAY
        ((SerialSystem *)t_this)->tickTask();
        vTaskDelay(SYSTEM_SERIAL_DELAY / portTICK_PERIOD_MS);
    }
}

void SerialSystem::tickTask()
{
    
    std::shared_ptr<std::string> msg; 
    // If we can recieve a queue item, print said item;
    if(xQueueReceive(m_messageQueue, &msg, 50/ portTICK_PERIOD_MS) == pdTRUE)
    {   

        //delete msg;
    }
   
  
}

// Called by log controller
void SerialSystem::onMessage(std::shared_ptr<std::string> t_msg)
{
    
    send(t_msg); 
}

void SerialSystem::send(std::shared_ptr<std::string> t_msg)
{
    if(t_msg.get() == nullptr){
        return;
    }

    Serial.println(t_msg->c_str());
    // Add the msg to the queue to be sent off.
    xQueueSend(m_messageQueue, &t_msg, portMAX_DELAY);
}

SerialSystem::~SerialSystem()
{

    // Free the UART semaphore in case we took it
    xSemaphoreGive(m_serialMutex);

    // Clear the queue
    xQueueReset(m_messageQueue);

    // End the serial comms.
    //Serial.end();
}