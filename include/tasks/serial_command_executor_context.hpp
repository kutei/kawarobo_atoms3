#ifndef __INCLUDE_TASKS_SERIAL_COMMAND_EXECUTOR_CONTEXT_HPP
#define __INCLUDE_TASKS_SERIAL_COMMAND_EXECUTOR_CONTEXT_HPP

#include "task_controller.hpp"
#include <M5Unified.h>

#include <array>
#include <string>
#include <vector>

class SerialCommandExecutorContext : public AbstractRtosTaskContext
{
public:
    static constexpr int MAX_CMD_NUM = 30;  // コマンド名を含む最大の引数数
    static constexpr int MAX_CMD_LEN = 50;  // 1つの引数の最大文字数
    static constexpr std::array<const char *, 2> KILL_SIGNAL_CHARS = {"Ctrl+C", "Ctrl+["};
    static constexpr std::array<char, 2> KILL_SIGNAL_ANSI = {3, 27};

    SerialCommandExecutorContext(RtosTaskConfigSharedPtr config, Stream *stream);
    void onActivated() override;
    void onExecute() override;

private:
    Stream *_stream;
    std::vector<char> _latest_cmdbuf;
    std::array<std::array<char, MAX_CMD_LEN>, MAX_CMD_NUM> _cmd_args;

    void _execute_command();

    bool _is_kill_char(char c);
    void _send_br();
    void _reset_parse();
};



#endif // __INCLUDE_TASKS_SERIAL_COMMAND_EXECUTOR_CONTEXT_HPP
