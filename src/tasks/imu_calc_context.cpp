#include "tasks/imu_calc_context.hpp"
#include "task_controller.hpp"
#include "global_constants.hpp"
#include "global_variables.hpp"


void ImuCalcContext::onExecute()
{
    if (!g_initialized) return;

    m5::IMU_Class::imu_data_t data;
    unsigned long start_time = micros();

    // 実行時間と間隔時間を記録
    unsigned long interval = start_time - this->_prev_exec_time;
    this->_prev_exec_time = start_time;

    //////////////////////////////////////////////////////////////
    // IMUの更新
    //////////////////////////////////////////////////////////////
    float ax, ay, az;
    float gx, gy, gz;
    M5.Imu.update();
    M5.Imu.getImuData(&data);

    g_imu_filter.updateIMU(
        data.gyro.x, data.gyro.y, data.gyro.z, data.accel.x, data.accel.y, data.accel.z);
    g_imu_euler_angle.roll = g_imu_filter.getRoll();
    g_imu_euler_angle.pitch = g_imu_filter.getPitch();
    g_imu_euler_angle.yaw = g_imu_filter.getYaw();

    // IMU計算時間をグローバル変数に記録
    g_imu_calc_time.exec_time = micros() - start_time;
    g_imu_calc_time.interval_time = interval;
}
