#ifndef __INCLUDE_GLOBAL_VARIABLES_HPP
#define __INCLUDE_GLOBAL_VARIABLES_HPP

/**********************************************************************
 * Include
 *********************************************************************/
#include "global_constants.hpp"
#include "task_controller.hpp"
#include "peripherals/pwm_out.hpp"
#include "peripherals/enc_reciever.hpp"

#include <Sbus2Reciever.hpp>
#include <atomic>


/**********************************************************************
 * Global variables
 *********************************************************************/
extern Sbus2Reciever g_sbus2;
extern EncReciever g_enc_boom;
extern PwmOutServo g_motor_boom;
extern PwmOutServo g_motor_roll;

extern std::atomic<int> g_core1_alive_count;

extern enum RobotStatus g_robot_status;
extern enum ControlStatus g_control_status;

extern float g_sbus2_ch[4];
extern float g_motor_output[2];
extern float g_movement_power_square;

extern uint32_t g_start_pose_sleep_counter;


/**********************************************************************
 * Initialize Function
 *********************************************************************/
void calculate_global_constants();

#endif // __INCLUDE_GLOBAL_VARIABLES_HPP
