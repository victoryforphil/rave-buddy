#pragma once

#include "GeoPoint.hpp"
#include <string>
namespace RaveBuddy
{
    class State
    {
        
    private:
        /* data */
        uint8_t m_unitId;
        std::string m_unitName; 
        GeoPoint m_location;
        uint64_t m_timestamp;
        std::string m_status;
        

    public:

        State(/* args */);
        ~State();

    }
    
} // namespace RaveBuddy
