#ifndef __INCLUDE_TASKS_CONTROL_LOOP_CONTEXT_HPP
#define __INCLUDE_TASKS_CONTROL_LOOP_CONTEXT_HPP

#include "task_controller.hpp"


class ControlLoopContext : public AbstractRtosTaskContext
{
public:
    ControlLoopContext(RtosTaskConfigSharedPtr config) : AbstractRtosTaskContext(config) {};
    void onExecute();
};



#endif // __INCLUDE_TASKS_CONTROL_LOOP_CONTEXT_HPP
