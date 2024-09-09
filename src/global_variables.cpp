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
Sbus2Reciever g_sbus2;
EncReciever g_enc_boom;
PwmOutServo g_motor_boom;
PwmOutServo g_motor_roll;

volatile int g_lcd_bottom_rect_x1, g_lcd_bottom_rect_x2, g_lcd_bottom_rect_x3;
volatile int g_lcd_bottom_rect_y1, g_lcd_bottom_rect_y2;

int g_core1_alive_count = 0;

enum RobotStatus g_robot_status = RobotStatus::RSTAT_WAITING_STABILIZED;
enum ControlStatus g_control_status = ControlStatus::CSTAT_NORMAL;

float g_sbus2_ch[4] = { 0.0 };
float g_motor_output[2] = { 0.0 };
float g_movement_power_square = 0.0;

uint32_t g_start_pose_sleep_counter = 0;


/**********************************************************************
 * Initialize Function
 *********************************************************************/
void calculate_global_constants(){
    g_lcd_bottom_rect_x1 = M5.Lcd.width()*0.9;
    g_lcd_bottom_rect_x2 = M5.Lcd.width()*0.95;
    g_lcd_bottom_rect_x3 = M5.Lcd.width()*1.0;
    g_lcd_bottom_rect_y1 = M5.Lcd.height()*0.95;
    g_lcd_bottom_rect_y2 = M5.Lcd.height()*1.0;
}



#endif // __INCLUDE_GLOBAL_VARIABLES_HPP
