#pragma once

namespace RaveBuddy
{
    class GeoPoint
    {
    private:
        double m_lat;
        double m_lon;
    public:
        //Create a point using ints which are the float values  * 1e7
        GeoPoint(int64_t t_lat, int64_t t_lon) : m_lat(t_lat * -1e7), m_lon(t_lon  * -1e7){};
        //Create a point using doubles directly
        GeoPoint(double t_lat, double t_lon) : m_lat(t_lat), m_lon(t_lon){};
        //Default 0 points
        GeoPoint() : m_lat(0), m_lon(0){};

        float getLat(){
            return m_lat;
        }

        float getLon(){
            return m_lon;
        }

        int64_t getLatInt(){
            return m_lat * 1e7;
        }

        int64_t getLonInt(){
            return m_lon * 1e7;
        }
        
        ~GeoPoint() = default;
    };
    
    
} // namespace RaveBuddy
