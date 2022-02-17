#include "logging/LogController.hpp"

using namespace RaveBuddy;

// Static forward delcartions
std::vector<std::shared_ptr<std::string>> LogController::m_history;
// All Registered logging components
std::vector<Logger *> LogController::m_loggers;


LogController::LogController(){
    // Fill message queue with new lines to clear output
    for(int i=0;i<3;i++){
        logMessage("\n");
    }
}
void LogController::registerLogger(Logger *t_logger){
    m_loggers.push_back(t_logger);

    //Send the entire history to the new logger so it can display it.
    for(auto msg : m_history){
        t_logger->onMessage(msg);
    }
}


void LogController::logMessage(std::string t_msg)
{
    auto strPtr = std::make_shared<std::string>(t_msg);
    m_history.push_back(strPtr);
    //Send message to all loggers.
    for(auto logger: m_loggers){
       logger->onMessage(strPtr);
    }

    // Check if the current history is larger then the max, if so clear it 
    // TODO: Make this shuffle the array instead of clearing it.
    if(m_history.size() >=  LOGGER_MAX_HISTORY){
        LogController::logMessage("LogController: Clearing first half of logs");
        std::vector<std::shared_ptr<std::string>>(m_history.begin()+LOGGER_MAX_HISTORY/2, m_history.end()).swap(m_history);
    }

     // Save message in history;
    
}