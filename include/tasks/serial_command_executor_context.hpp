#ifndef __INCLUDE_TASKS_SERIAL_COMMAND_EXECUTOR_CONTEXT_HPP
#define __INCLUDE_TASKS_SERIAL_COMMAND_EXECUTOR_CONTEXT_HPP

#include "task_controller.hpp"


class SerialCommandExecutorContext : public AbstractRtosTaskContext
{
public:
    SerialCommandExecutorContext(RtosTaskConfigSharedPtr config) : AbstractRtosTaskContext(config) {};
    void onExecute();
};



#endif // __INCLUDE_TASKS_SERIAL_COMMAND_EXECUTOR_CONTEXT_HPP
