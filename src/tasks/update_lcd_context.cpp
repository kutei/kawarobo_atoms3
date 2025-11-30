#include "tasks/update_lcd_context.hpp"
#include "global_variables.hpp"

UpdateLcdContext::UpdateLcdContext(RtosTaskConfigSharedPtr config, M5GFX *disp)
    : AbstractRtosTaskContext(config), _disp(disp), _canvas(disp)
{
    int box_size = this->_disp->width() * 0.05;
    this->_brect_x1 = this->_disp->width() - box_size * 2;
    this->_brect_x2 = this->_disp->width() - box_size;
    this->_brect_y = this->_disp->height() - box_size;
    this->_brect_w = box_size;
    this->_brect_h = box_size;
    this->_canvas.createSprite(this->_disp->width(), this->_disp->height());
};

void UpdateLcdContext::onExecute()
{
    // 定数設定
    constexpr int BAR_HEIGHT = 20;
    constexpr int BAR_RIM_SIZE = 2;
    constexpr int BAR_SIZE_DEC = BAR_RIM_SIZE * 2;
    constexpr int BAR_HEIGHT_SIZE = BAR_HEIGHT - BAR_SIZE_DEC;
    static int inner_bar_width = this->_disp->width() - BAR_SIZE_DEC;

    // カウントアップ
    this->_update_counter++;

    // 通常状態の場合にのみ表示する
    bool is_normal_status =
        g_robot_status == RobotStatus::RSTAT_SLEEPING ||
        g_robot_status == RobotStatus::RSTAT_STARTING_POSE_READY ||
        g_robot_status == RobotStatus::RSTAT_STARTING_POSE ||
        g_robot_status == RobotStatus::RSTAT_NORMAL;
    if(!is_normal_status){
        return;
    }

    // 背景色を状態に合わせて変更
    uint16_t bg_color;
    if(g_robot_status == RobotStatus::RSTAT_SLEEPING){
        bg_color = BLACK;
    }else if(g_robot_status == RobotStatus::RSTAT_STARTING_POSE){
        bg_color = DARKGREEN;
    }else if(g_robot_status == RobotStatus::RSTAT_STARTING_POSE_READY){
        if(this->_update_counter % 2 == 0){
            bg_color = GREEN;
        }else{
            bg_color = DARKGREEN;
        }
    }else if(g_robot_status == RobotStatus::RSTAT_NORMAL){
        if(g_control_status == ControlStatus::CSTAT_NORMAL){
            bg_color = GREEN;
        }else if(g_control_status == ControlStatus::CSTAT_ROLLING){
            bg_color = RED;
        }else if(g_control_status == ControlStatus::CSTAT_BOOM_UP_MOVING){
            bg_color = NAVY;
        }
    }

    // ボタン操作に対応してタスク実行など
    if(M5.BtnA.isHolding()){
        _entered();
    }
    if(M5.BtnA.wasClicked()){
        _nextMenu();
    }

    // 背景を塗りつぶし
    this->_canvas.fillScreen(bg_color);

    // 操作情報を表示
    this->_canvas.fillRect(0, 0, this->_canvas.width(), BAR_HEIGHT, YELLOW);
    this->_canvas.fillRect(BAR_RIM_SIZE, BAR_RIM_SIZE, inner_bar_width, BAR_HEIGHT_SIZE, WHITE);
    if(M5.BtnA.isPressed()){
        float bar_ratio = (float)(millis() - M5.BtnA.lastChange()) / M5.BtnA.getHoldThresh();
        if(bar_ratio > 1.0f) bar_ratio = 1.0f;
        this->_canvas.fillRect(BAR_RIM_SIZE, BAR_RIM_SIZE, bar_ratio * inner_bar_width, BAR_HEIGHT_SIZE, CYAN);
    }
    // 選択されているメニューを表示
    this->_canvas.setTextSize(1);
    this->_canvas.setTextColor(BLACK, WHITE);
    int text_x = this->_disp->width()/2;
    int text_y = BAR_HEIGHT/2 - this->_canvas.fontHeight()/2;
    if(this->_menu_index == Menu::MENU_CALIBRATE_IMU) {
        this->_canvas.drawCenterString("0: Calibrate IMU", text_x, text_y);
    }else if (this->_menu_index == Menu::MENU_DELETE_CALIB_DATA)
    {
        this->_canvas.drawCenterString("1: Delete Calib Data", text_x, text_y);
    }


    // ロボット情報を表示
    this->_canvas.setTextSize(1);
    this->_canvas.setTextColor(WHITE, BLACK);
    this->_canvas.setTextDatum(top_left);
    this->_canvas.setCursor(0, BAR_HEIGHT + 1);
    this->_canvas.printf("robot status: %d\n", g_robot_status);
    this->_canvas.printf("control status: %d\n", g_control_status);
    this->_canvas.printf("exec_time: %04lu [us]\n", g_control_loop_time.exec_time);
    this->_canvas.printf("interval : %04lu [us]\n", g_control_loop_time.interval_time);
    this->_canvas.printf("IMU 3-axis: \n  %.2f\n  %.2f\n  %.2f\n", g_imu_euler_angle.roll, g_imu_euler_angle.pitch, g_imu_euler_angle.yaw);
    this->_canvas.printf("imu: %.2f [ms]\n", g_imu_calc_time.interval_time/1000.0);

    // core1カウンタの状態でチラつく四角形を表示
    if(g_core1_alive_count % 2 == 0){
        this->_canvas.fillRect(this->_brect_x1, this->_brect_y, this->_brect_w, this->_brect_h, WHITE);
    }else{
        this->_canvas.fillRect(this->_brect_x2, this->_brect_y, this->_brect_w, this->_brect_h, WHITE);
    }

    // LCDへ描画
    _pushToDisplay();
}


void UpdateLcdContext::_entered()
{
    if(this->_menu_index == Menu::MENU_CALIBRATE_IMU){
        // 白文字に設定
        this->_canvas.setTextColor(WHITE, BLACK);

        this->_canvas.fillScreen(BLACK);
        this->_canvas.drawRect(
            this->_disp->width()*0.1, this->_disp->height()*0.1,
            this->_disp->width()*0.8, this->_disp->height()*0.8, WHITE);
        this->_canvas.setTextSize(2);
        this->_canvas.drawCenterString(
            "Calib", this->_disp->width()/2, this->_disp->height()/2 - this->_canvas.fontHeight());
        this->_canvas.drawCenterString("IMU", this->_disp->width()/2, this->_disp->height()/2);
        this->_pushToDisplay();
        delay(2000);

        // カウントダウン
        this->_canvas.setTextSize(5);
        for(int i = 5; i >= 1; i--){
            this->_canvas.fillScreen(BLACK);
            this->_canvas.drawCenterString(
                String(i), this->_disp->width()/2, this->_disp->height()/2 - this->_canvas.fontHeight()/2);
            this->_pushToDisplay();
            delay(1000);
        }
        this->_canvas.fillScreen(BLACK);
        this->_canvas.setTextSize(2);
        this->_canvas.drawCenterString(
            "Calib...", this->_disp->width()/2, this->_disp->height()/2 - this->_canvas.fontHeight()/2);
        this->_pushToDisplay();

        // IMUキャリブレーション実行
        M5.Imu.setCalibration(255, 255, 255);
        delay(2000); // キャリブレーション完了待ち
        M5.Imu.setCalibration(0, 0, 0);
        M5.Imu.saveOffsetToNVS();

    }else if(this->_menu_index == Menu::MENU_DELETE_CALIB_DATA){
        // 白文字に設定
        this->_canvas.setTextColor(WHITE, BLACK);

        this->_canvas.fillScreen(BLACK);
        this->_canvas.drawRect(
            this->_disp->width()*0.1, this->_disp->height()*0.1,
            this->_disp->width()*0.8, this->_disp->height()*0.8, WHITE);
        this->_canvas.setTextSize(2);

        this->_canvas.drawCenterString(
            "Delete", this->_disp->width()/2, this->_disp->height()/2 - this->_canvas.fontHeight());
        this->_canvas.drawCenterString("Calib Data", this->_disp->width()/2, this->_disp->height()/2);
        this->_pushToDisplay();
        delay(2000);

        // キャリブレーションデータ削除
        M5.Imu.clearOffsetData();   // オフセットデータを0にクリア
        M5.Imu.saveOffsetToNVS();   // クリアした値をNVSに保存
    }

    // 終了表示
    this->_canvas.fillScreen(BLACK);
    this->_canvas.setTextSize(2);
    this->_canvas.drawCenterString(
        "Done", this->_disp->width()/2, this->_disp->height()/2 - this->_canvas.fontHeight()/2);
    this->_pushToDisplay();
    delay(2000);
}

void UpdateLcdContext::_nextMenu()
{
    // 短押し確定時の処理
    this->_menu_index++;
    if(this->_menu_index >= Menu::MENU_SIZE){
        this->_menu_index = 0;
    }
}

void UpdateLcdContext::_pushToDisplay()
{
    // LCDへ描画
    this->_canvas.pushSprite(0, 0);
}
