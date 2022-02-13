#ifndef _SYS_SERIAL_H_
#define _SYS_SERIAL_H_

#define TASK_SERIAL_DELAY 50
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#include <Arduino.h>


class RaveSerial
{
public:
    
    RaveSerial(const uint32_t t_baud);
    void send(const char* t_msg);
    
    ~RaveSerial();

    ///


private:

    const uint32_t m_baud;

    const SemaphoreHandle_t m_serialMutex   = xSemaphoreCreateMutex();
    const xQueueHandle m_MessageQueue       = xQueueCreate(64, sizeof(std::string*));
    static void task_init(void *pvParameters);
    void task_tick();
};

#endif