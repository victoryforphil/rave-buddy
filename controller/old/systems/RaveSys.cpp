#include "RaveSys.h"

RaveSys* RaveSys::m_instance = nullptr;


RaveSys::RaveSys(const uint8_t t_id) :  m_id(t_id)
{
    if(m_instance == nullptr)
    {
       m_instance = this;
    }
    systems.serial.reset(new RaveSerial( 9600));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    systems.lora.reset(new RaveLoRa(m_id));
    systems.gps.reset(new RaveGPS( 3,4,9600));


    systems.serial->send("Systems/Main: Initialized");

    xTaskCreatePinnedToCore(
         this->task_init, "System/Main/Task", 2048, this, 5,NULL, ARDUINO_RUNNING_CORE);
}

void RaveSys::task_init(void *t_this){

    ((RaveSys*)t_this)->systems.serial->send("Systems/Main: RTOS Task Started.");

    for(;;){
         ((RaveSys*)t_this)->task_tick();
        vTaskDelay(TASK_SYS_DELAY / portTICK_PERIOD_MS);
    }
}



void RaveSys::task_tick(){
    
    const RaveGPS::GPSState state = systems.gps->getState();

    RaveLoRa::LoRaMessage msg = {
        m_id,
        0,
        1,
        2,
        4,
        String("Hey")
    };
    systems.lora->send(msg);

}