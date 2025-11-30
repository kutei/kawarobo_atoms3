#ifndef __INCLUDE_MAIN_TAKS_CONTROLLER_HPP
#define __INCLUDE_MAIN_TAKS_CONTROLLER_HPP

#include <M5Unified.h>

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

#include <memory>

typedef struct{
    bool start_required;
    const char *name;
    TaskHandle_t thand;
    BaseType_t repeated;
    TickType_t period;
    TickType_t initial;
    uint32_t stack_size;
    UBaseType_t priority;
    BaseType_t core_id;
} RtosTaskConfig_typedef;

using RtosTaskConfigSharedPtr = std::shared_ptr<RtosTaskConfig_typedef>;


class AbstractRtosTaskContext{
public:
    AbstractRtosTaskContext(RtosTaskConfigSharedPtr config);
    AbstractRtosTaskContext();

    inline void setConfig(RtosTaskConfigSharedPtr config) { this->_config = std::move(config); };
    inline RtosTaskConfigSharedPtr getConfig() { return this->_config; };
    inline bool isStarted() { return this->_is_started; };
    inline void setStarted(bool started) { this->_is_started = started; };
    virtual void onActivated() {};
    virtual void onExecute() = 0;
    void onExecutePre() { _start_time = micros(); }
    unsigned long getElapsedUs() { return micros() - _start_time; }

private:
    RtosTaskConfigSharedPtr _config;
    bool _is_started;
    unsigned long _start_time;
};

using AbstractRtosTaskContextRawPtr = AbstractRtosTaskContext *;
using AbstractRtosTaskContextSharedPtr = std::shared_ptr<AbstractRtosTaskContext>;


int16_t task_start(AbstractRtosTaskContextSharedPtr *task_context, std::size_t size);


#endif // __INCLUDE_MAIN_TAKS_CONTROLLER_HPP
