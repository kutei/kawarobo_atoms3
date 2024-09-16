#ifndef __INCLUDE_TASKS_SERIAL_COMMAND_EXECUTOR_CONTEXT_HPP
#define __INCLUDE_TASKS_SERIAL_COMMAND_EXECUTOR_CONTEXT_HPP

#include "task_controller.hpp"
#include <M5Unified.h>

#include <array>

class SerialCommandExecutorContext : public AbstractRtosTaskContext
{
public:
    // static constexpr std::array<String, 2> KILL_SIGNAL_CHAR = {"Ctrl+C", "Ctrl+["};
    // constexpr std::array<char, 2> KILL_SIGNAL_CHAR = {3, 27};

    SerialCommandExecutorContext(RtosTaskConfigSharedPtr config, Stream *stream)
        : AbstractRtosTaskContext(config), _stream(stream) {};
    void onExecute();

private:
    Stream *_stream;
};



#endif // __INCLUDE_TASKS_SERIAL_COMMAND_EXECUTOR_CONTEXT_HPP
