#pragma once

namespace RaveBuddy
{
    class GeoPoint
    {
    private:
        float m_lat;
        float m_lon;
    public:
        GeoPoint(double t_lat, double t_lon);
        GeoPoint();
        
        ~GeoPoint();
    };
    
    
} // namespace RaveBuddy
