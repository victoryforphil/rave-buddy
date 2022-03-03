#include "Unit.hpp"

using namespace RaveBuddy;

Unit::Unit(uint8_t t_id) : m_id(t_id)
{
    LogController::logMessage("Unit: Creating state for this unit");
    
    m_units[m_id] = State();
    m_units[m_id].setUintId(m_id, true);
    
    LogController::logMessage("Unit: Creating system with ID: " + TO_STRING(m_id));
    m_systems.push_back(std::move(std::make_shared<SerialSystem>(9600)));
    m_systems.push_back(std::move(std::make_unique<LoRaSystem>()));
    m_systems.push_back(std::move(std::make_unique<DisplaySystem>()));
    m_systems.push_back(std::move(std::make_shared<BLESystem>()));

    LogController::logMessage("Unit: Init RTOS Task");
  
    xTaskCreatePinnedToCore(this->initTask, "Unit/Task", 2048 * 2, this, 2, NULL, 1);
    

    
}

void Unit::initTask(void *t_this)
{
    LogController::logMessage("Unit: Task started");
    for (;;)
    {
        vTaskDelay(UNIT_TASK_DELAY / portTICK_PERIOD_MS);
        // Call tickSendTask at every interval defined by SYSTEM_LORA_SEND_DELAY
        ((Unit *)t_this)->tickTask();
    }
}

void Unit::tickTask()
{
    for (auto sys : m_systems)
    {
        sys->requestUpdateAll(m_units);
        sys->requestUpdate(m_units[m_id]);
        sys->responseUpdate(m_units[m_id]);
    }

    std::string logMsg = "Unit: CUR: " ;
    for (auto &x : m_units)
    {
        
        logMsg += TO_STRING(x.first) + " ";
    }
    LogController::logMessage(logMsg);
}

Unit::~Unit()
{
    m_systems.clear();
}