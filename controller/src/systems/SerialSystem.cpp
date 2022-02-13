#include "systems/SerialSystem.hpp"

using namespace RaveBuddy;

SerialSystem::SerialSystem(uint32_t t_baud) : m_baud(t_baud)
{
    LogController::logMessage("System/Serial: Starting serial with buad");
    Serial.begin(m_baud);
    Serial.println();
    LogController::registerLogger(this);
    xTaskCreate(this->initTask, "System/Serial/Task", 4096, this, 5, NULL);
   
}

void SerialSystem::initTask(void *t_this)
{
    LogController::logMessage("System/Serial/Task: Starting RTOS task...");
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
    while(xQueueReceive(m_messageQueue, &msg, 10/ portTICK_PERIOD_MS) == pdTRUE)
    {   

        Serial.println(msg->c_str());
        //delete msg;
    }
   
  
}

// Called by log controller
void SerialSystem::onMessage(std::shared_ptr<std::string> t_msg){

    send(t_msg);
   
}

void SerialSystem::send(std::shared_ptr<std::string> t_msg)
{
    // Add the msg to the queue to be sent off.
    xQueueSend(m_messageQueue, &t_msg, 10);
}

SerialSystem::~SerialSystem()
{

    // Free the UART semaphore in case we took it
    xSemaphoreGive(m_serialMutex);

    // Clear the queue
    xQueueReset(m_messageQueue);

    // End the serial comms.
    Serial.end();
}