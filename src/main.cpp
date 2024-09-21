#include "global_constants.hpp"
#include "global_variables.hpp"
#include "task_controller.hpp"
#include "tasks/core_counter_context.hpp"
#include "tasks/parse_serials_context.hpp"
#include "tasks/serial_command_executor_context.hpp"
#include "tasks/control_loop_context.hpp"
#include "tasks/update_lcd_context.hpp"
#include "peripherals/pwm_out.hpp"
#include "peripherals/enc_reciever.hpp"

#include <Sbus2Reciever.hpp>

#include <Arduino.h>
#include <M5Unified.h>



/**********************************************************************
 * Constants, macros and in-file global variables
 *********************************************************************/
#define NUMBER_OF_TASKS 5   // タスクの数
static std::array<AbstractRtosTaskContextSharedPtr, NUMBER_OF_TASKS> task_configs;



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
    int16_t sbus_init = g_sbus2.begin(&Serial1);
    if(sbus_init != 0){
        M5.Display.printf("g_sbus2 failed: %d\n", sbus_init);
        while(1);
    };

    // エンコーダ用にSerial2を設定
    Serial2.begin(921600, SERIAL_8N1, 8, -1); // Serial2 for enc
    if(g_enc_boom.begin(&Serial2, true) != true){
        M5.Display.printf("g_enc_boom failed\n");
        while(1);
    }

    // モーター出力を初期化
    g_motor_boom.begin(38, 0, true);
    g_motor_roll.begin(39, 1);

    task_configs[0] = std::make_shared<ParseSerialsContext>(
        std::make_shared<RtosTaskConfig_typedef>(RtosTaskConfig_typedef{
            .start_required = true,
            .name           = "task_serial_parser",
            .thand          = NULL,
            .repeated       = pdTRUE,
            .period         = pdMS_TO_TICKS(5),
            .initial        = pdMS_TO_TICKS(0),
            .stack_size     = 8096,
            .priority       = 2,
            .core_id        = APP_CPU_NUM,
        })
    );
    task_configs[1] = std::make_shared<SerialCommandExecutorContext>(
        std::make_shared<RtosTaskConfig_typedef>(RtosTaskConfig_typedef{
            .start_required = true,
            .name           = "task_serial_command_executor",
            .thand          = NULL,
            .repeated       = pdTRUE,
            .period         = pdMS_TO_TICKS(100),
            .initial        = pdMS_TO_TICKS(2000),
            .stack_size     = 8096,
            .priority       = 4,
            .core_id        = PRO_CPU_NUM,
        }),
        &Serial
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
    task_configs[3] = std::make_shared<UpdateLcdContext>(
        std::make_shared<RtosTaskConfig_typedef>(RtosTaskConfig_typedef{
            .start_required = true,
            .name           = "task_update_lcd_context",
            .thand          = NULL,
            .repeated       = pdTRUE,
            .period         = pdMS_TO_TICKS(100),
            .initial        = pdMS_TO_TICKS(0),
            .stack_size     = 8096,
            .priority       = 2,
            .core_id        = PRO_CPU_NUM,
        }),
        &M5.Display
    );
    task_configs[4] = std::make_shared<Core1CounterContext>(
        std::make_shared<RtosTaskConfig_typedef>(RtosTaskConfig_typedef{
            .start_required = true,
            .name           = "task_core1_counter",
            .thand          = NULL,
            .repeated       = pdTRUE,
            .period         = pdMS_TO_TICKS(100),
            .initial        = pdMS_TO_TICKS(0),
            .stack_size     = 1024,
            .priority       = 5,
            .core_id        = APP_CPU_NUM,
        })
    );

    // エンコーダ入力をクリアしてからタスクの処理を開始
    M5.Display.printf("\nstarting all tasks\n");
    g_enc_boom.flush_rx();
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
    while(g_core1_alive_count < LOOP_ALIVE_COUNT_THRESHOLD){
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // 制御データを正常受信するまで待機
    M5.Display.print("waiting comms\n");
    while(g_sbus2.isLostframe() == true || g_enc_boom.is_recieved() == false){
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // count down
    g_robot_status = RobotStatus::RSTAT_COUNTING_DOWN;
    char buf[3] = { ' ', '5', '\0' };
    for(int i = 5; i > 0; i--){
        M5.Display.print(buf);
        buf[1]--;
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    // エンコーダの位置の初期化行動を実行
    if(!g_enc_boom.is_initialized()){
        M5.Display.print("\nenc initializing");
        g_robot_status = RobotStatus::RSTAT_INITIALIZING;

        if(g_enc_boom.is_on_upper_side()){
            M5.Display.print("__");
            g_motor_boom.out(-0.1);
        }else{
            M5.Display.print("^^");
            g_motor_boom.out(0.1);
        }

        while(!g_enc_boom.is_initialized()){
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        M5.Display.print("\n  -> done!\n");
        g_motor_boom.out(0);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // 制御開始
    g_robot_status = RobotStatus::RSTAT_SLEEPING;
}



void loop() {
    vTaskDelay(1000);
}
