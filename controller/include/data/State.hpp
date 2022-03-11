#pragma once

#include "logging/LogController.hpp"
#include "GeoPoint.hpp"
#include <string>

#define STATE_STRIKES 5
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
        uint32_t m_timestamp = 0;
        std::string m_status = "";
        uint8_t m_timeoutStrikes = STATE_STRIKES;

    public:



        // --- MEMBER ACCESORS --- //
        uint8_t getTimeout(){
            return m_timeoutStrikes;
        }

        bool isTimedout(){
            if(m_mine){
                return false;
            }
            return m_timeoutStrikes <= 0;
        }

        State* timeoutStrike(){
            if(m_mine){
                return this;
            }
            m_timeoutStrikes--;
            return this;
        }

        State* resetStrikes(){
            m_timeoutStrikes = STATE_STRIKES;
            return this;
        }



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
        
        State* setSats(auto sats){
            m_sats = sats;
            return this;
        }

        State* setTimestamp(uint32_t t_time){
            m_timestamp = t_time;
             return this;
        };
        State* setStatus(std::string t_status){
            m_status = t_status;
             return this;
        };


        int getSats(){
            return m_sats;
        }
        std::string getName(){
            return m_unitName;
        }
        uint8_t getId(){ return m_unitId;};
        GeoPoint getLocation(){
            return m_location;
        }
        bool isMine(){return m_mine;}
        uint32_t getTimestamp(){
            return m_timestamp;
        }
        std::string getStatus(){
            return m_status;
        }

    };
    
} // namespace RaveBuddy
