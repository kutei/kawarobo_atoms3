#ifndef __INCLUDE_TASKS_IMU_CALC_CONTEXT_HPP
#define __INCLUDE_TASKS_IMU_CALC_CONTEXT_HPP

#include "task_controller.hpp"

#include <array>


class ImuCalcContext : public AbstractRtosTaskContext
{
public:
    ImuCalcContext(RtosTaskConfigSharedPtr config) : AbstractRtosTaskContext(config) {};
    void onExecute();

private:
    unsigned long _prev_exec_time = 0;
};



#endif // __INCLUDE_TASKS_IMU_CALC_CONTEXT_HPP
