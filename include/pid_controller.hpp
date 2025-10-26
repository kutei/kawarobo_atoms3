#ifndef __INCLUDE_PID_CONTROLLER_HPP__
#define __INCLUDE_PID_CONTROLLER_HPP__

#include <stdint.h>
#include <cstddef>

#define MOVING_AVERAGE_LEN 10

class PidController{
public:
    void begin(float Kp, float Ki, float Kd, float dt, float max_integral, int32_t max_target, int32_t min_target);

    void reset();
    void set_target(int32_t target);

    float get_integral(void) { return this->_integral; }
    int32_t get_error(void) { return this->_pre_err; }
    int32_t get_target() { return this->_target; }
    int32_t get_out() { return this->_out; }
    int32_t get_in() { return this->_in; }
    float get_term_p() { return this->_term_p; }
    float get_term_i() { return this->_term_i; }
    float get_term_d() { return this->_term_d; }
    int32_t step(int32_t in);

private:
    float _Kp = 0;
    float _Ki = 0;
    float _Kd = 0;
    float _dt = 0;

    int32_t _target;
    float _integral;
    int32_t _pre_err;
    float _max_integral;
    int32_t _max_target;
    int32_t _min_target;
    int32_t _out = 0;
    int32_t _in = 0;
    float _term_p = 0;
    float _term_i = 0;
    float _term_d = 0;

    int32_t mov_avg_que[MOVING_AVERAGE_LEN] = {0};
    std::size_t mov_avg_que_idx = 0;
};


#endif /* __INCLUDE_PID_CONTROLLER_HPP__ */
