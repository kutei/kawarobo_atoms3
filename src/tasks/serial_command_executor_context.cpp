#include "tasks/serial_command_executor_context.hpp"
#include "global_constants.hpp"
#include "global_variables.hpp"


SerialCommandExecutorContext::SerialCommandExecutorContext(RtosTaskConfigSharedPtr config, Stream *stream)
    : AbstractRtosTaskContext(config), _stream(stream)
{
    ;
}


void SerialCommandExecutorContext::onActivated()
{
    this->_stream->print("##################################################"); this->_send_br();
    this->_stream->print("##              Command Line Tools              ##"); this->_send_br();
    this->_stream->print("##################################################"); this->_send_br();
    this->_reset_parse();
}


void SerialCommandExecutorContext::onExecute()
{
    bool is_exec_required = false;

    // シリアル通信の受信バッファを読み取り、コマンドを解釈する
    while(this->_stream->available() > 0){
        char c = this->_stream->read();
        if(this->_is_kill_char(c)){
            this->_stream->print("^C");
            this->_send_br();
            this->_reset_parse();
            continue;
        }

        // BSの処理をしつつ、表示
        if(c == '\b'){
            if(this->_latest_cmdbuf.size() > 0){
                this->_latest_cmdbuf.pop_back();
                this->_stream->print("\b \b");
            }
        }else if(c == '\n'){
            is_exec_required = true;
            this->_send_br();
            break;
        }else if(c == '\r'){
            is_exec_required = true;
        }else{
            this->_latest_cmdbuf.push_back(c);
            this->_stream->print(c);
        }
    }
    if(!is_exec_required) return;

    // コマンドのパース実行
    int cmd_arg_num = 0;
    int cmd_arg_chars = 0;
    for(char c : this->_latest_cmdbuf){
        if(c == ' ' | c == '\r' | c == '\n'){
            this->_cmd_args[cmd_arg_num][cmd_arg_chars] = '\0';
            cmd_arg_num++;
            cmd_arg_chars = 0;
            if(cmd_arg_num >= this->_cmd_args.size()) break;
        }else{
            this->_cmd_args[cmd_arg_num][cmd_arg_chars] = c;
            cmd_arg_chars++;
        }
    }
    this->_cmd_args[cmd_arg_num][cmd_arg_chars] = '\0';

    // コマンドの実行
    this->_execute_command();
    this->_reset_parse();
}


void SerialCommandExecutorContext::_execute_command()
{
    if(strcmp(const_cast<const char *>(this->_cmd_args[0].data()), "echo") == 0){
        for(auto arg: this->_cmd_args){
            if(arg[0] == '\0') continue;
            this->_stream->print(arg.data()); this->_send_br();
        }
        return;
    }

    if(this->_cmd_args[0][0] != '\0'){
        this->_stream->printf("error: command not found: %s", this->_cmd_args[0].data());
        this->_send_br();
    }
}


bool SerialCommandExecutorContext::_is_kill_char(char c)
{
    for(int i = 0; i < KILL_SIGNAL_CHARS.size(); i++){
        if(c == KILL_SIGNAL_ANSI[i]){
            return true;
        }
    }
    return false;
}


void SerialCommandExecutorContext::_send_br()
{
    this->_stream->print("\r\n");
}


void SerialCommandExecutorContext::_reset_parse()
{
    this->_stream->print("$ ");
    this->_latest_cmdbuf.clear();
    for(int i = 0; i < this->_cmd_args.size(); i++){
        this->_cmd_args[i][0] = '\0';
    }
}
