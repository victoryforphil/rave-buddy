#include "serial_task.h"
#include <Arduino.h>

QueueHandle_t m_msgQueue;

void serial_print(const char *string)
{
    Serial.println(string);
    //xQueueSend(m_msgQueue, &string, 0);
}

void task_serial_init(void *pvParameters)
{

    m_msgQueue = xQueueCreate(64,         // Queue length
                              sizeof(char) // Queue item size
    );

    serial_print("Task/Serial: Init\n");
    for (;;)
    {
        task_serial_tick(nullptr);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
void task_serial_tick()
{
    char* string;
    if (xQueueReceive(m_msgQueue, &string, 50) == pdPASS)
    {
        //Serial.println(string);
    }
    
    
}