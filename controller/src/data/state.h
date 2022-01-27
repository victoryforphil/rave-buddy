#ifndef _STATE_H_
#define _STATE_H_
#include <stdint.h>
#include <string>

struct GPSState{
    uint64_t lat;
    uint64_t lon;
    uint64_t alt;
    bool fix;
    uint8_t n_sats;
};

struct StatusState{
    std::string status;
    uint64_t timestamp; 
};

struct RaveStaste{
    GPSState gps;
    StatusState status;
};



#endif