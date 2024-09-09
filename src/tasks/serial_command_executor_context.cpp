#include "tasks/serial_command_executor_context.hpp"
#include "global_constants.hpp"
#include "global_variables.hpp"


void SerialCommandExecutorContext::onExecute()
{
#if defined(SERIAL_OUT_SBUS2_RAW) || defined(SERIAL_OUT_CONTROL_STATUS)
    static uint8_t cnt = 0;
    cnt++;
    char head = cnt % 4;
    if(head == 0){ head = '_'; }
    else if(head == 1){ head = '>'; }
    else if(head == 2){ head = '^'; }
    else if(head == 3){ head = '<'; }

    int16_t vtail_ch[4] = { 0 };
    g_sbus2.getVtailChannel(&vtail_ch[0], &vtail_ch[1], &vtail_ch[2], &vtail_ch[3]);

#if defined(SERIAL_OUT_SBUS2_RAW)
    Serial.printf(
        "%c 1:%4d, 2:%4d, 3:%4d, 4:%4d, 5:%4d, 6:%4d, "
        "7:%4d, 8:%4d, 9:%4d,10:%4d, fs:%1d, lf:%1d, enc:%d.%d.%d\r\n",
        head,
        vtail_ch[0], vtail_ch[1], vtail_ch[2], vtail_ch[3],
        g_sbus2.getChannel(4), g_sbus2.getChannel(5),
        g_sbus2.getChannel(6), g_sbus2.getChannel(7),
        g_sbus2.getChannel(8), g_sbus2.getChannel(9),
        g_sbus2.isFailsafe(), g_sbus2.isLostframe(),
        g_enc_boom.is_initialized(), g_enc_boom.is_on_upper_side(), g_enc_boom.get_angle()
    );
#elif defined(SERIAL_OUT_CONTROL_STATUS)
    Serial.printf(
        "%c 1:%+3.02f, 2:%+3.02f, 3:%+3.02f, 4:%+3.02f, b:%+3.02f, r:%+3.02f, "
        "m:%+3.02f, slp:%4d, 9:%4d,10:%4d, fs:%1d, lf:%1d, enc:%d.%d.%d\r\n",
        head,
        g_sbus2_ch[0], g_sbus2_ch[1], g_sbus2_ch[2], g_sbus2_ch[3],
        g_motor_output[0], g_motor_output[1],
        sqrt(g_movement_power_square), g_start_pose_sleep_counter,
        g_sbus2.getChannel(8), g_sbus2.getChannel(9),
        g_sbus2.isFailsafe(), g_sbus2.isLostframe(),
        g_enc_boom.is_initialized(), g_enc_boom.is_on_upper_side(), g_enc_boom.get_angle()
    );
#endif
#endif

    // LCDにロボット状態を表示
    if(g_robot_status != RobotStatus::RSTAT_WAITING_STABILIZED && g_robot_status != RobotStatus::RSTAT_COUNTING_DOWN){
        M5.Display.fillRect(0, 0, M5.Lcd.width(), g_lcd_bottom_rect_y1, BLACK);
        M5.Display.setCursor(0, 0);
        M5.Display.printf("robot status: %d\n", g_robot_status);
        M5.Display.printf("control status: %d\n", g_control_status);
    }
}
