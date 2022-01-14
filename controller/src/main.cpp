#include "tasks/ble_task.h"
#include "tasks/serial_task.h"
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

void setup()
{
    Serial.begin(9600);
    xTaskCreatePinnedToCore(
    task_serial_init
    ,  "TaskSerial"   // A name just for humans
    ,  2048  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  nullptr
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  nullptr , ARDUINO_RUNNING_CORE);


    xTaskCreatePinnedToCore(
    task_ble_init
    ,  "TaskBLE"   // A name just for humans
    ,  4056  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  nullptr
    ,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  nullptr , ARDUINO_RUNNING_CORE);

}

void loop()
{
  
}

