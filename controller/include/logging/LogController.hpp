#pragma once
/*
    RaveBuddy::LogController
    ----------------
    Static class that allows logging from all subsystems.
    It stores a list of classes that implement RaveBuddy::Logger.

    Then upon any message, will distrubute said message to all logger classes

    It will also store a list of all messages recived so far so
    any new loggers may display the entire log tree
*/

// --- SYSTEM INCLUDES --- //
#include <vector>
#include <string>
#include <memory>
#include <malloc.h>
#include <sstream>
#include <cstdio>

#include <iostream>
// --- LOCAL INCLUDES --- //

// --- PREPROC DEFINES --- //
// How many messages to store before remoiving
#define LOGGER_MAX_HISTORY 256
#define TO_STRING(data) std::string(String(data).c_str())
namespace RaveBuddy
{
    // TODO: Move to own header.
    class Logger
    {
    public:
        // --- CLASS FUNCTIONS --- //
        // Send a message to the logger (overriden)
        virtual void onMessage(std::shared_ptr<std::string> t_msg) = 0;
    };
    //--------------------------------
    class LogController
    {
    private:
        // --- PRIVATE DATA --- //

        // Store all log messages (within limit)
        static std::vector<std::shared_ptr<std::string>> m_history;
        // All Registered logging components
        static std::vector<Logger *> m_loggers;

    public:
        // --- OPERATORS --- //

        LogController();

        // --- CLASS FUNCTIOSN --- //

        // Called on the creation of any logger, this is called
        // to store a reference to said logger in our list of known loggers.
        // This will also send all historic messages to the new logger (to catch up.)
        static void registerLogger(Logger *t_logger);

        // Log a message to all the registered loggers (and store it in a queue)
        static void logMessage(std::string t_msg);


        template <typename ValueType>
        static std::string stringulate(ValueType v)
        {
            std::ostringstream oss;
            oss << v;
            return oss.str();
        }
    };

} // namespace RaveBuddy
