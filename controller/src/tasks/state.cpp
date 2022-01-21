#include "state.h"
#include "ble_task.h"
#include <Arduino.h>
#include <heltec.h>
RaveSysState sysState = {0};

void state_init(int myId){
    sysState.myState = {0};
    sysState.myState.id = myId;
    sysState.myState.mine = true;
    sysState.myState.state = "init";
    sysState.myState.timestamp = millis();
    for(int i = 0; i < 8; i++){
        sysState.states[i] = {0};
        sysState.states[i].id = i;
        sysState.states[i].mine = false;
        sysState.states[i].state = "init";
        sysState.states[i].timestamp = millis();
    }
    
}

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

RaveDeviceState* state_get_mine()
{
    return &sysState.myState;
}