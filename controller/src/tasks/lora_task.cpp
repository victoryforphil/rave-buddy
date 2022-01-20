#include "lora_task.h"
#include <Arduino.h>
#include "heltec.h"

#include "state.h"

void lora_recieve(int packetSize)
{

    if (packetSize)
    {
        // received a packet
        unsigned int id= 0;
        LoRa.readBytes((char*)&id, 1);
        unsigned long time = 0;
        LoRa.readBytes((char*)&time, sizeof(time));
        String state = String("");
        // read packet
       

        while (LoRa.available())
        {
            state += String((char)LoRa.read());
      
        }
        
        // print RSSI of packet
        state_on_state(id, state, time);
        //Serial.printf("Packet from %d, time: %ld, state: %s\n", id, time, state);
    }
}

void task_lora_tick()
{
    lora_recieve(LoRa.parsePacket());
}

void task_lora_init(void *pvParameters)
{
    Serial.println("Task/Lora: Task Init...starting to listen in 3s");
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    for (;;)
    {
        task_lora_tick();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}


void lora_init()
{
    Serial.println("Lora: init");
    xTaskCreatePinnedToCore(
        task_lora_init, "TaskLora" // A name just for humans
        ,
        4096 // This stack size can be checked & adjusted by reading the Stack Highwater
        ,
        nullptr, 4 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,
        nullptr, ARDUINO_RUNNING_CORE);
}

