#ifndef __INCLUDE_MAIN_TAKS_CONTROLLER_HPP
#define __INCLUDE_MAIN_TAKS_CONTROLLER_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

#include <vector>
#include <memory>

typedef struct{
    const char *name;
    TimerHandle_t thand;
    BaseType_t repeated;
    TickType_t period;
    TickType_t initial;
    TimerCallbackFunction_t callback;
} RtosTaskConfig_typedef;

using RtosTaskConfigRawPtr = RtosTaskConfig_typedef *;

int16_t task_initialize(RtosTaskConfigRawPtr *task_configs, std::size_t size);
int16_t task_start(RtosTaskConfigRawPtr *task_configs, std::size_t size);

#endif // __INCLUDE_MAIN_TAKS_CONTROLLER_HPP
