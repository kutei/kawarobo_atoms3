#pragma once

/**********************************************************************
 * Include
 *********************************************************************/
#include "global_constants.hpp"
#include "task_controller.hpp"
#include "pid_controller.hpp"
#include "peripherals/pwm_out.hpp"
#include "peripherals/enc_reciever.hpp"

#include <MadgwickAHRS.h>
#include <Sbus2Reciever.hpp>
#include <atomic>

/**********************************************************************
 * Struct Definitions
 *********************************************************************/
typedef struct {
    float roll;
    float pitch;
    float yaw;
} ImuEulerAngle;

typedef struct {
    unsigned long exec_time;        // 実行時間
    unsigned long interval_time;    // 実行間隔
    unsigned long sleep_time;       // スリープ時間
} ControlTimeLog;

/**********************************************************************
 * Global variables
 *********************************************************************/
extern Sbus2Reciever g_sbus2;
extern EncReciever g_enc_boom;
extern PwmOutServo g_motor_boom;
extern PwmOutServo g_motor_roll;
extern PidController g_pid_boom;

extern std::atomic<int> g_core1_alive_count;

extern std::atomic<bool> g_initialized;
extern enum RobotStatus g_robot_status;
extern enum ControlStatus g_control_status;

extern float g_sbus2_ch[4];
extern float g_motor_output[2];
extern float g_movement_power_square;
extern float g_digital_trim[2];

extern Madgwick g_imu_filter;
extern ImuEulerAngle g_imu_euler_angle;

extern uint32_t g_start_pose_sleep_counter;     // 初期状態への遷移を決めるためのカウンタ
extern std::atomic<bool> g_control_loop_active; // 制御ループがアクティブかどうか
extern ControlTimeLog g_control_loop_time;      // 制御ループの実行時間
extern ControlTimeLog g_imu_calc_time;          // IMU計算の実行時間


/**********************************************************************
 * Initialize Function
 *********************************************************************/
void calculate_global_constants();
