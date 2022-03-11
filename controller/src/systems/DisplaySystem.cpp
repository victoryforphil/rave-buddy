#include "systems/DisplaySystem.hpp"

using namespace RaveBuddy;

DisplaySystem::DisplaySystem()
{
    LogController::registerLogger(this);
    LogController::logMessage("DSP: Starting RTOS Task.");
    xTaskCreatePinnedToCore(this->initTask, "DSP", 2048 * 3, this, 5, NULL, 1);
}

void DisplaySystem::initTask(void *t_this)
{
    DisplaySystem *self = (DisplaySystem *)t_this;

    LogController::logMessage("DSP: Task started");
    for (;;)
    {
        vTaskDelay(SYSTEM_DISPLAY_TASK_DELAY / portTICK_PERIOD_MS);
        // Call tickSendTask at every interval defined by SYSTEM_LORA_SEND_DELAY
        self->tickTask();
    }
}

void DisplaySystem::tickTask()
{
    Heltec.display->clear();
    drawDeviceList(m_allStates);
    drawDeviceInfo(m_state);
    Heltec.display->display();
}

void DisplaySystem::drawDeviceList(std::vector<State> t_states)
{
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0, 0, "Found");
    Heltec.display->drawLine(0, 13, 32, 13);
    Heltec.display->drawLine(32, 0, 32, 40);
    for (int i = 0; i < t_states.size(); i++)
    {
        if (!t_states[i].isMine())
        {
            Heltec.display->drawString(0, (i * 10) + 15,
                                       "#" + String(t_states[i].getId()));
        }
    }
}

void DisplaySystem::drawDeviceInfo(State t_state)
{
    Heltec.display->setFont(ArialMT_Plain_24);
    Heltec.display->drawString(45, 0, String(t_state.getId()));
    Heltec.display->setFont(ArialMT_Plain_10);
     Heltec.display->drawString(45, 25, "GPS Sats: " + String(t_state.getSats()));
    Heltec.display->drawString(45, 35, String(t_state.getLocation().getLat()) + "," +  String(t_state.getLocation().getLon()));
    Heltec.display->drawString(45, 45, "Time: " + String((unsigned int)t_state.getTimestamp()));
    //Heltec.display->drawString(0, 50, m_logmsg);
    
}

bool DisplaySystem::requestUpdateAll(std::unordered_map<uint8_t, State> &t_state)
{
    m_allStates.clear();
    for (auto state : t_state)
    {
        m_allStates.push_back(state.second);
    }
    return true;
}

bool DisplaySystem::responseUpdate(State &t_state)
{
    m_state = t_state;
    return true;
}

void DisplaySystem::onMessage(std::shared_ptr<std::string> t_msg)
{
    m_logmsg = String(t_msg->c_str());
}