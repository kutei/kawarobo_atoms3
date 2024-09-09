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


/**********************************************************************
 * Global variables
 *********************************************************************/
extern Sbus2Reciever sbus2;
extern EncReciever enc_boom;
extern PwmOutServo motor_boom;
extern PwmOutServo motor_roll;

extern volatile int lcd_bottom_rect_x1, lcd_bottom_rect_x2, lcd_bottom_rect_x3;
extern volatile int lcd_bottom_rect_y1, lcd_bottom_rect_y2;

extern int core1_alive_count;

extern enum RobotStatus robot_status;
extern enum ControlStatus control_status;

extern float sbus2_ch[4];
extern float motor_output[2];
extern float movement_power_square;

extern uint32_t start_pose_sleep_counter;


/**********************************************************************
 * Initialize Function
 *********************************************************************/
void calculate_global_constants();

#endif // __INCLUDE_GLOBAL_VARIABLES_HPP
