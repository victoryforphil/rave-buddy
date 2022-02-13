#pragma once

#include "Logger.hpp"
#include <vector>
#include <string>
namespace RaveBuddy
{
    class LogController
    {
    private:
        
        std::vector<std::string> m_history; // Store all log messages (within limit)
        std::vector<Logger*> m_loggers;     // All Registered logging components
    public:
        LogController(/* args */);
        static void registerLogger(Logger *t_logger);
        static void logMessage(std::string t_msg);

        ~LogController();
    };
    
} // namespace RaveBuddy
