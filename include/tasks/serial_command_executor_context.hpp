#ifndef __INCLUDE_TASKS_SERIAL_COMMAND_EXECUTOR_CONTEXT_HPP
#define __INCLUDE_TASKS_SERIAL_COMMAND_EXECUTOR_CONTEXT_HPP

#include "task_controller.hpp"
#include <M5Unified.h>


class SerialCommandExecutorContext : public AbstractRtosTaskContext
{
public:
    SerialCommandExecutorContext(RtosTaskConfigSharedPtr config, Stream *stream, M5GFX *disp)
        : AbstractRtosTaskContext(config), _stream(stream), _disp(disp) {};
    void onExecute();

private:
    Stream *_stream;
    M5GFX *_disp;
};



#endif // __INCLUDE_TASKS_SERIAL_COMMAND_EXECUTOR_CONTEXT_HPP
