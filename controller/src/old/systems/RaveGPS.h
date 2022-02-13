#ifndef _SYS_GPS_TASK_H_
#define _SYS_GPS_TASK_H_

#define TASK_GPS_DELAY 500
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#include "data/state.h"

#include <Arduino.h>
class RaveGPS
{
public:
    struct GPSState{
        long lat;
        long lon;
        char sats;
        bool fix;
    };
    
    RaveGPS(unsigned const int t_pinRx, unsigned const int t_pinTx, unsigned const int t_baud);

    const GPSState& getState();
    
    ~RaveGPS();

private:
    const unsigned m_pinRx;
    const unsigned int m_pinTx;
    const unsigned int m_baud;

    GPSState m_state = {0};

    const SemaphoreHandle_t m_gpsMutex = xSemaphoreCreateMutex();

    static void task_init(void* t_this);
    void task_tick();
};

#endif