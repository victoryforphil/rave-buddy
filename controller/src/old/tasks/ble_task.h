#ifndef _TASKS_BLE_TASK_H_
#define _TASKS_BLE_TASK_H_
#include "state.h"
void task_ble_init(void *pvParameters);
void task_ble_tick();
void ble_new_device(int id);
void ble_sync_state(RaveDeviceState* state);
#endif