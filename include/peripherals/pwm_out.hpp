#ifndef __INCLUDE_MAIN_PWM_OUT_HPP
#define __INCLUDE_MAIN_PWM_OUT_HPP

#include <Arduino.h>
#include <M5Unified.h>

class PwmOutServo
{
public:
    static constexpr uint32_t TIMER_FREQUENCY = 50;
    static constexpr uint32_t TIMER_RESOLUTION = 14;
    static constexpr uint32_t TIMER_RESOLUTION_VAL = 16384;
    static constexpr uint32_t MIN_TIME_US = 1095;
    static constexpr uint32_t MAX_TIME_US = 1895;

    static constexpr uint32_t PWM_US_WIDTH = (MAX_TIME_US - MIN_TIME_US) / 2;
    static constexpr uint32_t PWM_US_CENTER = (MAX_TIME_US + MIN_TIME_US) / 2;
    static constexpr float PWM_US_TO_CNT = TIMER_RESOLUTION_VAL / (1000000.0 / TIMER_FREQUENCY);

    void begin(uint8_t pin, uint8_t channel, float max = 1.0, float min = -1.0, bool inverted = false);
    uint32_t out(float in);

private:
    uint8_t channel;
    bool inverted;
    float max;
    float min;
};

#endif // __INCLUDE_MAIN_PWM_OUT_HPP
