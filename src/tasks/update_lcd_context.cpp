#include "tasks/update_lcd_context.hpp"
#include "global_variables.hpp"

UpdateLcdContext::UpdateLcdContext(RtosTaskConfigSharedPtr config, M5GFX *disp)
    : AbstractRtosTaskContext(config), _disp(disp)
{
    int box_size = this->_disp->width() * 0.05;
    this->_brect_x1 = this->_disp->width() - box_size * 2;
    this->_brect_x2 = this->_disp->width() - box_size;
    this->_brect_y = this->_disp->height() - box_size;
    this->_brect_w = box_size;
    this->_brect_h = box_size;
};

void UpdateLcdContext::onExecute()
{
    // 通常状態の場合にのみ表示する
    bool is_normal_status =
        g_robot_status == RobotStatus::RSTAT_SLEEPING ||
        g_robot_status == RobotStatus::RSTAT_STARTING_POSE ||
        g_robot_status == RobotStatus::RSTAT_NORMAL;

    if(!is_normal_status){
        return;
    }

    // LCDにロボット状態を表示
    this->_disp->fillRect(0, 0, this->_disp->width(), this->_disp->height(), BLACK);
    this->_disp->setCursor(0, 0);
    this->_disp->printf("robot status: %d\n", g_robot_status);
    this->_disp->printf("control status: %d\n", g_control_status);

    // core1カウンタの状態でチラつく四角形を表示
    if(g_core1_alive_count % 2 == 0){
        this->_disp->fillRect(this->_brect_x1, this->_brect_y, this->_brect_w, this->_brect_h, WHITE);
    }else{
        this->_disp->fillRect(this->_brect_x2, this->_brect_y, this->_brect_w, this->_brect_h, WHITE);
    }
}
