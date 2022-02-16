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

        // --- MEMBER ACCESORS --- //
        State* setUintId(uint8_t t_id) {
            m_unitId = t_id;
             return this;
        };
        State* setUnitName(std::string t_name){
            m_unitName = t_name;
             return this;
        };
        State* setLocation(int64_t t_lat, int64_t t_lon){
            m_location = GeoPoint(t_lat, t_lon);
            return this;
        };
        State* setTimestamp(uint64_t t_time){
            m_timestamp = t_time;
             return this;
        };
        State* setStatus(std::string t_status){
            m_status = t_status;
             return this;
        };

        uint8_t getId(){ return m_unitId;};
        GeoPoint getLocation(){
            return m_location;
        }

        std::string getStatus(){
            return m_status;
        }

    };
    
} // namespace RaveBuddy
