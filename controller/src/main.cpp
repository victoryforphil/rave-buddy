
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <heltec.h>
#include "systems/RaveSys.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#define BAND 433E6

const int DEVICE_ID = 2;
void task_testing(void *pvParameters)
{
    for (;;)
    {
    }
}

void setup()
{
    Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);

    RaveSys sys(1);
    for (;;)
    {
    }
    /*
    struct BuddyState{
    float lat, lon;
    long timestamp;
    char message[128];
} bs;

struct SystemState{
    bool bleConnected;
    BuddyState* states[8];
} ss;

    Serial.begin(9600);
    Serial.setTimeout(3000);
    Heltec.display->init();
    Heltec.display->flipScreenVertically();
    Heltec.display->setFont(ArialMT_Plain_24);

    lora_init();
    state_init(DEVICE_ID);

    xTaskCreatePinnedToCore(
    task_serial_init
    ,  "TaskSerial"   // A name just for humans
    ,  2048  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  nullptr
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  nullptr , ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(
    task_testing
    ,  "TaskTesting"   // A name just for humans
    ,  2048  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  nullptr
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  nullptr , ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(
    task_ble_init
    ,  "TaskBLE"   // A name just for humans
    ,  4056  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  nullptr
    ,  4  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  nullptr , ARDUINO_RUNNING_CORE);
    */
}

void loop()
{
}
