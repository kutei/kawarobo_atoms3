#include "pid_controller.hpp"


void PidController::begin(
    float Kp, float Ki, float Kd, float dt,
    int32_t max_integral, int32_t max_target, int32_t min_target)
{
    this->_Kp = Kp;
    this->_Ki = Ki;
    this->_Kd = Kd;
    this->_dt = dt;
    this->_max_integral = max_integral;
    this->_max_target = max_target;
    this->_min_target = min_target;
    this->reset();
}

void PidController::reset(){
    this->_target = 0;
    this->_integral = 0;
    this->_pre_err = 0;
    for(size_t i = 0; i < MOVING_AVERAGE_LEN; i++) this->mov_avg_que[i] = 0;
}

void PidController::set_target(int32_t target){
    if(target > this->_max_target){
        this->_target = this->_max_target;
        return;
    }
    if(target < this->_min_target){
        this->_target = this->_min_target;
        return;
    }
    this->_target = target;
}

float PidController::get_integral(void){
    return this->_integral;
}
int32_t PidController::get_error(void){
    return this->_pre_err;
}
int32_t PidController::get_target(){
    return this->_target;
}
int32_t PidController::get_out(){
    return this->_out;
}
int32_t PidController::get_in(){
    return this->_in;
}

int32_t PidController::step(int32_t in){
    // targetを移動平均で平滑化
    this->mov_avg_que[this->mov_avg_que_idx] = this->_target;
    this->mov_avg_que_idx++;
    if(this->mov_avg_que_idx >= MOVING_AVERAGE_LEN){
        this->mov_avg_que_idx = 0;
    }
    int64_t sum = 0;
    for(size_t i = 0; i < MOVING_AVERAGE_LEN; i++) sum += this->mov_avg_que[i];
    int32_t target = sum / MOVING_AVERAGE_LEN;

    // 偏差と積分値の計算
    int32_t err = target - in;
    this->_integral += err * this->_dt;
    if(this->_integral >  this->_max_integral) this->_integral =  this->_max_integral;
    if(this->_integral < -this->_max_integral) this->_integral = -this->_max_integral;

    // PIDの公式を実行
    float p_act = this->_Kp * err;
    float i_act = this->_Ki * this->_integral;
    float d_act = this->_Kd * (err - this->_pre_err) / this->_dt;
    int32_t output = static_cast<int32_t>(p_act + i_act + d_act);

    // 微分項用に今回の値を保存
    this->_pre_err = err;

    this->_out = output;
    this->_in = in;
    return output;
}
