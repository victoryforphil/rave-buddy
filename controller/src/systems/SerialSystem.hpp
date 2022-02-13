#pragma once
/*
    RaveBuddy::SerialSystem (System, Logger)
    ----------------
    SubSystem that manages all UART basec communications.
    
    It will take in a desired BAUD rate, and will then
    constantly check its internal queue of string refereneces
    for new messages to send out.

    This system also implements Logger, which means upon any log
    message, said message will be sent across the UART port (terminal logging)
*/

// --- SYSTEM INCLUDES --- //
#include <string>
#include <Arduino.h>

// --- LOCAL INCLUDES --- //
#include "logging/LogController.hpp"
#include "systems/System.hpp"

// --- PREPROC DEFS --- //
#define SYSTEM_SERIAL_DELAY 100
#define SYSTEM_SERIAL_QUEUE_SIZE 64
namespace RaveBuddy
{
    class SerialSystem : public System, public Logger
    {
    private:
        // --- PRIVATE DATA --- //

        // Baud rate to use for UART Comms
        const uint32_t m_baud;
        // Mutex to control access to UART ports.
        const SemaphoreHandle_t m_serialMutex  = xSemaphoreCreateMutex();
        // Queue to store messages (string*) that are to be sent
        const xQueueHandle m_messageQueue =  xQueueCreate(SYSTEM_SERIAL_QUEUE_SIZE, sizeof(std::shared_ptr<std::string>));

        // --- PRIVATE FUNCTIONS --- //

        // Start the RTOS task.
        static void initTask(void *t_this);
        // Called on each tick of the task loop.
        void tickTask();

    public:

        // --- Operators --- // 
        SerialSystem(uint32_t t_baud);
        ~SerialSystem();

        // --- SYSTEM OVERRIDES --- //

        // * Unused in this case, returning true.
        bool requestUpdate(State &t_state) { return true; };
        bool responseUpdate(State &t_state) { return true; };

        // --- LOGGER OVERRIDES --- //

        // Called by LogController when a message is to be displayed (ref to the string in the vector)
        void onMessage(std::shared_ptr<std::string> t_msg);

        // --- CLASS FUNCTIONS --- //
        // Send a string message over UART (this is queued for send, not directly)
        void send(std::shared_ptr<std::string> t_msg);
    };

} // namespace RaveBuddy
