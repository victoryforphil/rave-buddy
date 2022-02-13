#include "LoRaSystem.hpp"

using namespace RaveBuddy;

LoRaSystem::LoRaSystem(){
    xTaskCreate(this->initRecvTask, "System/Lora/Recv", 2048, this, 1, NULL);
    xTaskCreate(this->initSendTask, "System/Lora/Send", 2048, this, 2, NULL);
}

void LoRaSystem::initSendTask(void* t_this){
    
}

void LoRaSystem::initRecvTask(void* t_this){

}

LoRaSystem::~LoRaSystem(){
    xQueueReset(m_sendQueue);
    xQueueReset(m_recvQueue);
    xSemaphoreGive(m_loraMutex);
}