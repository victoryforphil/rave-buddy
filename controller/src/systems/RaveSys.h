#ifndef _SYS_MAIN_H_
#define _SYS_MAIN_H_

#define TASK_SYS_DELAY 1000
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#define SYS_LOG(t_msg) RaveSys::inst()->systems.serial->send(t_msg)


#include <memory>

#include "RaveLoRa.h"
#include "RaveSerial.h"
#include "RaveGPS.h"
#include <Arduino.h>
#include <string>
struct TSystems{
        std::unique_ptr<RaveSerial> serial;
        std::unique_ptr<RaveGPS> gps;
        std::unique_ptr<RaveLoRa> lora;
    };
class RaveSys
{

private:
    static RaveSys* m_instance;
    const uint8_t m_id;

    static void task_init(void *t_this);
    void task_tick();

public:

    TSystems systems;
    
    RaveSys(const uint8_t t_id);
    
    static RaveSys* inst(){
        return m_instance;
    }

};

#endif