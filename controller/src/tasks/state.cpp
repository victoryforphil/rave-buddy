#include "state.h"
#include "ble_task.h"
#include <Arduino.h>
RaveSysState sysState = {0};

void state_on_state(int id,String state, long timestamp)
{
    Serial.printf("State/Update: %d, %s, %ld\n", id, state.c_str(), timestamp);

    if (sysState.states[id].init == 0)
    {
        Serial.printf("State/Update: New Device: %d\n", id);
        sysState.states[id].id = id;
        sysState.states[id].init = 1;
        ble_new_device(id);
    }else{
        sysState.states[id].timestamp = timestamp;
        sysState.states[id].state = state;
        ble_sync_state(&sysState.states[id]);
    }

    
    
    
}