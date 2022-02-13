#pragma once

// --- SYSTEM INCLUDES --- //
#include <string>
#include <Arduino.h>

// --- LOCAL INCLUDES --- //
#include "logging/LogController.hpp"
#include "logging/Logger.hpp"
#include "systems/System.hpp"

// --- PREPROC DEFS --- //
#define SYSTEM_SERIAL_DELAY 20
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
        const SemaphoreHandle_t m_serialMutex   = xSemaphoreCreateMutex();;
        // Queue to store messages (string*) that are to be sent
        const xQueueHandle m_messageQueue =  xQueueCreate(SYSTEM_SERIAL_QUEUE_SIZE, sizeof(std::string*));;

        // --- PRIVATE FUNCTIONS --- //

        // Start the RTOS task.
        static void initTask(void *t_this);
        // Called on each tick of the task loop.
        void tickTask();

    public:
        // --- SYSTEM OVERRIDES --- //

        // * Unused in this case, returning true.
        virtual bool requestUpdate(State &t_state) { return true; };
        virtual bool responseUpdate(State &t_state) { return true; };

        // --- LOGGER OVERRIDES --- //

        // Called by LogController when a message is to be displayed (ref to the string in the vector)
        void onMessage(std::string &t_msg);

        // --- CLASS FUNCTIONS --- //
        SerialSystem(uint32_t t_baud);
        ~SerialSystem();
        // Send a string message over UART (this is queued for send, not directly)
        void send(const std::string t_msg);
    };

} // namespace RaveBuddy
