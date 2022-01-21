#ifndef _STATE_H_
#define _STATE_H_
#include <Arduino.h>
struct RaveDeviceState_t;
struct RaveSysState_t;
typedef struct RaveDeviceState_t{
    bool init;
    bool mine;
    int id;
    String state;
    long timestamp;
} RaveDeviceState;

typedef  struct RaveSysState_t{
    RaveDeviceState myState;
    RaveDeviceState states[8];
} RaveSysState;

void state_init(int myId);
void state_on_state(int id, String state, long timestamp);
RaveDeviceState* state_get_mine();

#endif