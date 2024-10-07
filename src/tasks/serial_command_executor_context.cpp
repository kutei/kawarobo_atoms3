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
    }else if(strcmp(const_cast<const char *>(this->_cmd_args[0].data()), "s") == 0){
        this->_stream->printf(
            "1:%+3.02f, 2:%+3.02f, 3:%+3.02f, 4:%+3.02f, b:%+3.02f, r:%+3.02f, "
            "m:%+3.02f, slp:%4d, 9:%4d,10:%4d, fs:%1d, lf:%1d",
            g_sbus2_ch[0], g_sbus2_ch[1], g_sbus2_ch[2], g_sbus2_ch[3],
            g_motor_output[0], g_motor_output[1],
            sqrt(g_movement_power_square), g_start_pose_sleep_counter,
            g_sbus2.getChannel(8), g_sbus2.getChannel(9),
            g_sbus2.isFailsafe(), g_sbus2.isLostframe()
        );
        this->_send_br();
        return;
    }else if(strcmp(const_cast<const char *>(this->_cmd_args[0].data()), "e") == 0){
        this->_stream->printf(
            "enc:%d,%d,%d", g_enc_boom.is_initialized(), g_enc_boom.is_on_upper_side(), g_enc_boom.get_angle()
        );
        this->_send_br();
        return;
    }else if(strcmp(const_cast<const char *>(this->_cmd_args[0].data()), "c") == 0){
        this->_stream->printf(
            "r_stat:%d, c_stat:%d", g_robot_status, g_control_status
        );
        this->_send_br();
        return;
    }else if(strcmp(const_cast<const char *>(this->_cmd_args[0].data()), "l") == 0){
        this->_stream->printf(
            "pid: %d, %d, %.4f", g_pid_boom.get_in(), g_pid_boom.get_target(), g_motor_output[0]
        );
        this->_send_br();
        return;
    }else if(strcmp(const_cast<const char *>(this->_cmd_args[0].data()), "mot") == 0){
        double v_boom = atof(this->_cmd_args[1].data());
        double v_roll = atof(this->_cmd_args[2].data());
        int msec = atoi(this->_cmd_args[3].data());

        if(v_boom > 1.0) v_boom = 1.0;
        if(v_boom < -1.0) v_boom = -1.0;
        if(v_roll > 1.0) v_roll = 1.0;
        if(v_roll < -1.0) v_roll = -1.0;
        if(msec < 0) msec = 0;
        if(msec > 1000) msec = 1000;
        this->_stream->printf("mot: %.03f, %.03f, %d", v_boom, v_roll, msec);

        g_motor_boom.out(v_boom);
        g_motor_roll.out(v_roll);

        vTaskDelay(pdMS_TO_TICKS(msec));

        g_motor_boom.out(0);
        g_motor_roll.out(0);
        this->_send_br();

        return;
    }else if(strcmp(const_cast<const char *>(this->_cmd_args[0].data()), "act") == 0){
        g_control_loop_active = true;
        return;
    }else if(strcmp(const_cast<const char *>(this->_cmd_args[0].data()), "deact") == 0){
        g_control_loop_active = false;
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
