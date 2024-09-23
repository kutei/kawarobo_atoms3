#include "tasks/control_loop_context.hpp"
#include "task_controller.hpp"
#include "global_constants.hpp"
#include "global_variables.hpp"


float conv_sbus2_to_float(uint16_t val, int16_t offset = 0, bool invert = false){
    float ret = ((val + offset) / 1024.0 - 1.0) * 1.57;
    if(ret > 1.0){ ret = 1.0; }
    if(ret < -1.0){ ret = -1.0; }
    if(invert == true){
        ret *= -1.0;
    }
    return ret;
}


void ControlLoopContext::onExecute()
{
    if(!g_control_loop_active) return;

    //////////////////////////////////////////////////////////////
    // 入力取得と状態遷移
    //////////////////////////////////////////////////////////////

    // 初期化中は制御ループはskipする
    bool is_g_robot_status_not_control =
        g_robot_status == RobotStatus::RSTAT_WAITING_STABILIZED
        || g_robot_status == RobotStatus::RSTAT_COUNTING_DOWN
        || g_robot_status == RobotStatus::RSTAT_INITIALIZING;
    if(is_g_robot_status_not_control) return;

    // プロポの入力を取得
    int16_t vtail_ch[4] = { 0 };
    g_sbus2.getVtailChannel(&vtail_ch[0], &vtail_ch[1], &vtail_ch[2], &vtail_ch[3]);
    g_sbus2_ch[0] = conv_sbus2_to_float(vtail_ch[0], 0, false);
    g_sbus2_ch[1] = conv_sbus2_to_float(vtail_ch[1], 0, true);
    g_sbus2_ch[2] = conv_sbus2_to_float(vtail_ch[2], 0, false);
    g_sbus2_ch[3] = conv_sbus2_to_float(vtail_ch[3], 32, false);
    float move_square = POW2(g_sbus2_ch[1]) + POW2(g_sbus2_ch[3]);
    g_movement_power_square = move_square;

    // スリープボタンのカウンタを更新
    if(SBUS2_BUTON_IS_HIGH(g_sbus2.getChannel(9))){
        if(g_start_pose_sleep_counter <= SLEEP_BUTTON_SLEEP_THRESHOLD){
            g_start_pose_sleep_counter++;
        }
    }else{
        g_start_pose_sleep_counter = 0;
    }

    // 状態遷移を実行
    if(g_sbus2.isFailsafe()){
        // Failsafeなら、即座にSLEEPに遷移する
        g_robot_status = RobotStatus::RSTAT_SLEEPING;
    }else{
        if(g_start_pose_sleep_counter == 0 && g_robot_status == RobotStatus::RSTAT_STARTING_POSE_READY){
            g_robot_status = RobotStatus::RSTAT_STARTING_POSE;
        }else if(g_start_pose_sleep_counter >= SLEEP_BUTTON_SLEEP_THRESHOLD){
            g_robot_status = RobotStatus::RSTAT_SLEEPING;
        }else if(g_start_pose_sleep_counter >= SLEEP_BUTTON_START_POSE_THRESHOLD){
            g_robot_status = RobotStatus::RSTAT_STARTING_POSE_READY;
        }
        if(move_square > POW2(UNSLEEP_MOVE_SQRT_THRESHOLD)){
            g_robot_status = RobotStatus::RSTAT_NORMAL;
        }
    }

    // Sleep中はモーター出力を0にする
    if(g_robot_status == RobotStatus::RSTAT_SLEEPING){
        g_motor_boom.out(0.0);
        g_motor_roll.out(0.0);
        return;
    }


    //////////////////////////////////////////////////////////////
    // ロボットのroll軸の制御
    //////////////////////////////////////////////////////////////

    // roll出力を計算
    bool roll_attaking = false;
    float roll_input = g_sbus2_ch[0];
    float roll_input_abs = fabs(roll_input);
    float roll_output = 0.0;
    if(g_robot_status == RobotStatus::RSTAT_STARTING_POSE){
        if(roll_input_abs < ROLL_DEADZONE){
            roll_output = 0.0;
        }
        else if(roll_input < 0.0){
            roll_output = - ROLL_ADJ_SPEED;
        }
        else
        {
            roll_output = ROLL_ADJ_SPEED;
        }
    }else{
        if(roll_input_abs < ROLL_DEADZONE){
            roll_output = 0.0;
        }
        else if(roll_input > ROLL_ADJ_INPUT_MAX)
        {
            roll_attaking = true;
            roll_output = ROLL_ROLLING_SPEED;
        }
        else if(roll_input < 0.0){
            roll_output = - ROLL_ADJ_SPEED;
        }
        else
        {
            roll_output = ROLL_ADJ_SPEED;
        }
    }

    // roll出力をモーターに出力
    g_motor_roll.out(roll_output);
    g_motor_output[1] = roll_output;


    //////////////////////////////////////////////////////////////
    // ロボットのboom軸の制御
    //////////////////////////////////////////////////////////////

    // boomの制御状態を更新
    if(roll_attaking == true){
        g_control_status = ControlStatus::CSTAT_ROLLING;
    }else if(move_square > POW2(BOOM_UP_MOVE_SQRT_THRESHOLD)){
        g_control_status = ControlStatus::CSTAT_BOOM_UP_MOVING;
    }else{
        g_control_status = ControlStatus::CSTAT_NORMAL;
    }

    // boom出力を計算
    int32_t target = 0;
    if(g_control_status == ControlStatus::CSTAT_NORMAL){
        target = BOOM_NORMAL_POSITION + (int32_t)(BOOM_NORMAL_STICK_SENSITIVITY * g_sbus2_ch[2]);
    }else if(g_control_status == ControlStatus::CSTAT_BOOM_UP_MOVING){
        target = BOOM_UP_POSTION + (int32_t)(BOOM_UP_STICK_SENSITIVITY * g_sbus2_ch[2]);
    }else if(g_control_status == ControlStatus::CSTAT_ROLLING){
        target = BOOM_ROLLING_POSITION + (int32_t)(BOOM_ROLLING_STICK_SENSITIVITY * g_sbus2_ch[2]);
    }
    g_pid_boom.set_target(target);
    int32_t out = g_pid_boom.step(g_enc_boom.get_angle());
    float boom_output = out / 6000.0;
    g_motor_boom.out(boom_output);
    g_motor_output[0] = boom_output;
}
