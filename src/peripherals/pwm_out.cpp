#include "peripherals/pwm_out.hpp"

void PwmOutServo::begin(uint8_t pin, uint8_t channel, float max, float min, bool inverted)
{
    this->channel = channel;
    this->inverted = inverted;
    this->max = max;
    this->min = min;
    ledcSetup(this->channel, PwmOutServo::TIMER_FREQUENCY, PwmOutServo::TIMER_RESOLUTION);
    ledcAttachPin(pin, this->channel);

    this->out(0.0);
}

uint32_t PwmOutServo::out(float in)
{
    if(this->inverted) in *= -1;

    if(in > this->max) in = this->max;
    if(in < this->min) in = this->min;

    float sig_us = in * PwmOutServo::PWM_US_WIDTH + PwmOutServo::PWM_US_CENTER;
    uint32_t sig_cnt = sig_us * PwmOutServo::PWM_US_TO_CNT;
    ledcWrite(this->channel, sig_cnt);
    return sig_cnt;
}
