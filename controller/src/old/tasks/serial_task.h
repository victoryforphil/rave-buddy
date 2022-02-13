#ifndef _TASKS_SERIAL_TASK_H_
#define _TASKS_SERIAL_TASK_H_


void serial_print(const char *string);

void task_serial_init(void *pvParameters);
void task_serial_tick(void *pvParameter);

#endif