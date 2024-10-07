#ifndef __INCLUDE_TASKS_CORE_COUNTER_CONTEXT_HPP
#define __INCLUDE_TASKS_CORE_COUNTER_CONTEXT_HPP

#include "task_controller.hpp"


class Core1CounterContext : public AbstractRtosTaskContext
{
public:
    Core1CounterContext(RtosTaskConfigSharedPtr config) : AbstractRtosTaskContext(config) {};
    void onExecute();
};


#endif // __INCLUDE_TASKS_CORE_COUNTER_CONTEXT_HPP
