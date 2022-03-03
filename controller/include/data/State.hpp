#pragma once

#include "logging/LogController.hpp"
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
        bool m_mine = false;
        GeoPoint m_location;
        uint8_t m_sats;
        uint8_t m_currentLed;
        uint64_t m_timestamp;
        std::string m_status = "Default Status";
        

    public:



        // --- MEMBER ACCESORS --- //
        State* setUintId(uint8_t t_id, bool t_mine=false) {
            m_unitId = t_id;
            m_mine = t_mine;
            m_unitName = "Unit #" + TO_STRING(m_unitId);
            return this;
        };
        State* setUnitName(std::string t_name){
            m_unitName = t_name;
             return this;
        };
        State* setLocation(auto t_lat, auto t_lon){
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
        std::string getName(){
            return m_unitName;
        }
        uint8_t getId(){ return m_unitId;};
        GeoPoint getLocation(){
            return m_location;
        }
        bool isMine(){return m_mine;}

        std::string getStatus(){
            return m_status;
        }

    };
    
} // namespace RaveBuddy
