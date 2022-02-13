#include "RaveGPS.h"
#include "RaveSys.h"
RaveGPS::RaveGPS(unsigned const int t_pinRx, unsigned const int t_pinTx, unsigned const int t_baud) 
: m_pinRx(t_pinRx), m_pinTx(t_pinTx), m_baud(t_baud)
{

    xTaskCreatePinnedToCore(
         this->task_init, "System/GPS/Task", 2048, this, 5,NULL, ARDUINO_RUNNING_CORE);
}


void RaveGPS::task_init(void* t_this)
{
    SYS_LOG("System/GPS: RTOS Task Started.");

    for(;;)
    {
         ((RaveGPS*)t_this)->task_tick();
        vTaskDelay(TASK_GPS_DELAY / portTICK_PERIOD_MS);
    }
}

void RaveGPS::task_tick()
{
    if (!xSemaphoreTake(m_gpsMutex, 50 / portTICK_PERIOD_MS))
    {
        return;
    }

    xSemaphoreGive(m_gpsMutex);

}

const RaveGPS::GPSState &RaveGPS::getState()
{
    return m_state;
}

RaveGPS::~RaveGPS()
{
    // Release Mutex on GPS when class is deconstructed;
    xSemaphoreGive(m_gpsMutex);
}