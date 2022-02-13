#include "RaveSerial.h"

RaveSerial::RaveSerial( const uint32_t t_baud) : m_baud(t_baud){
    
    Serial.begin(m_baud);
    
    //Flood the queue with new line characters to clear and corruption
   
    xTaskCreatePinnedToCore(
         this->task_init, "System/Serial/Task", 2048, this, 5,NULL, ARDUINO_RUNNING_CORE);
}


void RaveSerial::task_init(void *t_this){

     ((RaveSerial*)t_this)->send("\n \n ----- RAVE BUDDY -----\nSystem/Serial: RTOS Task Started.");

    for(;;){
         ((RaveSerial*)t_this)->task_tick();
        vTaskDelay(TASK_SERIAL_DELAY / portTICK_PERIOD_MS);
    }
}

void RaveSerial::send(const char* t_msg){
    std::string *pStr = new std::string(t_msg);
    xQueueSend(m_MessageQueue, &pStr, 10);
    //delete msg;
}

void RaveSerial::task_tick(){
    
    std::string* msg = NULL;
    if (xQueueReceive(m_MessageQueue, &msg, 10 / portTICK_PERIOD_MS) == pdTRUE)
    {
        Serial.println(msg->c_str());
        delete msg;
    }   
}

RaveSerial::~RaveSerial(){
    // Release Mutex on GPS when class is deconstructed;
    xSemaphoreGive(m_serialMutex);
    xQueueReset(m_MessageQueue);
}