#include "global_constants.hpp"
#include "global_variables.hpp"
#include "task_controller.hpp"
#include "tasks/core_counter_context.hpp"
#include "peripherals/pwm_out.hpp"
#include "peripherals/enc_reciever.hpp"

#include <Sbus2Reciever.hpp>

#include <Arduino.h>
#include <M5Unified.h>



/**********************************************************************
 * Constants and macros
 *********************************************************************/
#define NUMBER_OF_TASKS 4   // タスクの数



/**********************************************************************
 * Global variables
 *********************************************************************/
static std::array<AbstractRtosTaskContextSharedPtr, NUMBER_OF_TASKS> task_configs;



/**********************************************************************
 * API Functions
 *********************************************************************/
void calculate_global_constants(){
    lcd_bottom_rect_x1 = M5.Lcd.width()*0.9;
    lcd_bottom_rect_x2 = M5.Lcd.width()*0.95;
    lcd_bottom_rect_x3 = M5.Lcd.width()*1.0;
    lcd_bottom_rect_y1 = M5.Lcd.height()*0.95;
    lcd_bottom_rect_y2 = M5.Lcd.height()*1.0;
}

float conv_sbus2_to_float(uint16_t val, int16_t offset = 0, bool invert = false){
    float ret = ((val + offset) / 1024.0 - 1.0) * 1.57;
    if(ret > 1.0){ ret = 1.0; }
    if(ret < -1.0){ ret = -1.0; }
    if(invert == true){
        ret *= -1.0;
    }
    return ret;
}



/**********************************************************************
 * Task functions
 *********************************************************************/


class ParseSerialsContext : public AbstractRtosTaskContext
{
public:
    ParseSerialsContext(RtosTaskConfigSharedPtr config) : AbstractRtosTaskContext(config) {};
    void onExecute()
    {
        sbus2.parse();
        enc_boom.parse();
    }
};

class DrawInfoContext : public AbstractRtosTaskContext
{
public:
    DrawInfoContext(RtosTaskConfigSharedPtr config) : AbstractRtosTaskContext(config) {};
    void onExecute()
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
        sbus2.getVtailChannel(&vtail_ch[0], &vtail_ch[1], &vtail_ch[2], &vtail_ch[3]);

    #if defined(SERIAL_OUT_SBUS2_RAW)
        Serial.printf(
            "%c 1:%4d, 2:%4d, 3:%4d, 4:%4d, 5:%4d, 6:%4d, "
            "7:%4d, 8:%4d, 9:%4d,10:%4d, fs:%1d, lf:%1d, enc:%d.%d.%d\r\n",
            head,
            vtail_ch[0], vtail_ch[1], vtail_ch[2], vtail_ch[3],
            sbus2.getChannel(4), sbus2.getChannel(5),
            sbus2.getChannel(6), sbus2.getChannel(7),
            sbus2.getChannel(8), sbus2.getChannel(9),
            sbus2.isFailsafe(), sbus2.isLostframe(),
            enc_boom.is_initialized(), enc_boom.is_on_upper_side(), enc_boom.get_angle()
        );
    #elif defined(SERIAL_OUT_CONTROL_STATUS)
        Serial.printf(
            "%c 1:%+3.02f, 2:%+3.02f, 3:%+3.02f, 4:%+3.02f, b:%+3.02f, r:%+3.02f, "
            "m:%+3.02f, slp:%4d, 9:%4d,10:%4d, fs:%1d, lf:%1d, enc:%d.%d.%d\r\n",
            head,
            sbus2_ch[0], sbus2_ch[1], sbus2_ch[2], sbus2_ch[3],
            motor_output[0], motor_output[1],
            sqrt(movement_power_square), start_pose_sleep_counter,
            sbus2.getChannel(8), sbus2.getChannel(9),
            sbus2.isFailsafe(), sbus2.isLostframe(),
            enc_boom.is_initialized(), enc_boom.is_on_upper_side(), enc_boom.get_angle()
        );
    #endif
    #endif

        // LCDにロボット状態を表示
        if(robot_status != RobotStatus::RSTAT_WAITING_STABILIZED && robot_status != RobotStatus::RSTAT_COUNTING_DOWN){
            M5.Display.fillRect(0, 0, M5.Lcd.width(), lcd_bottom_rect_y1, BLACK);
            M5.Display.setCursor(0, 0);
            M5.Display.printf("robot status: %d\n", robot_status);
            M5.Display.printf("control status: %d\n", control_status);
        }
    }
};

// 制御ループ
class ControlLoopContext : public AbstractRtosTaskContext
{
public:
    ControlLoopContext(RtosTaskConfigSharedPtr config) : AbstractRtosTaskContext(config) {};
    void onExecute()
    {
        //////////////////////////////////////////////////////////////
        // 入力取得と状態遷移
        //////////////////////////////////////////////////////////////

        // 初期化中は制御ループはskipする
        bool is_robot_status_not_control =
            robot_status == RobotStatus::RSTAT_WAITING_STABILIZED
            || robot_status == RobotStatus::RSTAT_COUNTING_DOWN
            || robot_status == RobotStatus::RSTAT_INITIALIZING;
        if(is_robot_status_not_control) return;

        // プロポの入力を取得
        int16_t vtail_ch[4] = { 0 };
        sbus2.getVtailChannel(&vtail_ch[0], &vtail_ch[1], &vtail_ch[2], &vtail_ch[3]);
        sbus2_ch[0] = conv_sbus2_to_float(vtail_ch[0], 0, false);
        sbus2_ch[1] = conv_sbus2_to_float(vtail_ch[1], 0, true);
        sbus2_ch[2] = conv_sbus2_to_float(vtail_ch[2], 0, false);
        sbus2_ch[3] = conv_sbus2_to_float(vtail_ch[3], 32, false);
        float move_square = sbus2_ch[1] * sbus2_ch[1] + sbus2_ch[3] * sbus2_ch[3];
        movement_power_square = move_square;

        // スリープボタンのカウンタを更新
        if(SBUS2_BUTON_IS_HIGH(sbus2.getChannel(9))){
            if(start_pose_sleep_counter <= SLEEP_BUTTON_SLEEP_THRESHOLD){
                start_pose_sleep_counter++;
            }
        }else{
            start_pose_sleep_counter = 0;
        }

        // 状態遷移を実行
        if(start_pose_sleep_counter >= SLEEP_BUTTON_SLEEP_THRESHOLD){
            robot_status = RobotStatus::RSTAT_SLEEPING;
        }else if(start_pose_sleep_counter >= SLEEP_BUTTON_START_POSE_THRESHOLD){
            robot_status = RobotStatus::RSTAT_STARTING_POSE;
        }
        if(move_square > POW2(UNSLEEP_MOVE_SQRT_THRESHOLD)){
            robot_status = RobotStatus::RSTAT_NORMAL;
        }

        // Sleep中はモーター出力を0にする
        if(robot_status == RobotStatus::RSTAT_SLEEPING){
            motor_boom.out(0.0);
            motor_roll.out(0.0);
            return;
        }


        //////////////////////////////////////////////////////////////
        // ロボットのroll軸の制御
        //////////////////////////////////////////////////////////////

        // roll出力を計算
        bool roll_attaking = false;
        float roll_input = sbus2_ch[0];
        float roll_input_abs = fabs(roll_input);
        float roll_output = 0.0;
        if(robot_status == RobotStatus::RSTAT_STARTING_POSE){
            if(roll_input_abs < ROLL_DEADZONE){
                roll_output = 0.0;
            }
            else if(roll_input < 0.0){
                roll_output = - ROLL_ADJ_SPEED;
            }
            else
            {
                roll_output = ROLL_ADJ_SPEED;
            }
        }else{
            if(roll_input_abs < ROLL_DEADZONE){
                roll_output = 0.0;
            }
            else if(roll_input > ROLL_ADJ_INPUT_MAX)
            {
                roll_attaking = true;
                roll_output = ROLL_ROLLING_SPEED;
            }
            else if(roll_input < 0.0){
                roll_output = - ROLL_ADJ_SPEED;
            }
            else
            {
                roll_output = ROLL_ADJ_SPEED;
            }
        }

        // roll出力をモーターに出力
        motor_roll.out(roll_output);
        motor_output[1] = roll_output;


        //////////////////////////////////////////////////////////////
        // ロボットのboom軸の制御
        //////////////////////////////////////////////////////////////

        // boomの制御状態を更新
        if(roll_attaking == true){
            control_status = ControlStatus::CSTAT_ROLLING;
        }else if(move_square > POW2(BOOM_UP_MOVE_SQRT_THRESHOLD)){
            control_status = ControlStatus::CSTAT_BOOM_UP_MOVING;
        }else{
            control_status = ControlStatus::CSTAT_NORMAL;
        }

        // boom出力を計算
        int32_t boom_target = 0 + (int)(2000 * sbus2_ch[2]);
        int32_t diff = boom_target - enc_boom.get_angle();

        float boom_output = diff * 0.0005;
        if(boom_output > 0.2){ boom_output = 0.2; }
        if(boom_output < -0.2){ boom_output = -0.2; }
        motor_boom.out(boom_output);
        motor_output[0] = boom_output;
    }
};



/**********************************************************************
 * intialize and loop
 *********************************************************************/
void setup() {
    // M5Unifiedの初期化
    auto cfg = M5.config();
    M5.begin(cfg);

    // フォントサイズを初期化
    M5.Display.setTextSize(1);
    M5.Display.print("initializing...\n");

    // 定数を計算
    calculate_global_constants();

    // デバッグ用シリアルポートを初期化
    Serial.begin(921600);

    // Serial1をSBUS2受信用に設定(8E2で25byte)
    Serial1.begin(100000, SERIAL_8E2, 5, -1, true); // Serial1 for SBUS2
    pinMode(5, INPUT_PULLDOWN);
    int16_t sbus_init = sbus2.begin(&Serial1);
    if(sbus_init != 0){
        M5.Display.printf("sbus2 failed: %d\n", sbus_init);
        while(1);
    };

    // エンコーダ用にSerial2を設定
    Serial2.begin(921600, SERIAL_8N1, 8, -1); // Serial2 for enc
    if(enc_boom.begin(&Serial2, true) != true){
        M5.Display.printf("enc_boom failed\n");
        while(1);
    }

    // モーター出力を初期化
    motor_boom.begin(38, 0, true);
    motor_roll.begin(39, 1);

    task_configs[0] = std::make_shared<ParseSerialsContext>(
        std::make_shared<RtosTaskConfig_typedef>(RtosTaskConfig_typedef{
            .start_required = true,
            .name           = "task_serial_parser",
            .thand          = NULL,
            .repeated       = pdTRUE,
            .period         = pdMS_TO_TICKS(5),
            .initial        = pdMS_TO_TICKS(0),
            .stack_size     = 8096,
            .priority       = 1,
            .core_id        = APP_CPU_NUM,
        })
    );
    task_configs[1] = std::make_shared<DrawInfoContext>(
        std::make_shared<RtosTaskConfig_typedef>(RtosTaskConfig_typedef{
            .start_required = true,
            .name           = "task_draw_info",
            .thand          = NULL,
            .repeated       = pdTRUE,
            .period         = pdMS_TO_TICKS(200),
            .initial        = pdMS_TO_TICKS(0),
            .stack_size     = 8096,
            .priority       = 1,
            .core_id        = APP_CPU_NUM,
        })
    );
    task_configs[2] = std::make_shared<ControlLoopContext>(
        std::make_shared<RtosTaskConfig_typedef>(RtosTaskConfig_typedef{
            .start_required = true,
            .name           = "task_control_loop",
            .thand          = NULL,
            .repeated       = pdTRUE,
            .period         = pdMS_TO_TICKS(2),
            .initial        = pdMS_TO_TICKS(0),
            .stack_size     = 8096,
            .priority       = 1,
            .core_id        = APP_CPU_NUM,
        })
    );
    task_configs[3] = std::make_shared<Core1CounterContext>(
        std::make_shared<RtosTaskConfig_typedef>(RtosTaskConfig_typedef{
            .start_required = true,
            .name           = "task_core1_counter",
            .thand          = NULL,
            .repeated       = pdTRUE,
            .period         = pdMS_TO_TICKS(500),
            .initial        = pdMS_TO_TICKS(0),
            .stack_size     = 1024,
            .priority       = 5,
            .core_id        = APP_CPU_NUM,
        })
    );

    // エンコーダ入力をクリアしてからタスクの処理を開始
    M5.Display.printf("\nstarting all tasks\n");
    enc_boom.flush_rx();
    task_start(task_configs.data(), task_configs.size());

    // タスクの実行状態を表示
    for(int i = 0; i < task_configs.size(); i++){
        if(task_configs[i]->isStarted()){
            M5.Display.printf("-[T]%.*s\n", MAX_CHARS_DISPLAY_WIDTH, task_configs[i]->getConfig()->name);
        }else{
            M5.Display.printf("-[ ]%.*s\n", MAX_CHARS_DISPLAY_WIDTH, task_configs[i]->getConfig()->name);
        }
    }
    M5.Display.print("[[task started]]\n");

    // タスクが正常に開始されるまで待機
    M5.Display.print("waiting stablized\n");
    while(core1_alive_count < LOOP_ALIVE_COUNT_THRESHOLD){
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // 制御データを正常受信するまで待機
    M5.Display.print("waiting comms\n");
    while(sbus2.isLostframe() == true || enc_boom.is_recieved() == false){
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // count down
    robot_status = RobotStatus::RSTAT_COUNTING_DOWN;
    char buf[3] = { ' ', '5', '\0' };
    for(int i = 5; i > 0; i--){
        M5.Display.print(buf);
        buf[1]--;
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    // 制御開始
    robot_status = RobotStatus::RSTAT_SLEEPING;
}



void loop() {
    vTaskDelay(100);
    M5.Display.fillRect(lcd_bottom_rect_x1, lcd_bottom_rect_y1, lcd_bottom_rect_x2, lcd_bottom_rect_y2, WHITE);
    M5.Display.fillRect(lcd_bottom_rect_x2, lcd_bottom_rect_y1, lcd_bottom_rect_x3, lcd_bottom_rect_y2, BLACK);

    vTaskDelay(100);
    M5.Display.fillRect(lcd_bottom_rect_x1, lcd_bottom_rect_y1, lcd_bottom_rect_x2, lcd_bottom_rect_y2, BLACK);
    M5.Display.fillRect(lcd_bottom_rect_x2, lcd_bottom_rect_y1, lcd_bottom_rect_x3, lcd_bottom_rect_y2, WHITE);
}
