#include "lora_task.h"
#include <Arduino.h>
#include "heltec.h"
#include "serial_task.h"
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

        long gps_lat = 0;
        LoRa.readBytes((char*)&gps_lat, sizeof(gps_lat));

        long gps_lon = 0;
        LoRa.readBytes((char*)&gps_lon, sizeof(gps_lon));

        String state = String("");
        // read packet
       

        while (LoRa.available())
        {
            state += String((char)LoRa.read());
      
        }
        
        // print RSSI of packet
        if(id != state_get_mine()->id){
            state_on_state(id, state, time);
        }
        //Serial.printf("Packet from %d, time: %ld, state: %s\n", id, time, state);
    }
}



void task_lora_rec_init(void *pvParameters)
{
    Serial.println("Task/Lora: Recieve Task Init...starting to listen in 3s");
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    for (;;)
    {
        lora_recieve(LoRa.parsePacket());
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void task_lora_send_init(void *pvParameters)
{
    Serial.println("Task/Lora: Send Task Init...starting to listen in 3s");
    vTaskDelay(3000 / portTICK_PERIOD_MS);

    for (;;)
    {

        RaveDeviceState* state = state_get_mine();
        serial_print("Task/Lora: Sending state");
        LoRa.beginPacket();
        
        LoRa.setTxPower(14,RF_PACONFIG_PASELECT_PABOOST);

        LoRa.write(state->id); // Device ID




        unsigned long time = millis();
        LoRa.write((const uint8_t*)&time, sizeof(long));// Time Stamp

        long gps_lat = state->gps_lat;
        LoRa.write((const uint8_t*)&gps_lat, sizeof(long));// Time Stamp

        long gps_lon = state->gps_lon;
        LoRa.write((const uint8_t*)&gps_lon, sizeof(long));// Time Stamp

        LoRa.print(state->state.c_str());
        LoRa.endPacket();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void lora_init()
{
    Serial.println("Lora: init");
    xTaskCreatePinnedToCore(
        task_lora_rec_init, "TaskLora_Recieve" // A name just for humans
        ,
        4096 // This stack size can be checked & adjusted by reading the Stack Highwater
        ,
        nullptr, 4 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,
        nullptr, ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(
        task_lora_send_init, "TaskLora_Send" // A name just for humans
        ,
        4096 // This stack size can be checked & adjusted by reading the Stack Highwater
        ,
        nullptr, 4 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,
        nullptr, ARDUINO_RUNNING_CORE);
}

