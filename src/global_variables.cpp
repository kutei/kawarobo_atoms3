#ifndef __INCLUDE_GLOBAL_VARIABLES_HPP
#define __INCLUDE_GLOBAL_VARIABLES_HPP

/**********************************************************************
 * Include
 *********************************************************************/
#include "global_constants.hpp"
#include "global_variables.hpp"
#include "task_controller.hpp"
#include "pid_controller.hpp"
#include "peripherals/pwm_out.hpp"
#include "peripherals/enc_reciever.hpp"

#include <Sbus2Reciever.hpp>
#include <atomic>



/**********************************************************************
 * Global variables
 *********************************************************************/
Sbus2Reciever g_sbus2;
EncReciever g_enc_boom;
PwmOutServo g_motor_boom;
PwmOutServo g_motor_roll;
PidController g_pid_boom;

std::atomic<int> g_core1_alive_count(0);

enum RobotStatus g_robot_status = RobotStatus::RSTAT_WAITING_STABILIZED;
enum ControlStatus g_control_status = ControlStatus::CSTAT_NORMAL;

float g_sbus2_ch[4] = { 0.0 };
float g_motor_output[2] = { 0.0 };
float g_movement_power_square = 0.0;

uint32_t g_start_pose_sleep_counter = 0;
std::atomic<bool> g_control_loop_active(true);


/**********************************************************************
 * Initialize Function
 *********************************************************************/
void calculate_global_constants(){
    // currently nothing to do
}



#endif // __INCLUDE_GLOBAL_VARIABLES_HPP
