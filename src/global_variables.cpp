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
Sbus2Reciever sbus2;
EncReciever enc_boom;
PwmOutServo motor_boom;
PwmOutServo motor_roll;

volatile int lcd_bottom_rect_x1, lcd_bottom_rect_x2, lcd_bottom_rect_x3;
volatile int lcd_bottom_rect_y1, lcd_bottom_rect_y2;

int core1_alive_count = 0;

enum RobotStatus robot_status = RobotStatus::RSTAT_WAITING_STABILIZED;
enum ControlStatus control_status = ControlStatus::CSTAT_NORMAL;

float sbus2_ch[4] = { 0.0 };
float motor_output[2] = { 0.0 };
float movement_power_square = 0.0;

uint32_t start_pose_sleep_counter = 0;


/**********************************************************************
 * Initialize Function
 *********************************************************************/
void calculate_global_constants(){
    lcd_bottom_rect_x1 = M5.Lcd.width()*0.9;
    lcd_bottom_rect_x2 = M5.Lcd.width()*0.95;
    lcd_bottom_rect_x3 = M5.Lcd.width()*1.0;
    lcd_bottom_rect_y1 = M5.Lcd.height()*0.95;
    lcd_bottom_rect_y2 = M5.Lcd.height()*1.0;
}



#endif // __INCLUDE_GLOBAL_VARIABLES_HPP
