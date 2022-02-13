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

        // --- MEMBER ACCESORS --- //
        State* setUintId(uint8_t t_id);
        State* setUnitName(std::string t_name);
        State* setLocation(int64_t t_lat, int64_t t_lon);
        State* setTimestamp(uint64_t t_time);
        State* setStatus(std::string t_status);

    };
    
} // namespace RaveBuddy
