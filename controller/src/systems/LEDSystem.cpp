#include "systems/LEDSystem.hpp"

using namespace RaveBuddy;

LEDSystem::LEDSystem(uint8_t t_num) : m_num(t_num)
{
    LogController::logMessage("LED: Starting");
    
    xTaskCreatePinnedToCore(this->initTask, "LED", 2048 * 4, this, 4, NULL, 0);
}

void LEDSystem::initTask(void *t_this)
{
    LogController::logMessage("LED: RTOS Task Start");
    LEDSystem* self  = (LEDSystem*)t_this;

    //Queue to store the latest LED state 
    self->m_queue = xQueueCreate(1, sizeof(uint8_t));
   // self->m_leds.reserve(self->m_num);
    FastLED.addLeds<WS2812,SYSTEM_LED_PIN,GRB>(self->m_leds, SYSTEM_LED_LEN);
    FastLED.setBrightness(40);
    if (!self->m_queue)
    {
        LogController::logMessage("!!!LED: Queue fail");
        return;
    }

    // Setup LEDS
   // 
    for (;;)
    {
        self->tickTask();
        
    }
}

void LEDSystem::tickTask()
{
    for(int i = 0; i < SYSTEM_LED_LEN; i++) {
		// Set the i'th led to red 
		m_leds[i] = CHSV(m_tick, 255, 255);
        FastLED.show(); 
        m_tick += 5;
        if(m_tick > 255){
            m_tick = 0;
        }
        vTaskDelay(SYSTEM_LED_DELAY / portTICK_PERIOD_MS);
	}
    
   
    
}
 bool LEDSystem::responseUpdate(State &t_state)
 {
     uint8_t ledState = 1;
    return xQueueSendToFront(m_queue, &ledState, 100) == pdTRUE;
     
 }


LEDSystem::~LEDSystem(){
    xQueueReset(m_queue);
    //m_leds.clear();
}