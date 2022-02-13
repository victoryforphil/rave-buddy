#ifndef _TASKS_LORA_TASK_H_
#define _TASKS_LORA_TASK_H_
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

void lora_init();
void task_lora_rec_init(void *pvParameters);
void task_lora_send_init(void *pvParameters);
#endif