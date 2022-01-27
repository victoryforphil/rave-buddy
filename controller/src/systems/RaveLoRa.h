#ifndef _SYS_LORA_TASK_H_
#define _SYS_LORA_TASK_H_

#define TASK_LORA_SEND_DELAY 100
#define TASK_LORA_RECV_DELAY 50

#include <Arduino.h>
#include <vector>
#include "heltec.h"
class RaveLoRa
{
public:

    struct LoRaMessage{
        uint8_t id;
        uint64_t timestamp;
        uint64_t gps_lat;
        uint64_t gps_lon;
        uint8_t msg_len;
        String msg;
    };
    
    RaveLoRa(const uint8_t t_id);

    void send(const LoRaMessage& t_msg);
    const std::vector<LoRaMessage>& getMessages();

private:
    const uint8_t m_id;

    std::vector<LoRaMessage> m_messages;

    const SemaphoreHandle_t m_loraMutex = xSemaphoreCreateMutex();
    const SemaphoreHandle_t m_stateMutex = xSemaphoreCreateMutex();

    const xQueueHandle m_MessageQueue = xQueueCreate(8, sizeof(LoRaMessage*));


    static void task_init_send(void* t_this);
    static void task_init_recv(void* t_this);
    void task_tick_send();
    void task_tick_recv();
};

#endif