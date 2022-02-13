#include "SerialSystem.hpp"

using namespace RaveBuddy;

SerialSystem::SerialSystem(uint32_t t_baud) : m_baud(t_baud)
{
    
    Serial.begin(m_baud);

}